#pragma once
#include <libsecret/secret.h>
#include <memory>

namespace MAL {

	struct SecretDeleter {
		void operator()(gchar* str) const {
			secret_password_free(str);
		}
	};

	class UserInfo {
	public:
		UserInfo();
		UserInfo(const UserInfo&) = delete;
		void operator=(const UserInfo&) = delete;

		void set_details(const std::string& username, const std::string& password);
		bool has_details() const { return username && password; };

		std::shared_ptr<gchar> get_username() const {return username;};
		std::shared_ptr<gchar> get_password() const {return password;};
		
	private:
		void lookup_details();
		std::shared_ptr<gchar> username;
		std::shared_ptr<gchar> password;
	};

}
