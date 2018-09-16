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

#include <algorithm>
#include <giomm/simpleaction.h>
#include "application.hpp"
#include "mal.hpp"
#include "user_info.hpp"

namespace MAL {
	Application::Application(int& argc, char**& argv) :
		app(Gtk::Application::create(argc, argv, Glib::ustring(APPLICATION_ID))),
		pool(std::make_shared<curl_pool>()),
		mal(std::make_shared<MAL>(std::unique_ptr<UserInfo>{new UserInfo()}, pool)),
		window(mal)
	{
        auto action = Gio::SimpleAction::create("quit");
        action->signal_activate().connect(sigc::hide(sigc::mem_fun(app.operator->(), &Gtk::Application::quit)));
        app->add_action(action);
        app->add_accelerator("<Control>q", "app.quit", nullptr);
	}

	int Application::run() {
		return app->run(window);
	}

}
