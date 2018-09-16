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

#include <libsecret/secret.h>
#include "user_info.hpp"

namespace {
    struct SecretDeleter {
        void operator()(gchar* str) const {
            secret_password_free(str);
        }
    };

    struct SchemaDeleter {
        void operator()(SecretSchema *ptr) const {
            secret_schema_unref(ptr);
        }
    };

    std::unique_ptr<SecretSchema, SchemaDeleter> make_username_schema()
    {
        return std::unique_ptr<SecretSchema, SchemaDeleter>(
            secret_schema_new("com.malgtk.username",
                              SECRET_SCHEMA_NONE,
                              "NULL", SECRET_SCHEMA_ATTRIBUTE_STRING,
                              nullptr)
            );
    }

    std::unique_ptr<SecretSchema, SchemaDeleter> make_password_schema()
    {
        return std::unique_ptr<SecretSchema, SchemaDeleter>{
            secret_schema_new("com.malgtk.password",
                              SECRET_SCHEMA_NONE,
                              "NULL", SECRET_SCHEMA_ATTRIBUTE_STRING,
                              nullptr)
                };
    }
}

namespace MAL {
    class MAL::UserInfo::UserInfoPrivate {
    public:
        UserInfoPrivate() :
            username_schema(make_username_schema()),
            password_schema(make_password_schema()) { }

        ~UserInfoPrivate() = default;

        std::shared_ptr<gchar> username;
        std::shared_ptr<gchar> password;
        std::unique_ptr<SecretSchema, SchemaDeleter> username_schema;
        std::unique_ptr<SecretSchema, SchemaDeleter> password_schema;
    };

    UserInfo::UserInfo() :
        pimpl{new UserInfoPrivate()}
    {
        lookup_details();
    }

    UserInfo::~UserInfo() = default;

    void UserInfo::lookup_details() {
        gchar *cu = secret_password_lookup_nonpageable_sync(pimpl->username_schema.get(),
                                                            nullptr,
                                                            nullptr,
                                                            NULL);
        if (cu) {
            std::shared_ptr<gchar> u(cu, SecretDeleter());
            pimpl->username = u;
        };

        gchar *cp = secret_password_lookup_nonpageable_sync(pimpl->password_schema.get(),
                                                nullptr,
                                                nullptr,
                                                NULL);
        if (cp) {
            std::shared_ptr<gchar> p(cp, SecretDeleter());
            pimpl->password = p;
        };
    }

    void UserInfo::set_details(const std::string& username, const std::string& password) {
        gboolean user_result = secret_password_store_sync(pimpl->username_schema.get(),
                                                          SECRET_COLLECTION_SESSION,
                                                          "MAL Username",
                                                          username.c_str(),
                                                          nullptr,
                                                          nullptr,
                                                          NULL);

        gboolean pass_result = secret_password_store_sync(pimpl->password_schema.get(),
                                                          SECRET_COLLECTION_SESSION,
                                                          "MAL Password",
                                                          password.c_str(),
                                                          nullptr,
                                                          nullptr,
                                                          NULL);
        if (user_result && pass_result)
            lookup_details();
    }

    bool UserInfo::has_details() const
    {
        return pimpl->username && pimpl->password;
    }

    std::shared_ptr<gchar> UserInfo::get_username() const
    {
        return pimpl->username;
    }

    secret_string UserInfo::get_username_str() const
    {
        return pimpl->username.get();
    }

    std::shared_ptr<gchar> UserInfo::get_password() const
    {
        return pimpl->password;
    }

    secret_string UserInfo::get_password_str() const
    {
        return pimpl->password.get();
    }


}
