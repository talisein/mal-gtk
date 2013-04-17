#include <algorithm>
#include <giomm/simpleaction.h>
#include "application.hpp"
#include "mal.hpp"
#include "user_info.hpp"

namespace MAL {
	Application::Application(int& argc, char**& argv) :
		app(Gtk::Application::create(argc, argv, Glib::ustring(APPLICATION_ID))),
		mal(std::make_shared<MAL>(std::unique_ptr<UserInfo>(new UserInfo()))),
		window(mal)
	{
        auto action = Gio::SimpleAction::create("quit");
        action->signal_activate().connect([&](const Glib::VariantBase&) {
                app->quit();
            });
        app->add_action(action);
        app->add_accelerator("<Control>q", "app.quit", nullptr);
	}

	int Application::run() {
		return app->run(window);
	}

}
