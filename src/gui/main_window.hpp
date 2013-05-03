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

#include <glibmm/dispatcher.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/infobar.h>
#include <gtkmm/statusbar.h>
#include "mal.hpp"
#include "anime_list_view.hpp"
#include "manga_list_view.hpp"

namespace MAL {
	class MainWindow : public Gtk::ApplicationWindow {
	public:
		MainWindow(const std::shared_ptr<MAL>&);

	private:
        std::shared_ptr<MAL>       m_mal;
        Gtk::InfoBar              *m_infobar;
        Gtk::Label                *m_infobar_label;
        Gtk::Statusbar            *m_statusbar;
        std::deque<Glib::ustring>  m_status_messages;
        sigc::connection           m_status_timeout;

        void mal_error_cb();
        void infobar_response_cb(int);
        void mal_info_cb();
        bool status_timeout_cb();
	};
}
