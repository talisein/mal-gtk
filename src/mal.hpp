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

#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <algorithm>
#include <functional>
#include <mutex>
#include <curl/curl.h>
#include <glibmm/dispatcher.h>
#include "anime.hpp"
#include "manga.hpp"
#include "anime_serializer.hpp"
#include "manga_serializer.hpp"
#include "user_info.hpp"
#include "text_util.hpp"
#include "active.hpp"
#include "message_dispatcher.hpp"
#include "callback_dispatcher.hpp"

namespace MAL {

    /* Deleters for libcurl */
    struct CURLEscapeDeleter {
        void operator()(char* str) const
            {
                curl_free(str);
            }
    };

    struct CURLEasyDeleter {
        void operator()(CURL* curl) const {
            curl_easy_cleanup(curl);
        }
    };
    
    struct CURLShareDeleter {
        void operator()(CURLSH* share) const {
            CURLSHcode code = curl_share_cleanup(share);
            if (code != CURLSHE_OK) {
                std::cerr << "Error: Curl share cleanup: "
                          << curl_share_strerror(code) << std::endl;
            }
        }
    };

    /** Interface to myanimelist.net.
     *
     * Includes a local cache of lists downloaded. Network operations
     * performed in worker thread using libcurl.
     */
    class MAL {
        typedef std::function<void (CURL*, curl_lock_data, curl_lock_access)> lock_functor_t;
        typedef std::function<void (CURL*, curl_lock_data)> unlock_functor_t;

    public:
        MAL(std::unique_ptr<UserInfo>&& info);
        ~MAL();

        /** Applies the given functor object f to all anime.
         *
         * Used because the anime list is behind a mutex.
         */
        void for_each_anime(const std::function<void (const std::shared_ptr<Anime>& item)>& f) {
            std::lock_guard<std::mutex> lock(m_anime_list_mutex);
            std::for_each(m_anime_list.cbegin(), m_anime_list.cend(), f);
        }

        /** Applies the given functor object f to all manga.
         *
         * Used because the manga list is behind a mutex.
         */
        void for_each_manga(const std::function<void (const std::shared_ptr<Manga>& item)>& f) {
            std::lock_guard<std::mutex> lock(m_manga_list_mutex);
            std::for_each(m_manga_list.cbegin(), m_manga_list.cend(), f);
        }

        /** Applies the given functor object f to all anime search results.
         *
         * TODO: Migrate to using CallbackDispatcher and remove.
         */
        void for_each_anime_search_result(const std::function<void (const std::shared_ptr<Anime>& item)>& f) {
            std::lock_guard<std::mutex> lock(m_anime_search_results_mutex);
            std::for_each(m_anime_search_results.cbegin(), m_anime_search_results.cend(), f);
        }

        /** Applies the given functor object f to all manga search results.
         *
         * TODO: Migrate to using CallbackDispatcher and remove.
         */
        void for_each_manga_search_result(const std::function<void (const std::shared_ptr<Manga>& item)>& f) {
            std::lock_guard<std::mutex> lock(m_manga_search_results_mutex);
            std::for_each(m_manga_search_results.cbegin(), m_manga_search_results.cend(), f);
        }

        std::shared_ptr<Anime>
        find_anime(const std::shared_ptr<Anime>& anime)
            {
                std::lock_guard<std::mutex> lock(m_anime_list_mutex);
                auto iter = m_anime_list.find(anime);
                if (iter == m_anime_list.end())
                    return nullptr;
                else
                    return std::static_pointer_cast<Anime>((**iter).clone());
            }
        
        /** Provides a callback on GTK+ Main Thread when an error occurs.
         */
        MessageDispatcher<Glib::ustring> signal_mal_error;

        /** Provides a callback on GTK+ Main Thread with info a user
         * may like.
         *
         * Messages are like "Anime 'Kill la Kill' added" or "Manga
         * list fetched"
         */
        MessageDispatcher<Glib::ustring> signal_mal_info;

        typedef std::function<void (int_fast64_t bytes)> DownloadProgressCb_t;
        typedef std::function<void (bool success)> OperationCompleteCb_t;

        /** Fetches anime list from MAL.net.
         *
         * Callbacks are delivered on the GTK+ main thread.
         *
         * @progress_cb: Called multiple times with download progress.
         * @complete_cb: Called once when the operation is finished.
         */
        void get_anime_list_async(DownloadProgressCb_t progress_cb = nullptr,
                                  OperationCompleteCb_t complete_cb = nullptr);
        void get_manga_list_async();
        void get_anime_details_async(const std::shared_ptr<const Anime>& anime);
        void get_manga_details_async(const std::shared_ptr<const Manga>& manga);
        void search_anime_async(const std::string&);
        void search_manga_async(const std::string&);
        void update_anime_async(const std::shared_ptr<Anime>&);
        void update_manga_async(const std::shared_ptr<Manga>&);

        void add_anime_async(const Anime&,
                             OperationCompleteCb_t = nullptr);
        void add_manga_async(const Manga&);

        Glib::Dispatcher signal_anime_added;
        Glib::Dispatcher signal_manga_added;
        Glib::Dispatcher signal_anime_search_completed;
        Glib::Dispatcher signal_manga_search_completed;
        Glib::Dispatcher signal_anime_detailed;
        Glib::Dispatcher signal_manga_detailed;

        void get_image_async(const MALItem& item, const std::function<void (const std::string&)>& cb);
        std::string get_image_sync(const MALItem& item);
        std::string get_manga_image_sync(const Manga& manga);

        typedef std::pair<lock_functor_t, unlock_functor_t> pair_lock_functor_t;
        void serialize_to_disk_async();

    private:
        const std::string LIST_BASE_URL          = "http://myanimelist.net/malappinfo.php?u=";
        const std::string DETAILS_BASE_URL       = "http://myanimelist.net/editlist.php?type=anime&id=";
        const std::string SEARCH_BASE_URL        = "http://myanimelist.net/api/anime/search.xml?q=";
        const std::string UPDATED_BASE_URL       = "http://myanimelist.net/api/animelist/update/";
        const std::string ADD_BASE_URL           = "http://myanimelist.net/api/animelist/add/";
        const std::string MANGA_DETAILS_BASE_URL = "http://myanimelist.net/panel.php?go=editmanga&id=";
        const std::string MANGA_SEARCH_BASE_URL  = "http://myanimelist.net/api/manga/search.xml?q=";
        const std::string MANGA_UPDATED_BASE_URL = "http://myanimelist.net/api/mangalist/update/";
        const std::string MANGA_ADD_BASE_URL     = "http://myanimelist.net/api/mangalist/add/";

        CallbackDispatcher cb_dispatcher;

        /** Returns the anime list for username. As slow as the
         * internet.
         * Safe to call from multiple threads.
         */

        void get_anime_list_sync(DownloadProgressCb_t progress_cb = nullptr,
                                 OperationCompleteCb_t complete_cb = nullptr);

        /** Returns the manga list for username. As slow as the
         * internet.
         * Safe to call from multiple threads.
         */
        void get_manga_list_sync();

        std::unique_ptr<std::string> get_sync(const std::string& url, DownloadProgressCb_t progress_cb = nullptr);
        void get_anime_details_sync(const std::shared_ptr<const Anime>& anime);
        void get_manga_details_sync(const std::shared_ptr<const Manga>& manga);

        /** Searches MAL.net. Slow as the Internet.
         * Safe to call from multiple threads.
         */
        void search_anime_sync(const std::string& terms);

        /** Searches MAL.net. Slow as the Internet.
         * Safe to call from multiple threads.
         */
        void search_manga_sync(const std::string& terms);

        /** Updates MAL.net with the new anime details. As slow as the
         * Internet.
         * Safe to call from multiple threads.
         */
        bool update_anime_sync(const std::shared_ptr<Anime>& anime);

        /** Updates MAL.net with the new manga details. As slow as the
         * Internet.
         * Safe to call from multiple threads.
         */
        bool update_manga_sync(const std::shared_ptr<Manga>& manga);

        /** Adds an anime to the MAL.net anime list. As slow as the
         * Internet.
         * Safe to call from multiple threads.
         */
        bool add_anime_sync(const Anime& anime,
                            OperationCompleteCb_t = nullptr);

        /** Adds an anime to the MAL.net anime list. As slow as the
         * Internet.
         * Safe to call from multiple threads.
         */
        bool add_manga_sync(const Manga& manga);

        std::unique_ptr<UserInfo> user_info;
        Glib::Dispatcher signal_run_password_dialog;
        void run_password_dialog();

        void involke_lock_function(CURL*, curl_lock_data, curl_lock_access);
        void involke_unlock_function(CURL*, curl_lock_data);

        void setup_curl_easy(CURL* easy, const std::string& url, std::string*);

        void serialize_to_disk_sync();
        void deserialize_from_disk_async();
        void deserialize_from_disk_sync();

        template <typename T>
        class MALItemComparator {
        public:
            bool operator()(const std::shared_ptr<const T>& l,const std::shared_ptr<const T>& r)
                {
                    return l->series_itemdb_id < r->series_itemdb_id;
                    /*auto season = l->series_date_begin.substr(0,7).compare(r->series_date_begin.substr(0,7));
                      if (season == 0)
                      return l->series_itemdb_id < r->series_itemdb_id;
                      else
                      return season > 0;*/
                };
        };

        std::set<std::shared_ptr<Anime>, MALItemComparator<Anime> > m_anime_list;
        std::mutex                                                  m_anime_list_mutex;
        std::set<std::shared_ptr<Manga>, MALItemComparator<Manga> > m_manga_list;
        std::mutex                                                  m_manga_list_mutex;
        std::set<std::shared_ptr<Anime>, MALItemComparator<Anime> > m_anime_search_results;
        std::mutex                                                  m_anime_search_results_mutex;
        std::set<std::shared_ptr<Manga>, MALItemComparator<Manga> > m_manga_search_results;
        std::mutex                                                  m_manga_search_results_mutex;

        std::shared_ptr<TextUtility> text_util;
        AnimeSerializer serializer;
        MangaSerializer manga_serializer;
        std::unique_ptr<char[]> curl_ebuffer;
        std::unique_ptr<pair_lock_functor_t> share_lock_functors;
        std::map<curl_lock_data, std::unique_ptr<std::mutex>> map_mutex;
        
        std::map<std::string, std::unique_ptr<std::string> > image_cache;
        std::map<int_fast64_t, std::unique_ptr<std::string> > manga_image_cache;

        std::unique_ptr<CURLSH, CURLShareDeleter> curl_share;
        Active active; /* Must be destroyed before curl_share */
    };
}
