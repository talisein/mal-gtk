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

#include <gtkmm/notebook.h>
#include <gtkmm/stock.h>
#include "main_window.hpp"
#include "malitem_list_view.hpp"

namespace MAL {

	MainWindow::MainWindow(const std::shared_ptr<MAL>& mal) :
		Gtk::ApplicationWindow(),
        m_mal(mal),
        m_infobar(Gtk::manage(new Gtk::InfoBar())),
        m_infobar_label(Gtk::manage(new Gtk::Label())),
        m_statusbar(Gtk::manage(new Gtk::Statusbar()))
	{
        auto grid = Gtk::manage(new Gtk::Grid());
        grid->set_orientation(Gtk::ORIENTATION_VERTICAL);
        grid->add(*m_infobar);
        Gtk::Container* infoBarContainer = dynamic_cast<Gtk::Container*>(m_infobar->get_content_area());
        if (infoBarContainer) {
            infoBarContainer->add(*m_infobar_label);
            m_infobar->add_button(Gtk::Stock::OK, 0);
        }
        m_infobar->signal_response().connect(sigc::mem_fun(this, &MainWindow::infobar_response_cb));

		auto book = Gtk::manage(new Gtk::Notebook());
		book->set_show_border(false);

        {
        auto itemcolumns = std::make_shared<AnimeModelColumnsEditable>();
        auto itemlistview = Gtk::manage(new AnimeListViewEditable(mal, itemcolumns));
        auto itemdetailview = Gtk::manage(new AnimeDetailViewEditable(mal,
                                                                      itemcolumns,
                                                                      sigc::mem_fun(*itemlistview, &AnimeListViewEditable::do_model_foreach)));
        auto itempage = Gtk::manage(new AnimeFilteredListPage(mal, itemcolumns,  itemlistview, itemdetailview));
        book->append_page(*itempage, "My Anime List");
        }

        {
        auto itemcolumns = std::make_shared<AnimeModelColumnsStatic>();
        auto itemlistview = Gtk::manage(new AnimeListViewStatic(mal, itemcolumns));
        auto itemdetailview = Gtk::manage(new AnimeDetailViewStatic(mal));
        auto itempage = Gtk::manage(new AnimeSearchListPage(mal, itemlistview, itemdetailview));
        book->append_page(*itempage, "Anime Search");
        }

        {
        auto itemcolumns = std::make_shared<MangaModelColumnsEditable>();
        auto itemlistview = Gtk::manage(new MangaListViewEditable(mal, itemcolumns));
        auto itemdetailview = Gtk::manage(new MangaDetailViewEditable(mal,
                                                                      itemcolumns,
                                                                      sigc::mem_fun(*itemlistview, &MangaListViewEditable::do_model_foreach)));
        auto itempage = Gtk::manage(new MangaFilteredListPage(mal, itemlistview, itemdetailview));
        book->append_page(*itempage, "My Manga List");
        }

        {
        auto itemcolumns = std::make_shared<MangaModelColumnsStatic>();
        auto itemlistview = Gtk::manage(new MangaListViewStatic(mal, itemcolumns));
        auto itemdetailview = Gtk::manage(new MangaDetailViewStatic(mal));
        auto itempage = Gtk::manage(new MangaSearchListPage(mal, itemlistview, itemdetailview));
        book->append_page(*itempage, "Manga Search");
        }

        grid->add(*book);
        add(*grid);
        grid->add(*m_statusbar);
        grid->show();
        book->show();
        m_statusbar->show();
        m_infobar->hide();
		resize(1000,800);

        mal->signal_mal_error.connect(sigc::mem_fun(this, &MainWindow::mal_error_cb));
        mal->signal_mal_info.connect(sigc::mem_fun(this, &MainWindow::mal_info_cb));
	}

    void MainWindow::mal_error_cb()
    {
        Glib::ustring msg;
        while (!m_mal->signal_mal_error.empty()) {
            msg.append(m_mal->signal_mal_error.front());
            m_mal->signal_mal_error.pop();
            if (!m_mal->signal_mal_error.empty())
                msg.append("\n");
            else
                break;
        }
        m_infobar_label->set_text(msg);
        m_infobar_label->show();
        m_infobar->set_message_type(Gtk::MESSAGE_ERROR);
        m_infobar->show();
    }

    void MainWindow::mal_info_cb()
    {
        while (!m_mal->signal_mal_info.empty()) {
            m_status_messages.push_back(m_mal->signal_mal_info.front());
            m_mal->signal_mal_info.pop();
        }

        if (!m_status_timeout.connected() && !m_status_messages.empty()) {
            m_status_timeout = Glib::signal_timeout().connect_seconds(sigc::mem_fun(this, &MainWindow::status_timeout_cb), 3);
            m_statusbar->push(m_status_messages.front());
            m_status_messages.pop_front();
        }
    }

    bool MainWindow::status_timeout_cb()
    {
        m_statusbar->pop();
        if (m_status_messages.empty()) {
            m_status_timeout.disconnect();
            return G_SOURCE_REMOVE;
        } else {
            m_statusbar->push(m_status_messages.front());
            m_status_messages.pop_front();
            return G_SOURCE_CONTINUE;
        }
    }

    void MainWindow::infobar_response_cb(int)
    {
        m_infobar->hide();
    }
}
