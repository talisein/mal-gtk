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

#pragma once
#include <type_traits>
#include <string>
#include <sstream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <map>
#include <curl/curl.h>
#include "secret_allocator.h"
#include "glib.h"
#include "logging.hpp"

namespace
{
    template<typename T>
    void set_option_value_field(GLogField& field, T&& val, std::string &buf)
    {
        if constexpr (std::is_array_v<std::remove_reference_t<T>>) {
            field.value = val;
            if constexpr (std::is_same_v<char, std::remove_cv_t<std::remove_extent_t<std::remove_reference_t<T>>>>) {
                field.length = -1;
            } else {
                field.length = sizeof(val);
            }
        } else if constexpr (std::is_same_v<char *, std::remove_reference_t<T>> ||
                             std::is_same_v<const char *, std::remove_reference_t<T>>)
        {
            /* Don't log passwords */
            if (!gcr_secure_memory_is_secure(const_cast<char *>(val))) {
                field.value  = val;
                field.length = -1;
            } else {
                field.value  = "SECRET";
                field.length = -1;
            }
        } else if constexpr (std::is_member_pointer_v<std::remove_reference_t<T>> ||
                             std::is_pointer_v<std::decay_t<std::remove_reference_t<T>>>)
        {
            field.value  = reinterpret_cast<gconstpointer>(val);
            field.length = 0;
        } else if constexpr (std::is_arithmetic_v<std::remove_reference_t<T>>) {
            buf          = std::to_string(val);
            field.value  = buf.c_str();
            field.length = -1;
        } else {
            field.value  = std::addressof(val);
            field.length = sizeof(T);
        }
    }
}

namespace MAL
{
    /* Deleters for libcurl */
    struct CURLEscapeDeleter {
        void operator()(char* str) const;
    };

    struct CURLEasyDeleter {
        void operator()(CURL* curl) const;
    };

    struct CURLShareDeleter {
        void operator()(CURLSH* share) const;
    };

    struct CURLSListDeleter {
        void operator()(struct curl_slist* list) const;
    };

    using curlp = std::unique_ptr<CURL, CURLEasyDeleter>;
    using curlshp = std::unique_ptr<CURLSH, CURLShareDeleter>;
    using curlstr = std::unique_ptr<char, CURLEscapeDeleter>;
    using curlslistp = std::unique_ptr<struct curl_slist, CURLSListDeleter>;

    std::string curl_escape(const curlp& curl, const std::string& str);
    secret_string curl_escape(const curlp& curl, const secret_string& str);

    template<typename T>
    int curlsh_setopt(curlshp& curlsh, CURLSHoption option, T&& val)
    {
        CURLSHcode code = curl_share_setopt(curlsh.get(), option, std::forward<T>(val));
        if (G_UNLIKELY(CURLSHE_OK != code)) {
            const char *estr = curl_share_strerror(code);
            GLogField fields[] = {
                { "CURL_SHARE_OPTION_VALUE", NULL, 0},
                { "CURL_SHARE_OPTION", &option, sizeof(CURLSHoption) },
                { "CURL_SHARE_ERRORCODE", &code, sizeof(CURLSHcode) },
                { "CURL_SHARE_STRERROR", estr ? estr : "", -1 },
                { "MESSAGE_ID", "f7c86224686d47e18e9dfbe8906aa3d4", -1 },
                { "MESSAGE", "Failed to set CURLSH option", -1 },
                { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_WARNING), -1},
            };

            std::string buf;
            set_option_value_field(fields[0], std::forward<T>(val), buf);

            g_log_structured_array(G_LOG_LEVEL_WARNING, fields, G_N_ELEMENTS(fields));
            return -1;
        }

        return 0;
    }

    template<typename T>
    int curl_setopt(curlp& curl, CURLoption option, T&& val)
    {
        CURLcode code = curl_easy_setopt(curl.get(), option, std::forward<T>(val));
        if (G_UNLIKELY(CURLE_OK != code)) {
            const char *estr = curl_easy_strerror(code);
            GLogField fields[] = {
                { "CURL_EASY_OPTION_VALUE", NULL, 0},
                { "CURL_EASY_OPTION", &option, sizeof(CURLoption) },
                { "CURL_EASY_ERRORCODE", &code, sizeof(CURLcode) },
                { "CURL_EASY_STRERROR", estr ? estr : "", -1 },
                { "MESSAGE_ID", "66d3f6b87b424082b83d88ac918dcf4c", -1 },
                { "MESSAGE", "Failed to set CURL option", -1 },
                { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_WARNING), -1},
            };

            std::string buf;
            set_option_value_field(fields[0], std::forward<T>(val), buf);

            g_log_structured_array(G_LOG_LEVEL_WARNING, fields, G_N_ELEMENTS(fields));
            return -1;
        }

        return 0;
    }

    template<typename T>
    int curl_getinfo(curlp& curl, CURLINFO info, T&& val)
    {
        CURLcode code = curl_easy_getinfo(curl.get(), info, std::forward<T>(val));
        if (G_UNLIKELY(CURLE_OK != code)) {
            std::stringstream ss;
            const char *estr = curl_easy_strerror(code);
            ss << "Error getting curl info";
            if (estr) ss << ": " << estr;
            GLogField fields[] = {
                { "CURL_EASY_INFO_VALUE", NULL, 0},
                { "CURL_EASY_INFO", &info, sizeof(CURLINFO) },
                { "CURL_EASY_ERRORCODE", &code, sizeof(CURLcode) },
                { "CURL_EASY_STRERROR", estr ? estr : "", -1 },
                { "MESSAGE_ID", "2e8e202192914fc9ae0eb07b998e481c", -1 },
                { "MESSAGE", ss.str().c_str(), -1 },
                { "PRIORITY", log_level_to_priority(G_LOG_LEVEL_WARNING), -1},
            };
            std::string buf;
            set_option_value_field(fields[0], std::forward<T>(val), buf);

            g_log_structured_array(G_LOG_LEVEL_WARNING, fields, G_N_ELEMENTS(fields));
            return -1;
        }

        return 0;
    }

    inline curl_slist*
    curl_getcookies(curlp& curl)
    {
        struct curl_slist *cookies = nullptr;
        curl_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
        return cookies;
    }

    CURLcode curl_perform(curlp& curl);

    class curl_pool
    {
    public:
        curl_pool();

        curlp get();

        typedef std::function<void (CURL*, curl_lock_data, curl_lock_access)> lock_functor_t;
        typedef std::function<void (CURL*, curl_lock_data)> unlock_functor_t;
        typedef std::pair<lock_functor_t, unlock_functor_t> pair_lock_functor_t;
    private:
        void involke_lock_function(CURL*, curl_lock_data, curl_lock_access);
        void involke_unlock_function(CURL*, curl_lock_data);
        std::unique_ptr<pair_lock_functor_t> share_lock_functors;
        std::map<curl_lock_data, std::mutex> map_mutex;
        curlshp curl_share;
    };
}
