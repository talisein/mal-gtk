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
#include <libsecret/secret.h>
#include <memory>

namespace MAL {

    class UserInfo {
    public:
        UserInfo();
        ~UserInfo();
        UserInfo(const UserInfo&) = delete;
        void operator=(const UserInfo&) = delete;

        void set_details(const std::string& username, const std::string& password);
        bool has_details() const;

        std::shared_ptr<gchar> get_username() const;
        std::shared_ptr<gchar> get_password() const;

    private:
        void lookup_details();
        class UserInfoPrivate;
        std::unique_ptr<UserInfoPrivate> pimpl;
    };

}
