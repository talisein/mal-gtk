#include <thread>
#include <gtkmm/notebook.h>
#include <gtkmm/stock.h>
#include "main_window.hpp"

namespace MAL {

	AnimeSearchPage::AnimeSearchPage(const std::shared_ptr<MAL>& mal_p) :
		mal(mal_p),
		entry(Gtk::manage(new Gtk::Entry())),
		list_view(Gtk::manage(new AnimeListView(mal_p, ANIMESTATUS_INVALID, false)))
	{
		set_orientation(Gtk::ORIENTATION_VERTICAL);
		entry->set_hexpand(true);
		entry->set_activates_default(true);
		entry->show();
		add(*entry);
		auto action = Gtk::Action::create();
		action->signal_activate().connect(sigc::mem_fun(*this, &AnimeSearchPage::do_search_async));
		auto button = Gtk::manage(new Gtk::Button("Search"));
		button->set_always_show_image(true);
		auto icon = button->render_icon_pixbuf(Gtk::Stock::FIND, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		button->set_image(*image);
		button->set_tooltip_text("Search myanimelist.net for anime that maches the entered terms.");

		button->set_related_action(action);
		attach_next_to(*button, *entry, Gtk::POS_RIGHT, 1, 1);
		button->show();
		show();
		button->set_can_default(true);
		button->set_receives_default(true);
		attach_next_to(*list_view, *entry, Gtk::POS_BOTTOM, 2, 1);
		show_all();
	}

	void AnimeSearchPage::do_search_async() {
		auto t = std::thread(std::bind(&AnimeSearchPage::do_search, this));
		t.detach();
	}

	void AnimeSearchPage::do_search() {
		auto list = mal->search_anime_sync(entry->get_text());

		if (list.size() > 0) {
			list_view->set_anime_list(list);
		}
	}

	MainWindow::MainWindow(const std::shared_ptr<MAL>& mal) :
		Gtk::ApplicationWindow(),
		anime_list_view(Gtk::manage(new AnimeListPage(mal))),
		anime_search_view(Gtk::manage(new AnimeSearchPage(mal)))
	{
		auto book = Gtk::manage(new Gtk::Notebook());
		book->set_show_border(false);
		book->append_page(*anime_list_view, "My Anime List");
		book->append_page(*anime_search_view, "Search");
		book->show_all();
		add(*book);
		anime_list_view->show_all();
		resize(1200,600);
	}


}
