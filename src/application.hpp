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

#include <gtkmm/application.h>
#include "main_window.hpp"
#include "mal.hpp"

namespace MAL {

	class Application
	{
	public:
		Application(int& argc, char**& argv);
		
		int run();

	private:
		Glib::RefPtr<Gtk::Application> app;
		std::shared_ptr<MAL> mal;
		MainWindow window;

	};
	
	constexpr char APPLICATION_ID[] = "com.malgtk";
}
