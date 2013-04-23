/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mal.hpp"
#include "gui/password_dialog.hpp"
#include <curl/curl.h>

namespace {
	extern "C" {
		static size_t curl_write_function(void *buffer, size_t size, size_t nmemb, void *userp) {
			auto const data = std::string(static_cast<char*>(buffer), size*nmemb);
			auto const buf = static_cast<std::string*>(userp);
			buf->append(std::move(data));

			return size*nmemb;
		}

		static void mal_curl_lock_function(CURL* curl, curl_lock_data data, curl_lock_access access, void* userp) {
			auto functor_pair_p = static_cast<MAL::MAL::pair_lock_functor_t*>(userp);
			functor_pair_p->first(curl, data, access);
		}

		static void mal_curl_unlock_function(CURL* curl, curl_lock_data data, void* userp) {
			auto functor_pair_p = static_cast<MAL::MAL::pair_lock_functor_t*>(userp);
			functor_pair_p->second(curl, data);
		}
	}
}

namespace MAL {
	MAL::MAL(std::unique_ptr<UserInfo>&& info) :
		user_info(std::move(info)),
        text_util(std::make_shared<TextUtility>()),
        serializer(text_util),
        manga_serializer(text_util),
		curl_ebuffer(new char[CURL_ERROR_SIZE]),
		share_lock_functors(new pair_lock_functor_t(std::bind(&MAL::involke_lock_function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
		                                            std::bind(&MAL::involke_unlock_function, this, std::placeholders::_1, std::placeholders::_2))),
		curl_share(curl_share_init())
	{
		signal_run_password_dialog.connect(sigc::mem_fun(*this, &MAL::run_password_dialog));
		CURLSHcode code;

		code = curl_share_setopt(curl_share.get(), CURLSHOPT_LOCKFUNC, &mal_curl_lock_function);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_UNLOCKFUNC, &mal_curl_unlock_function);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_USERDATA, share_lock_functors.get());
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}
		code = curl_share_setopt(curl_share.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
		if (code != CURLSHE_OK) {
			print_curl_share_error(code);
		}

		if (!user_info->has_details()) {
			run_password_dialog();
		}
	}

	void MAL::run_password_dialog() {
		PasswordDialog dialog;
		auto res = dialog.run();
		if (res == 1) {
			if (dialog.get_username().size() > 0 && dialog.get_password().size() > 0) {
				user_info->set_details(dialog.get_username(), dialog.get_password());
			} else {
				std::cerr << "Error: Username and/or password were left blank." << std::endl;
			}
		} else {
			std::cerr << "Error: Password dialog had unexpected exit." << std::endl;
		}
	}

	void MAL::setup_curl_easy(CURL* easy, const std::string& url, std::string* buffer) {
		CURLcode code;
		code = curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, &curl_write_function);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_WRITEDATA, buffer);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_ERRORBUFFER, curl_ebuffer.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_FAILONERROR, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_URL, url.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(easy, CURLOPT_SHARE, curl_share.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
	}
	
	std::list<std::shared_ptr<Anime> > MAL::get_anime_list_sync() {
		std::list<std::shared_ptr<Anime> > out;
		if (!user_info->get_username()) {
			std::cerr << "Error: No username provided." << std::endl;
			return out;
		}

		const std::string url = LIST_BASE_URL + user_info->get_username().get() + "&status=all&type=anime";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());

		setup_curl_easy(curl.get(), url, buf.get());
		CURLcode code = curl_easy_perform(curl.get());
		
		if (code != CURLE_OK) {
			print_curl_error(code);
			return out;
		}

		text_util->parse_html_entities(*buf);
		out = serializer.deserialize(*buf);

		return out;
	}

	std::list<std::shared_ptr<Manga> > MAL::get_manga_list_sync() {
		std::list<std::shared_ptr<Manga> > out;
		if (!user_info->get_username()) {
			std::cerr << "Error: No username provided." << std::endl;
			return out;
		}

		const std::string url = LIST_BASE_URL + user_info->get_username().get() + "&status=all&type=manga";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());

		setup_curl_easy(curl.get(), url, buf.get());
		CURLcode code = curl_easy_perform(curl.get());
		
		if (code != CURLE_OK) {
			print_curl_error(code);
			return out;
		}

		text_util->parse_html_entities(*buf);
		out = manga_serializer.deserialize(*buf);

		return out;
	}

	std::string MAL::get_image_sync(const MALItem& item) {
        auto iter = image_cache.find(item.image_url);
        if (iter == std::end(image_cache)) {
            std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
            std::unique_ptr<std::string> buf(new std::string());
            setup_curl_easy(curl.get(), item.image_url, buf.get());
            CURLcode code = curl_easy_perform(curl.get());
            
            if (code != CURLE_OK) {
                print_curl_error(code);
                return std::string();
            } else {
                auto inserted = image_cache.insert(std::make_pair(item.image_url, std::move(buf)));
                return *inserted.first->second;
            }
        } else {
            return *iter->second;
        }
	}

	std::string MAL::get_manga_image_sync(const Manga& manga) {
        auto iter = manga_image_cache.find(manga.series_itemdb_id);
        if (iter == std::end(manga_image_cache)) {
            std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
            std::unique_ptr<std::string> buf(new std::string());
            setup_curl_easy(curl.get(), manga.image_url, buf.get());
            CURLcode code = curl_easy_perform(curl.get());
            
            if (code != CURLE_OK) {
                print_curl_error(code);
                return std::string();
            } else {
                auto inserted = manga_image_cache.insert(std::make_pair(manga.series_itemdb_id, std::move(buf)));
                return *inserted.first->second;
            }
        } else {
            return *iter->second;
        }
	}

	std::list<std::shared_ptr<Anime> > MAL::search_anime_sync(const std::string& terms) {
		std::list<std::shared_ptr<Anime> > res;
		
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		std::unique_ptr<char, CURLEscapeDeleter> terms_escaped(curl_easy_escape(curl.get(), terms.c_str(), terms.size()));
		const std::string url = SEARCH_BASE_URL + terms_escaped.get();

		setup_curl_easy(curl.get(), url, buf.get());
		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (code != CURLE_OK)
				print_curl_error(code);
			else if (res == 401) {
				signal_run_password_dialog();
			}
		}

		text_util->parse_html_entities(*buf);
        if (buf->size() > 0)
            res = serializer.deserialize(*buf);
        else
            std::cerr << "Info: Null response for search terms \"" << terms 
                      << "\"." << std::endl;
		
		return res;
	}

	std::list<std::shared_ptr<Manga> > MAL::search_manga_sync(const std::string& terms) {
		std::list<std::shared_ptr<Manga> > res;

		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		std::unique_ptr<char, CURLEscapeDeleter> terms_escaped(curl_easy_escape(curl.get(), terms.c_str(), terms.size()));
		const std::string url = MANGA_SEARCH_BASE_URL + terms_escaped.get();

		setup_curl_easy(curl.get(), url, buf.get());
		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (code != CURLE_OK)
				print_curl_error(code);
			else if (res == 401) {
				signal_run_password_dialog();
			}
		}

		text_util->parse_html_entities(*buf);
        if (buf->size() > 0)
            res = manga_serializer.deserialize(*buf);
        else
            std::cerr << "Info: Null response for search terms \"" << terms 
                      << "\"." << std::endl;

		return res;
	}

	bool MAL::update_anime_sync(const Anime& anime) {
		const std::string url = UPDATED_BASE_URL + std::to_string(anime.series_itemdb_id) + ".xml";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		setup_curl_easy(curl.get(), url, buf.get());
		auto xml = serializer.serialize(anime);
		xml.insert(0, "data=");

		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, xml.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (code != CURLE_OK)
				print_curl_error(code);
			else if (res == 401) {
				signal_run_password_dialog();
			}
		}

		if (buf->compare("Updated") == 0) 
			return true;
		else {
            std::cerr << "myanimelist.net Error: Response: " << *buf << std::endl;
			return false;
        }
	}

	bool MAL::update_manga_sync(const Manga& manga) {
		const std::string url = MANGA_UPDATED_BASE_URL + std::to_string(manga.series_itemdb_id) + ".xml";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		setup_curl_easy(curl.get(), url, buf.get());
		auto xml = manga_serializer.serialize(manga);
		xml.insert(0, "data=");

		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, xml.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (code != CURLE_OK)
				print_curl_error(code);
			else if (res == 401) {
				signal_run_password_dialog();
			}
		}

		if (buf->compare("Updated") == 0) 
			return true;
		else {
            std::cerr << "myanimelist.net Error: Response: " << *buf << std::endl;
			return false;
        }
	}

	bool MAL::add_anime_sync(const Anime& anime) {
		const std::string url = ADD_BASE_URL + std::to_string(anime.series_itemdb_id) + ".xml";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		setup_curl_easy(curl.get(), url, buf.get());
		auto xml = serializer.serialize(anime);
		xml.insert(0, "data=");
		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, xml.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (res == 401) {
				signal_run_password_dialog();
			}
		}

		std::cerr << "Response: " << *buf << std::endl;
		if (code == CURLE_OK) {
			signal_anime_added();
			return true;
		} else {
			return false;
		}
	}

	bool MAL::add_manga_sync(const Manga& manga) {
		const std::string url = MANGA_ADD_BASE_URL + std::to_string(manga.series_itemdb_id) + ".xml";
		std::unique_ptr<CURL, CURLEasyDeleter> curl(curl_easy_init());
		std::unique_ptr<std::string> buf(new std::string());
		setup_curl_easy(curl.get(), url, buf.get());
		auto xml = manga_serializer.serialize(manga);
		xml.insert(0, "data=");
		CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, xml.c_str());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}
		code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
		if (code != CURLE_OK) {
			print_curl_error(code);
		}

		code = curl_easy_perform(curl.get());
		if (code != CURLE_OK) {
			print_curl_error(code);
			long res = 0;
			curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
			if (res == 401) {
				signal_run_password_dialog();
			}
		}

		std::cerr << "Response: " << *buf << std::endl;
		if (code == CURLE_OK) {
			signal_manga_added();
			return true;
		} else {
			return false;
		}
	}

	void MAL::print_curl_error(CURLcode code) {
			std::cerr << "Error: " << curl_easy_strerror(code)
			          << ", " << curl_ebuffer.get() << std::endl;
	}

	void MAL::print_curl_share_error(CURLSHcode code) {
			std::cerr << "Error: " << curl_share_strerror(code) << std::endl;
	}

	void MAL::involke_lock_function(CURL*, curl_lock_data data, curl_lock_access) {
		auto iter = map_mutex.find(data);
		if (iter == map_mutex.end()) {
			auto res = map_mutex.insert(std::move(std::make_pair(data, std::move(std::unique_ptr<std::mutex>(new std::mutex())))));
			if (!res.second) {
				std::cerr << "Error: Trying to lock curl data but we can't create the mutex" << std::endl;
				return;
			}
			iter = res.first;
		}

		iter->second->lock();
	}

	void MAL::involke_unlock_function(CURL*, curl_lock_data data) {
		auto iter = map_mutex.find(data);
		if (iter == map_mutex.end()) {
			std::cerr << "Error: Trying to unlock curl data but we don't have the mutex." << std::endl;
			return;
		}

		iter->second->unlock();
	}
}
