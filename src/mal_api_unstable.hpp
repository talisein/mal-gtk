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
#include <memory>
#include <optional>
#include "user_info.hpp"
#include "mal_curl.hpp"
#include "glibmm.h"

namespace MAL
{

    struct anime_details_unstable
    {
        template<typename T>
        using opt = std::optional<T>;

        opt<Glib::ustring> comments;

        opt<int> times_watched;
        opt<double> storage_value;

        opt<int> priority;
        opt<int> storage_type;
        opt<int> rewatch_value;
        opt<int> discuss;
        opt<int> sns;
    };

    bool mal_unstable_is_logged_in(curlp& curl);
    int mal_unstable_html_login(const std::unique_ptr<UserInfo>& info, curlp& curl, Glib::ustring& error_msg);
    int mal_unstable_anime_details(int64_t series_itemdb_id, const std::unique_ptr<UserInfo>& info, curlp& curl, anime_details_unstable& details, Glib::ustring& error_msg);
}
