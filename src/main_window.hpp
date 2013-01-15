#include <glibmm/dispatcher.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include "mal.hpp"
#include "anime_list_view.hpp"

namespace MAL {

	class AnimeSearchView : public Gtk::Grid {

	};

	class MainWindow : public Gtk::ApplicationWindow {
	public:
		MainWindow(const std::shared_ptr<MAL>&);

	private:

		AnimeListView *anime_list_view;
		AnimeSearchView *anime_search_view;
	};

}
