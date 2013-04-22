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

#include <thread>
#include <gtkmm/notebook.h>
#include <gtkmm/stock.h>
#include "main_window.hpp"
#include "malitem_list_view.hpp"

namespace MAL {

	MainWindow::MainWindow(const std::shared_ptr<MAL>& mal) :
		Gtk::ApplicationWindow()
	{
		auto book = Gtk::manage(new Gtk::Notebook());
		book->set_show_border(false);

        {
        auto itemcolumns = std::make_shared<AnimeModelColumnsEditable>();
        auto itemlistview = Gtk::manage(new AnimeListViewEditable(mal, itemcolumns));
        auto itemdetailview = Gtk::manage(new AnimeDetailViewEditable(mal,
                                                                      itemcolumns,
                                                                      sigc::mem_fun(*itemlistview, &AnimeListViewEditable::do_model_foreach)));
        auto itempage = Gtk::manage(new AnimeFilteredListPage(mal, itemlistview, itemdetailview));
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

		book->show();
		add(*book);
		resize(1000,800);
	}


}
