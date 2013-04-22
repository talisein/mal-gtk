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

#include <string>
#include "user_info.hpp"

namespace {
	extern "C" {
		static const SecretSchema* get_username_schema() {
			static const SecretSchema username_schema = {
				"com.malgtk.username", SECRET_SCHEMA_NONE,
				{
					{ "NULL", SECRET_SCHEMA_ATTRIBUTE_STRING },
				}
			};
			return &username_schema;
		}

		static const SecretSchema* get_password_schema() {
			static const SecretSchema password_schema = {
				"com.malgtk.password", SECRET_SCHEMA_NONE,
				{
					{ "NULL", SECRET_SCHEMA_ATTRIBUTE_STRING },
				}
			};
			return &password_schema;
		}
	}
}

namespace MAL {

	UserInfo::UserInfo()
	{
		lookup_details();
	}

	void UserInfo::lookup_details() {
		gchar *cu = secret_password_lookup_nonpageable_sync(get_username_schema(),
		                                                    nullptr,
		                                                    nullptr,
		                                                    NULL);
		if (cu) {
			std::shared_ptr<gchar> u(cu, SecretDeleter());
			username = u;
		};

		gchar *cp = secret_password_lookup_nonpageable_sync(get_password_schema(),
		                                        nullptr,
		                                        nullptr,
		                                        NULL);
		if (cp) {
			std::shared_ptr<gchar> p(cp, SecretDeleter());
			password = p;
		};
	}

	void UserInfo::set_details(const std::string& username, const std::string& password) {
		gboolean user_result = secret_password_store_sync(get_username_schema(),
		                                                  nullptr,
		                                                  "MAL Username",
		                                                  username.c_str(),
		                                                  nullptr,
		                                                  nullptr,
		                                                  NULL);

		gboolean pass_result = secret_password_store_sync(get_password_schema(),
		                                                  nullptr,
		                                                  "MAL Password",
		                                                  password.c_str(),
		                                                  nullptr,
		                                                  nullptr,
		                                                  NULL);
		if (user_result && pass_result)
			lookup_details();
	}

}
