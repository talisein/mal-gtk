#include <glibmm/dispatcher.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include "mal.hpp"
#include "anime_list_view.hpp"
#include "manga_list_view.hpp"

namespace MAL {

	class AnimeSearchPage : public Gtk::Grid {
	public:
		AnimeSearchPage(const std::shared_ptr<MAL>&);

	private:
		std::shared_ptr<MAL> mal;
		void do_search_async();
		void do_search();
		
		Gtk::Entry *entry;
		std::list<Anime> search_list;
		AnimeListView *list_view;
	};

	class MangaSearchPage : public Gtk::Grid {
	public:
		MangaSearchPage(const std::shared_ptr<MAL>&);

	private:
		std::shared_ptr<MAL> mal;
		void do_search_async();
		void do_search();

		Gtk::Entry *entry;
		std::list<Manga> search_list;
		MangaListView *list_view;
	};


	class MainWindow : public Gtk::ApplicationWindow {
	public:
		MainWindow(const std::shared_ptr<MAL>&);

	private:

		AnimeListPage *anime_list_view;
		MangaListPage *manga_list_view;
		AnimeSearchPage *anime_search_view;
		MangaSearchPage *manga_search_view;
	};

}
