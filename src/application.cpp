#include <algorithm>
#include "application.hpp"
#include "mal.hpp"
#include "user_info.hpp"

namespace MAL {
	Application::Application(int& argc, char**& argv) :
		app(Gtk::Application::create(argc, argv, Glib::ustring(APPLICATION_ID))),
		mal(std::make_shared<MAL>(std::unique_ptr<UserInfo>(new UserInfo()))),
		window(mal)
	{

	}

	int Application::run() {
		return app->run(window);
	}

}
