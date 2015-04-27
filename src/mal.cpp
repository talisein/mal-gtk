
/* -*- mode: c++; c-file-style: "linux"; c-basic-offset: 4; indent-tabs-mode: nil; -*-
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
#include <giomm/file.h>
#include <glibmm/miscutils.h>
#include <glibmm.h>
#include <chrono>
#include "xml_reader.hpp"

namespace {
    extern "C" {
        static size_t
        curl_write_function(void *buffer,
                            size_t size,
                            size_t nmemb,
                            void *userp)
        {
            auto const data = std::string(static_cast<char*>(buffer), size*nmemb);
            auto const buf = static_cast<std::string*>(userp);
            buf->append(std::move(data));

            return size*nmemb;
        }

        static size_t
        curl_write_function_mis(void *buffer,
                                size_t size,
                                size_t nmemb,
                                void *userp)
        {
            auto ba = static_cast<GByteArray*>(userp);
            g_byte_array_append(ba, static_cast<guint8*>(buffer), size * nmemb);

            return size*nmemb;
        }

        static void
        mal_curl_lock_function(CURL* curl,
                               curl_lock_data data,
                               curl_lock_access access,
                               void* userp)
        {
            auto functor_pair_p = static_cast<MAL::MAL::pair_lock_functor_t*>(userp);
            functor_pair_p->first(curl, data, access);
        }

        static void
        mal_curl_unlock_function(CURL* curl,
                                 curl_lock_data data,
                                 void* userp)
        {
            auto functor_pair_p = static_cast<MAL::MAL::pair_lock_functor_t*>(userp);
            functor_pair_p->second(curl, data);
        }

        static int
        mal_curl_progress_function(void *clientp,
                                   double,// dltotal,
                                   double dlnow,
                                   double,// upload total
                                   double)// upload now
        {
            auto progress_cb = static_cast<MAL::MAL::DownloadProgressCb_t*>(clientp);
            (*progress_cb)(static_cast<int_fast64_t>(dlnow));

            return 0;
        }
    }

    static void
    print_curl_error(CURLcode code,
                     const std::unique_ptr<char[]>& curl_ebuffer)
    {
        std::cerr << "Error: " << curl_easy_strerror(code);
        if (curl_ebuffer)
            std::cerr << ". " << curl_ebuffer.get();
        std::cerr << std::endl;
    }

    static void
    print_curl_share_error(CURLSHcode code)
    {
        std::cerr << "Error: " << curl_share_strerror(code) << std::endl;
    }

    static void
    curl_setup_httpauth(std::unique_ptr<CURL, MAL::CURLEasyDeleter>& curl,
                        std::unique_ptr<MAL::UserInfo>& user_info)
    {
        CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        if (G_UNLIKELY(code != CURLE_OK)) {
            print_curl_error(code, nullptr);
        }
        code = curl_easy_setopt(curl.get(), CURLOPT_USERNAME, user_info->get_username().get());
        if (G_UNLIKELY(code != CURLE_OK)) {
            print_curl_error(code, nullptr);
        }
        code = curl_easy_setopt(curl.get(), CURLOPT_PASSWORD, user_info->get_password().get());
        if (G_UNLIKELY(code != CURLE_OK)) {
            print_curl_error(code, nullptr);
        }
    }

    static void
    curl_setup_progress(std::unique_ptr<CURL, MAL::CURLEasyDeleter>& curl,
                        MAL::MAL::DownloadProgressCb_t& bound_cb)
    {
        CURLcode code = curl_easy_setopt(curl.get(),
                                         CURLOPT_PROGRESSFUNCTION,
                                         &mal_curl_progress_function);
        if (code != CURLE_OK) {
            print_curl_error(code, nullptr);
        }

        code = curl_easy_setopt(curl.get(), CURLOPT_NOPROGRESS, 0);
        if (code != CURLE_OK) {
            print_curl_error(code, nullptr);
        }

        code = curl_easy_setopt(curl.get(), CURLOPT_PROGRESSDATA, &bound_cb);
        if (code != CURLE_OK) {
            print_curl_error(code, nullptr);
        }
    }

    static void
    curl_setup_post(std::unique_ptr<CURL, MAL::CURLEasyDeleter>& curl,
                    const std::string& fields)
    {
        CURLcode code = curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        if (code != CURLE_OK) {
            print_curl_error(code, nullptr);
        }

        code = curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(fields.size()));
        if (code != CURLE_OK) {
            print_curl_error(code, nullptr);
        }

        code = curl_easy_setopt(curl.get(), CURLOPT_COPYPOSTFIELDS, fields.c_str());
        if (code != CURLE_OK) {
            print_curl_error(code, nullptr);
        }
    }

    static CURLcode
    curl_do_html_login(std::unique_ptr<CURL, MAL::CURLEasyDeleter>& curl,
                       const std::string& username,
                       const std::string& password)
    {
        std::string fields = "username=";
        fields += username;
        fields += "&password=";
        fields += password;
        fields += "&cookie=1";
        curl_setup_post(curl, fields);
        curl_easy_setopt(curl.get(), CURLOPT_URL, "http://myanimelist.net/login.php");
        return curl_easy_perform(curl.get());
    }
}
    
namespace MAL {
    MAL::MAL(std::unique_ptr<UserInfo>&& info) :
        user_info(std::move(info)),
        text_util(std::make_shared<TextUtility>()),
        serializer(text_util),
        manga_serializer(text_util),
        curl_ebuffer(std::make_unique<char[]>(CURL_ERROR_SIZE)),
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

        deserialize_from_disk_async();
    }

    MAL::~MAL()
    {
        serialize_to_disk_async();
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

    void MAL::setup_curl_easy_mis(CURL* easy, const std::string& url, GByteArray *ba)
    {
        CURLcode code;
        code = curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, &curl_write_function_mis);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_WRITEDATA, static_cast<void*>(ba));
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_ERRORBUFFER, curl_ebuffer.get());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_FAILONERROR, 1);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_NOSIGNAL, 1);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_URL, url.c_str());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_SHARE, curl_share.get());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_FOLLOWLOCATION, 1);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        std::stringstream user_agent;
//        auto const cvid = curl_version_info(CURLVERSION_NOW);
        user_agent << "mal-gtk/0.1.0 (linux)" << " "
                   << "libcurl" << "/7.32.0"; /* Have to lie. */
//                   << cvid->version;
//        std::cerr << "Setting user agent to " << user_agent.str() << std::endl;
        code = curl_easy_setopt(easy, CURLOPT_USERAGENT, user_agent.str().c_str());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
    }

    void MAL::setup_curl_easy(CURL* easy, const std::string& url, std::string* buffer) {
        CURLcode code;
        code = curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, &curl_write_function);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_WRITEDATA, buffer);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_ERRORBUFFER, curl_ebuffer.get());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_FAILONERROR, 1);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_NOSIGNAL, 1);
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_URL, url.c_str());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        code = curl_easy_setopt(easy, CURLOPT_SHARE, curl_share.get());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
        std::stringstream user_agent;
//        auto const cvid = curl_version_info(CURLVERSION_NOW);
        user_agent << "mal-gtk/0.1.0 (linux)" << " "
                   << "libcurl" << "/7.32.0"; /* Have to lie. */
//                   << cvid->version;
//        std::cerr << "Setting user agent to " << user_agent.str() << std::endl;
        code = curl_easy_setopt(easy, CURLOPT_USERAGENT, user_agent.str().c_str());
        if (code != CURLE_OK) {
            print_curl_error(code, curl_ebuffer);
        }
    }

    void MAL::get_anime_list_async(DownloadProgressCb_t progress_cb,
                                   OperationCompleteCb_t complete_cb)
    {
        active.send( [=] { this->get_anime_list_sync(progress_cb, complete_cb); } );
    }

    void MAL::get_anime_list_sync(DownloadProgressCb_t progress_cb,
                                  OperationCompleteCb_t complete_cb)
    {
        const std::string url = LIST_BASE_URL + user_info->get_username().get() + "&status=all&type=anime";
        auto buf = get_sync(url, progress_cb);
        if (buf) {
            text_util->parse_html_entities(*buf);
            auto anime_list = serializer.deserialize(*buf);
        
            {
                std::lock_guard<std::mutex> lock(m_anime_list_mutex);
                std::for_each(anime_list.begin(), anime_list.end(),
                              [this](const std::shared_ptr<Anime>& anime) {
                                  auto iter = m_anime_list.find(anime);
                                  if (iter != m_anime_list.end()) {
                                      (**iter).update_from_list(anime);
                                  } else {
                                      m_anime_list.insert(anime);
                                  }
                              });
            }

            if (complete_cb)
                cb_dispatcher.send(std::bind(complete_cb, true));

            signal_mal_info("Refreshed anime list from myanimelist.net");
        } else {
            if (complete_cb)
                cb_dispatcher.send(std::bind(complete_cb, false));

            signal_mal_info("Refresh of anime list failed");
        }
    }

    void MAL::get_manga_list_async()
    {
        active.send( [this](){ this->get_manga_list_sync(); } );
    }

    void MAL::get_manga_list_sync() {
        const std::string url = LIST_BASE_URL + user_info->get_username().get() + "&status=all&type=manga";
        auto buf = get_sync(url);
        if (buf) {
            text_util->parse_html_entities(*buf);
            auto manga_list = manga_serializer.deserialize(*buf);

            {
                std::lock_guard<std::mutex> lock(m_manga_list_mutex);
                std::for_each(manga_list.begin(), manga_list.end(),
                              [this](const std::shared_ptr<Manga>& manga) {
                                  auto iter = m_manga_list.find(manga);
                                  if (iter != m_manga_list.end()) {
                                      (**iter).update_from_list(manga);
                                  } else {
                                      m_manga_list.insert(manga);
                                  }
                              });
            }

            signal_manga_added();
            signal_mal_info("Refreshed manga list from myanimelist.net");
        }
    }

    void MAL::get_anime_details_async(const std::shared_ptr<const Anime>& anime)
    {
        active.send( [this, anime](){ this->get_anime_details_sync(anime); } );
    }

    void MAL::get_manga_details_async(const std::shared_ptr<const Manga>& manga)
    {
        active.send( [this, manga](){ this->get_manga_details_sync(manga); } );
    }

    void MAL::get_manga_details_sync(const std::shared_ptr<const Manga>& manga)
    {
        const std::string url = MANGA_DETAILS_BASE_URL + std::to_string(manga->id);
        auto buf = get_sync(url);
        std::shared_ptr<Manga> details = nullptr;
        if (buf) {
            details = manga_serializer.deserialize_details(*buf);
        }

        if (details) {
            std::lock_guard<std::mutex> lock(m_manga_list_mutex);
            auto const id = manga->series_itemdb_id;
            auto iter = std::find_if(m_manga_list.begin(), m_manga_list.end(), [id](const std::shared_ptr<Manga>& a) {
                    return a->series_itemdb_id == id;
                });
            if (iter != m_manga_list.end()) {
                (**iter).update_from_details(details);
            }

            signal_manga_detailed();
            signal_mal_info(std::string("Downloaded extended details for ") + manga->series_title);
        } else {
            signal_mal_error("Error while parsing manga details");
        }
    }

    std::unique_ptr<std::string> MAL::get_sync(const std::string& url,
                                               DownloadProgressCb_t progress_cb)
    {
        if (!user_info->get_username()) {
            signal_mal_error("No username provided");
            return nullptr;
        }

        std::unique_ptr<CURL, CURLEasyDeleter> curl { curl_easy_init() };
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        setup_curl_easy(curl.get(), url, buf.get());
        curl_setup_httpauth(curl, user_info);
        DownloadProgressCb_t bound_cb = [this, &progress_cb] (int_fast64_t progress) {
            cb_dispatcher.send( std::bind(progress_cb, progress) );
        };

        if (progress_cb) {
            curl_setup_progress(curl, bound_cb);
        }

        CURLcode code = curl_easy_perform(curl.get());
        if (code != CURLE_OK) {
            signal_mal_error(std::string("Error communicating with myanimelist.net: ") + curl_ebuffer.get());
            return nullptr;
        }

        if (buf->find("Error: You must first login to see this page.") == std::string::npos) {
            return buf;
        } else {
            code = curl_do_html_login(curl, user_info->get_username().get(), user_info->get_password().get());
            if (code != CURLE_OK) {
                signal_mal_error(std::string("Couldn't perform myanimelist.net php login: ") + curl_ebuffer.get() );
                return nullptr;
            } else {
                buf->clear();

                code = curl_easy_setopt(curl.get(), CURLOPT_HTTPGET, 1);
                if (code != CURLE_OK) {
                    print_curl_error(code, curl_ebuffer);
                }
                code = curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
                if (code != CURLE_OK) {
                    print_curl_error(code, curl_ebuffer);
                }

                if (progress_cb) {
                    curl_setup_progress(curl, bound_cb);
                }
                
                code = curl_easy_perform(curl.get());
                if (code != CURLE_OK) {
                    signal_mal_error(std::string("Error communicating with myanimelist.net: ") + curl_ebuffer.get());
                    return nullptr;
                } else {
                    return buf;
                }
            }
        }
    }

    void MAL::get_anime_details_sync(const std::shared_ptr<const Anime>& anime)
    {
        const std::string url = DETAILS_BASE_URL + std::to_string(anime->series_itemdb_id);
        auto buf = get_sync(url);
        std::shared_ptr<Anime> details = nullptr;
        if (buf) {
            details = serializer.deserialize_details(*buf);
        }

        if (details) {
            std::lock_guard<std::mutex> lock(m_anime_list_mutex);
            auto const id = anime->series_itemdb_id;
            auto iter = std::find_if(m_anime_list.begin(), m_anime_list.end(), [id](const std::shared_ptr<Anime>& a) {
                    return a->series_itemdb_id == id;
                });
            if (iter != m_anime_list.end()) {
                (**iter).update_from_details(details);
            }

            signal_anime_detailed();
            signal_mal_info(std::string("Downloaded extended details for ") + anime->series_title);
        } else {
            signal_mal_error("Error while parsing anime details");
        }
    }

    void MAL::get_image_async(const MALItem& item,
                              const std::function<void(const Glib::RefPtr<Gio::MemoryInputStream>&)>& cb)
    {
        active.send([this, item, cb] {
                auto img = get_image_sync(item);
                cb_dispatcher.send(std::bind(cb, img));
            });
    }

    Glib::RefPtr<Gio::MemoryInputStream> MAL::get_image_sync(const MALItem& item) {
        auto iter = image_cache.find(item.image_url);
        if (iter == std::end(image_cache)) {
            std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
            GByteArray *ba = g_byte_array_new();
            setup_curl_easy_mis(curl.get(), item.image_url, ba);
            CURLcode code = curl_easy_perform(curl.get());
            
            if (code != CURLE_OK) {
                print_curl_error(code, curl_ebuffer);
                signal_mal_info("Unable to fetch image for " + item.series_title + ": " + curl_ebuffer.get());
                g_byte_array_free(ba, TRUE);
                return Glib::RefPtr<Gio::MemoryInputStream>();
            } else {
                char *url;
                curl_easy_getinfo(curl.get(), CURLINFO_EFFECTIVE_URL, &url);
                if (strcmp(url, "http://myanimelist.net/404.php") == 0) {
                    signal_mal_info("Unable to fetch image for " + item.series_title + ": 404");
                    g_byte_array_free(ba, TRUE);
                    return Glib::RefPtr<Gio::MemoryInputStream>();
                }

                GBytes *gbytes = g_byte_array_free_to_bytes(ba);
                auto inserted = image_cache.emplace(item.image_url, Glib::wrap(gbytes));
                auto mis = Gio::MemoryInputStream::create();
                g_memory_input_stream_add_bytes(mis->gobj(), inserted.first->second->gobj());
                return mis;
            }
        } else {
            auto mis = Gio::MemoryInputStream::create();
            g_memory_input_stream_add_bytes(mis->gobj(), iter->second->gobj());
            return mis;
        }
    }

    void MAL::search_anime_async(const std::string& terms) {
        active.send( [this, terms](){ this->search_anime_sync(terms); } );
    }

    void MAL::search_anime_sync(const std::string& terms) {
        std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        std::unique_ptr<char, CURLEscapeDeleter> terms_escaped {curl_easy_escape(curl.get(), terms.c_str(), terms.size())};
        const std::string url = SEARCH_BASE_URL + terms_escaped.get();

        setup_curl_easy(curl.get(), url, buf.get());
        curl_setup_httpauth(curl, user_info);

        CURLcode code = curl_easy_perform(curl.get());
        if (code != CURLE_OK) {
            long res = 0;
            code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
            if (code != CURLE_OK)
                print_curl_error(code, curl_ebuffer);
            else if (res == 401) {
                signal_run_password_dialog();
            } else {
                signal_mal_error(std::string("Error searching myanimelist.net: ") + curl_ebuffer.get());
            }
            return;
        }

        text_util->parse_html_entities(*buf);
        if (buf->size() > 0) {
            auto search_results = serializer.deserialize(*buf);
            std::lock_guard<std::mutex> lock(m_anime_search_results_mutex);
            m_anime_search_results.clear();
            m_anime_search_results.insert(search_results.cbegin(), search_results.cend());
        } else {
            signal_mal_error("myanimelist.net returned zero responses for search terms '" + terms + "'");
        }

        signal_anime_search_completed();
    }

    void MAL::search_manga_async(const std::string& terms) {
        active.send( [this, terms](){ this->search_manga_sync(terms); } );
    }

    void MAL::search_manga_sync(const std::string& terms) {
        std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        std::unique_ptr<char, CURLEscapeDeleter> terms_escaped {curl_easy_escape(curl.get(), terms.c_str(), terms.size())};
        const std::string url = MANGA_SEARCH_BASE_URL + terms_escaped.get();

        setup_curl_easy(curl.get(), url, buf.get());
        curl_setup_httpauth(curl, user_info);

        CURLcode code = curl_easy_perform(curl.get());
        if (code != CURLE_OK) {
            long res = 0;
            code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
            if (code != CURLE_OK)
                print_curl_error(code, curl_ebuffer);
            else if (res == 401) {
                signal_run_password_dialog();
            } else {
                signal_mal_error(std::string("Error searching myanimelist.net: ") + curl_ebuffer.get());
            }
            return;
        }

        text_util->parse_html_entities(*buf);
        if (buf->size() > 0) {
            auto search_results = manga_serializer.deserialize(*buf);
            std::lock_guard<std::mutex> lock(m_manga_search_results_mutex);
            m_manga_search_results.clear();
            m_manga_search_results.insert(search_results.cbegin(), search_results.cend());
        } else {
            signal_mal_error("myanimelist.net returned zero responses for search terms '" + terms + "'");
        }

        signal_manga_search_completed();
    }

    void MAL::update_anime_async(const std::shared_ptr<Anime>& anime) {
        active.send( [=] { update_anime_sync(anime); } );
    }
    
    bool MAL::update_anime_sync(const std::shared_ptr<Anime>& anime) {
        const std::string url = UPDATED_BASE_URL + std::to_string(anime->series_itemdb_id) + ".xml";
        std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        setup_curl_easy(curl.get(), url, buf.get());
        auto xml = serializer.serialize(*anime);
//        std::cerr << "Sending: " << xml << std::endl;
        xml.insert(0, "data=");
        curl_setup_post(curl, xml);
        curl_setup_httpauth(curl, user_info);

        CURLcode code = curl_easy_perform(curl.get());
        if (code != CURLE_OK) {
            long res = 0;
            code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
            if (code != CURLE_OK)
                print_curl_error(code, curl_ebuffer);
            else if (res == 401) {
                signal_run_password_dialog();
            } else {
                signal_mal_error(anime->series_title + " not updated due to myanimelist.net error: " + curl_ebuffer.get());
            }
            return false;
        }

        if (buf->compare("Updated") == 0) {
            std::lock_guard<std::mutex> lock(m_anime_list_mutex);
            auto iter = std::find_if(m_anime_list.begin(), m_anime_list.end(), [&anime](const std::shared_ptr<Anime>& a) {
                    return a->series_itemdb_id == anime->series_itemdb_id;
                });
            if (iter != m_anime_list.end()) {
                anime->last_updated = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                m_anime_list.erase(iter);
                m_anime_list.insert(anime);
                signal_mal_info(anime->series_title + " successfully updated");
            } else {
                signal_mal_error(anime->series_title + " updated, but is not in our local list. Programmer error!");
            }
            return true;
        } else {
            std::cerr << "Error: Couldn't update: " << *buf << std::endl;
            signal_mal_error(anime->series_title + " not updated due to myanimelist.net error.");
            return false;
        }
    }

    void MAL::update_manga_async(const std::shared_ptr<Manga>& manga) {
        active.send( [this, manga](){ this->update_manga_sync(manga); } );
    }

    bool MAL::update_manga_sync(const std::shared_ptr<Manga>& manga) {
        const std::string url = MANGA_UPDATED_BASE_URL + std::to_string(manga->series_itemdb_id) + ".xml";
        std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        setup_curl_easy(curl.get(), url, buf.get());
        auto xml = manga_serializer.serialize(*manga);
        xml.insert(0, "data=");
        curl_setup_post(curl, xml);
        curl_setup_httpauth(curl, user_info);

        CURLcode code = curl_easy_perform(curl.get());
        if (code != CURLE_OK) {
            long res = 0;
            code = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
            if (code != CURLE_OK)
                print_curl_error(code, curl_ebuffer);
            else if (res == 401) {
                signal_run_password_dialog();
            } else {
                signal_mal_error(manga->series_title + " not updated due to myanimelist.net error: " + curl_ebuffer.get());
            }
            return false;
        }

        if (buf->compare("Updated") == 0) {
            std::lock_guard<std::mutex> lock(m_manga_list_mutex);
            auto iter = std::find_if(m_manga_list.begin(), m_manga_list.end(), [&manga](const std::shared_ptr<Manga>& m) {
                    return m->series_itemdb_id == manga->series_itemdb_id;
                });
            if (iter != m_manga_list.end()) {
                m_manga_list.erase(iter);
                m_manga_list.insert(manga);
                signal_mal_info(manga->series_title + " successfully updated");
            }
            return true;
        } else {
            signal_mal_error(manga->series_title + " not updated due to myanimelist.net error: " + *buf);
            return false;
        }
    }

    void
    MAL::add_anime_async(const Anime& anime,
                         OperationCompleteCb_t complete_cb)
    {
        active.send( [=] { this->add_anime_sync(anime, complete_cb); } );
    }

    bool
    MAL::add_anime_sync(const Anime& anime,
                        OperationCompleteCb_t complete_cb)
    {
        const std::string url = ADD_BASE_URL + std::to_string(anime.series_itemdb_id) + ".xml";
        std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        setup_curl_easy(curl.get(), url, buf.get());
        auto xml = serializer.serialize(anime);
//        std::cerr << "Adding anime " << anime.series_title << " with status = " << to_string(anime.status) << std::endl;
//        std::cerr << "The xml we are sending is: " << xml << std::endl;
        xml.insert(0, "data=");
        curl_setup_post(curl, xml);
        curl_setup_httpauth(curl, user_info);
        CURLcode code = curl_easy_perform(curl.get());
        if (code != CURLE_OK) {
            signal_mal_error(anime.series_title + " not added due to myanimelist.net error: " + curl_ebuffer.get());
            long res = 0;
            curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &res);
            if (res == 401) {
                signal_run_password_dialog();
            }
            return false;
        }

        if (code == CURLE_OK) {
            {
                auto anime_p = std::static_pointer_cast<Anime>(anime.clone());
                std::lock_guard<std::mutex> lock(m_anime_list_mutex);
                auto iter = m_anime_list.find(anime_p);
                if (iter != m_anime_list.end()) {
                    (**iter).update_from_list(anime_p);
                } else {
                    m_anime_list.insert(anime_p);
                }
            }

            if (complete_cb)
                cb_dispatcher.send(std::bind(complete_cb, true));

            signal_anime_added();
            signal_mal_info(anime.series_title + " successfully added");
            return true;
        } else {
            if (complete_cb)
                cb_dispatcher.send(std::bind(complete_cb, false));

            signal_mal_error(anime.series_title + " could not be added due to myanimelist.net error: " + *buf);
            return false;
        }
    }

    void MAL::add_manga_async(const Manga& manga) {
        active.send( [this, manga](){ this->add_manga_sync(manga); } );
    }

    bool MAL::add_manga_sync(const Manga& manga) {
        const std::string url = MANGA_ADD_BASE_URL + std::to_string(manga.series_itemdb_id) + ".xml";
        std::unique_ptr<CURL, CURLEasyDeleter> curl {curl_easy_init()};
        std::unique_ptr<std::string> buf = std::make_unique<std::string>();
        setup_curl_easy(curl.get(), url, buf.get());
        auto xml = manga_serializer.serialize(manga);
        xml.insert(0, "data=");
        curl_setup_post(curl, xml);
        curl_setup_httpauth(curl, user_info);

        CURLcode code = curl_easy_perform(curl.get());
        long html_code = 0;
        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &html_code);

        if (code != CURLE_OK) {
            signal_mal_error(manga.series_title + " not added due to myanimelist.net error: " + curl_ebuffer.get());
            if (html_code == 401) {
                signal_run_password_dialog();
            }
            return false;
        }

        if (code == CURLE_OK) {
            signal_manga_added();
            signal_mal_info(manga.series_title + " successfully added");
            return true;
        } else {
            signal_mal_error(manga.series_title + " could not be added due to myanimelist.net error: " + *buf);
            return false;
        }
    }

    void MAL::involke_lock_function(CURL*, curl_lock_data data, curl_lock_access) {
        auto iter = map_mutex.find(data);
        if (iter == map_mutex.end()) {
            auto res = map_mutex.emplace(std::piecewise_construct, std::forward_as_tuple(data), std::tuple<>());
            if (!res.second) {
                std::cerr << "Error: Trying to lock curl data but we can't create the mutex" << std::endl;
                return;
            }
            iter = res.first;
        }

        iter->second.lock();
    }

    void MAL::involke_unlock_function(CURL*, curl_lock_data data) {
        auto iter = map_mutex.find(data);
        if (iter == map_mutex.end()) {
            std::cerr << "Error: Trying to unlock curl data but we don't have the mutex." << std::endl;
            return;
        }

        iter->second.unlock();
    }

    void MAL::deserialize_from_disk_async()
    {
        active.send( [this](){ deserialize_from_disk_sync(); } );
    }

    void MAL::deserialize_from_disk_sync()
    {
        auto filename = Glib::build_filename(Glib::get_user_data_dir(), "mal-gtk", "AnimeMangaList.xml");
        try {
            XmlReader reader(Glib::file_get_contents(filename));
            std::unique_lock<std::mutex> anime_lock(m_anime_list_mutex, std::defer_lock);
            std::unique_lock<std::mutex> manga_lock(m_manga_list_mutex, std::defer_lock);
            std::lock(anime_lock, manga_lock);
            reader.read();
            try {
                while(!(reader.get_name() == "mal-gtk" && reader.get_type() == XML_READER_TYPE_END_ELEMENT)) {
                    if (reader.get_name() == "anime" && reader.get_type() == XML_READER_TYPE_ELEMENT) {
                        m_anime_list.insert(m_anime_list.end(), std::make_shared<Anime>(reader));
                    } else if (reader.get_name() == "manga" && reader.get_type() == XML_READER_TYPE_ELEMENT) {
                        m_manga_list.insert(m_manga_list.end(), std::make_shared<Manga>(reader));
                    } else {
                        if (reader.read() < 0)
                            break;
                    }
                }

                signal_anime_added();
                signal_manga_added();
                signal_mal_info("Loaded anime and manga list from local storage.");
            } catch (std::exception e) {
                std::cerr << "Caught exception " << e.what() << " on node " << reader.get_name() << " value '" << reader.get_value() << "'" << std::endl;
                reader.read();
                std::cerr << "Next node was " << reader.get_name() << std::endl;
            }

        } catch (Glib::FileError e) {
            if (e.code() != Glib::FileError::NO_SUCH_ENTITY)
                signal_mal_error("Error reading anime list from disk: " + e.what());
            get_anime_list_async();
            get_manga_list_async();
        }
    }

    void MAL::serialize_to_disk_async() {
        active.send( [this](){ serialize_to_disk_sync(); } );
    }

    void MAL::serialize_to_disk_sync()
    {
        XmlWriter writer;
        writer.startDoc();
        writer.startElement("mal-gtk");
        writer.startElement("anime_list");
        for_each_anime(std::bind(&Anime::serialize, std::placeholders::_1, std::ref(writer)));
        writer.endElement();
        writer.startElement("manga_list");
        for_each_manga(std::bind(&Manga::serialize, std::placeholders::_1, std::ref(writer)));
        writer.endElement();
        writer.endDoc();

        auto datadir = Glib::get_user_data_dir();
        auto dir = Glib::build_filename(datadir, "mal-gtk");
        auto dirfile = Gio::File::create_for_path(dir);
        if (!dirfile->query_exists()) {
            try {
                if (!dirfile->make_directory_with_parents()) {
                    signal_mal_error("Unable to create directory " + dirfile->get_parse_name() + ". Unable to save to disk!");
                    return;
                }
            } catch (Glib::Error e) {
                signal_mal_error("Unable to create directory " + dirfile->get_parse_name() + ". Unable to save to disk: " + e.what());
                return;
            }
        } else {
            auto info = dirfile->query_info(G_FILE_ATTRIBUTE_STANDARD_TYPE);
            if (info->get_file_type() != Gio::FILE_TYPE_DIRECTORY) {
                signal_mal_error(dirfile->get_parse_name() + " is not a directory. Unable to save to disk!");
                return;
            }
        }
        auto filename = Glib::build_filename(dir, "AnimeMangaList.xml");
        try {
            Glib::file_set_contents(filename, writer.getString());
        } catch (Glib::FileError e) {
            signal_mal_error("Unable to save to disk: " + e.what());
        }
    }
}
