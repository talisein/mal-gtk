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
