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
