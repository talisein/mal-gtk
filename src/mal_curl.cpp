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

#include <glib.h>
#include <string.h>
#include "mal_curl.hpp"

namespace {
    extern "C" {
        static void
        mal_curl_lock_function(CURL* curl,
                               curl_lock_data data,
                               curl_lock_access access,
                               void* userp)
        {
            auto functor_pair_p = static_cast<MAL::curl_pool::pair_lock_functor_t*>(userp);
            functor_pair_p->first(curl, data, access);
        }

        static void
        mal_curl_unlock_function(CURL* curl,
                                 curl_lock_data data,
                                 void* userp)
        {
            auto functor_pair_p = static_cast<MAL::curl_pool::pair_lock_functor_t*>(userp);
            functor_pair_p->second(curl, data);
        }
    }
}

namespace MAL
{
    /* Deleters for libcurl */
    void CURLEscapeDeleter::operator()(char* str) const
    {
        curl_free(str);
    }

    void CURLEasyDeleter::operator()(CURL* curl) const
    {
        curl_easy_cleanup(curl);
    }

    void CURLShareDeleter::operator()(CURLSH* share) const
    {
        CURLSHcode code = curl_share_cleanup(share);
        if (G_UNLIKELY(code != CURLSHE_OK)) {
            g_critical("Failed to clean curl share: %s",
                       curl_share_strerror(code));
        }
    }

    void CURLSListDeleter::operator()(struct curl_slist* list) const
    {
        curl_slist_free_all(list);
    }

    std::string curl_escape(const curlp& curl, const std::string& str)
    {
        curlstr escaped_str {curl_easy_escape(curl.get(), str.c_str(),
                                              str.size())};
        if (G_LIKELY(escaped_str)) {
            return std::string(escaped_str.get());
        } else {
            g_critical("curl_easy_escape returned NULL for input of length %zu", str.size());
            return std::string();
        }
    }

    secret_string curl_escape(const curlp& curl, const secret_string& str)
    {
        curlstr s {curl_easy_escape(curl.get(), str.c_str(), str.size())};
        if (G_UNLIKELY(!s)) {
            g_critical("curl_easy_escape returned NULL for secret input of length %zu", str.size());
            return secret_string();
        }

        auto len = strlen(s.get());
        secret_string res {s.get(), len};
        memset(s.get(), 0, len);
        return res;
    }

    CURLcode curl_perform(curlp& curl)
    {
        CURLcode res = curl_easy_perform(curl.get());
        if (G_UNLIKELY(CURLE_OK != res)) {
            std::stringstream ss;
            const char *estr = curl_easy_strerror(res);
            ss << "Error performing http request ";

            long curl_errno {0};
            curl_getinfo(curl, CURLINFO_OS_ERRNO, &curl_errno);
            char *url {nullptr};
            curl_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            long response_code {0};
            curl_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            char *redirect_url {nullptr};
            curl_getinfo(curl, CURLINFO_REDIRECT_URL, &redirect_url);
            curl_off_t uploaded {0};
            curl_getinfo(curl, CURLINFO_SIZE_UPLOAD_T, &uploaded);
            curl_off_t downloaded {0};
            curl_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &downloaded);
            long ssl_verify_result {0};
            curl_getinfo(curl, CURLINFO_SSL_VERIFYRESULT, &ssl_verify_result);
            long num_connects {0};
            curl_getinfo(curl, CURLINFO_NUM_CONNECTS, &num_connects);
            char *ip {nullptr};
            curl_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
            long port {0};
            curl_getinfo(curl, CURLINFO_PRIMARY_PORT, &port);
            double total_time {0.0};
            curl_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);

            ss << "(" << response_code << "): " << estr;
            auto str = ss.str();
            GLogField fields[] = {
                { "CURL_EASY_ERRORCODE", &res, sizeof(CURLcode) },
                { "CURL_EASY_STRERROR", estr, -1 },
                { "CURL_INFO_EFFECTIVE_URL", url ? url : "", -1},
                { "CURL_INFO_REDIRECT_URL", redirect_url ? redirect_url : "", -1},
                { "CURL_INFO_RESPONSE_CODE", &response_code, sizeof(long) },
                { "CURL_INFO_NUM_CONNECTS", &num_connects, sizeof(long) },
                { "CURL_INFO_PRIMARY_IP", ip ? ip : "", -1},
                { "CURL_INFO_PRIMARY_PORT", &port, sizeof(long)},
                { "CURL_INFO_SIZE_UPLOAD", &uploaded, sizeof(curl_off_t) },
                { "CURL_INFO_SIZE_DOWNLOAD", &downloaded, sizeof(curl_off_t) },
                { "CURL_INFO_TOTAL_TIME_SECONDS", &total_time, sizeof(double) },
                { "CURL_INFO_SSL_VERIFYRESULT", &ssl_verify_result, sizeof(long) },
                { "MESSAGE_ID", "6ed68ebc18f84670a5730905eee8053d", -1 },
                { "MESSAGE", str.c_str(), -1 },
                { "ERRNO", &curl_errno, sizeof(long) },
                { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_MESSAGE), -1},
            };

            g_log_structured_array(G_LOG_LEVEL_MESSAGE, fields, G_N_ELEMENTS(fields));
        } else {
            char *url {nullptr};
            curl_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
            long response_code {0};
            curl_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            char *redirect_url {nullptr};
            curl_getinfo(curl, CURLINFO_REDIRECT_URL, &redirect_url);
            curl_off_t uploaded {0};
            curl_getinfo(curl, CURLINFO_SIZE_UPLOAD_T, &uploaded);
            curl_off_t downloaded {0};
            curl_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &downloaded);
            long ssl_verify_result {0};
            curl_getinfo(curl, CURLINFO_SSL_VERIFYRESULT, &ssl_verify_result);
            long num_connects {0};
            curl_getinfo(curl, CURLINFO_NUM_CONNECTS, &num_connects);
            char *ip {nullptr};
            curl_getinfo(curl, CURLINFO_PRIMARY_IP, &ip);
            long port {0};
            curl_getinfo(curl, CURLINFO_PRIMARY_PORT, &port);
            double total_time {0.0};
            curl_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
            std::stringstream ss;
            ss << "Curl success. " << response_code << " " << url << " "
               << downloaded << "B down " << uploaded << "B up in "
               << total_time << "s";
            auto msg = ss.str();
            GLogField fields[] = {
                { "CURL_INFO_EFFECTIVE_URL", url ? url : "", -1},
                { "CURL_INFO_REDIRECT_URL", redirect_url ? redirect_url : "", -1},
                { "CURL_INFO_RESPONSE_CODE", &response_code, sizeof(long) },
                { "CURL_INFO_NUM_CONNECTS", &num_connects, sizeof(long) },
                { "CURL_INFO_PRIMARY_IP", ip ? ip : "", -1},
                { "CURL_INFO_PRIMARY_PORT", &port, sizeof(long)},
                { "CURL_INFO_SIZE_UPLOAD", &uploaded, sizeof(curl_off_t) },
                { "CURL_INFO_SIZE_DOWNLOAD", &downloaded, sizeof(curl_off_t) },
                { "CURL_INFO_TOTAL_TIME_SECONDS", &total_time, sizeof(double) },
                { "CURL_INFO_SSL_VERIFYRESULT", &ssl_verify_result, sizeof(long) },
                { "MESSAGE_ID", "4cc7906ea5cf4809a1e6ad64b2a9561c", -1 },
                { "MESSAGE", msg.c_str(), -1 },
                { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_DEBUG), -1},
            };

            g_log_structured_array(G_LOG_LEVEL_DEBUG, fields, G_N_ELEMENTS(fields));
        }

        return res;
    }

    curl_pool::curl_pool() :
        share_lock_functors(new pair_lock_functor_t(std::bind(&curl_pool::involke_lock_function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                                                    std::bind(&curl_pool::involke_unlock_function, this, std::placeholders::_1, std::placeholders::_2))),
        curl_share(curl_share_init())
    {
        for (curl_lock_data d : { CURL_LOCK_DATA_SHARE,
                                  CURL_LOCK_DATA_COOKIE,
                                  CURL_LOCK_DATA_DNS,
                                  CURL_LOCK_DATA_SSL_SESSION })

        {
            auto iter = map_mutex.emplace(std::piecewise_construct,
                                          std::forward_as_tuple(d),
                                          std::tuple<>());
            if (G_LIKELY(map_mutex.end() != iter.first)) {
                continue;
            }

            GLogField fields[] = {
                { "MESSAGE_ID", "76fb3f2e1e1c445bab253b61124779a8", -1},
                { "MESSAGE", "Tried to create mutex but failed", -1 },
                { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_ERROR), -1},
            };
            g_log_structured_array(G_LOG_LEVEL_ERROR, fields, G_N_ELEMENTS(fields));
        }

        curlsh_setopt(curl_share, CURLSHOPT_LOCKFUNC, &mal_curl_lock_function);
        curlsh_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, &mal_curl_unlock_function);
        curlsh_setopt(curl_share, CURLSHOPT_USERDATA, share_lock_functors.get());
        curlsh_setopt(curl_share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
        curlsh_setopt(curl_share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        curlsh_setopt(curl_share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
    }

    curlp curl_pool::get()
    {
        static const char ua[] = "mal-gtk/0.1.0 (linux) libcurl/7.32.0"; /* Have to lie. */
        curlp easy {curl_easy_init()};
        curl_setopt(easy, CURLOPT_SHARE, curl_share.get());
        curl_setopt(easy, CURLOPT_USERAGENT, ua);

        return easy;
    }

    void curl_pool::involke_lock_function(CURL*, curl_lock_data data, curl_lock_access)
    {
        auto iter = map_mutex.find(data);
        if (G_LIKELY(iter != map_mutex.end())) {
            iter->second.lock();
            return;
        }

        GLogField fields[] = {
            { "MESSAGE_ID", "9d5911ec541548b39dcf9af969e87789", -1},
            { "MESSAGE", "Trying to lock curl data but we can't find the mutex", -1 },
            { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_ERROR), -1},
        };
        g_log_structured_array(G_LOG_LEVEL_ERROR, fields, G_N_ELEMENTS(fields));
    }

    void curl_pool::involke_unlock_function(CURL*, curl_lock_data data) {
        auto iter = map_mutex.find(data);
        if (G_LIKELY(iter != map_mutex.end())) {
            iter->second.unlock();
            return;
        }

        GLogField fields[] = {
            { "MESSAGE_ID", "7505324597fb4e61a3d077c5a332f196", -1},
            { "MESSAGE", "Trying to unlock curl data but we can't find the mutex", -1 },
            { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_ERROR), -1},
        };

        g_log_structured_array(G_LOG_LEVEL_ERROR, fields, G_N_ELEMENTS(fields));
    }

}
