#include <thread>
#include "main_window.hpp"

namespace MAL {

	MainWindow::MainWindow(const std::shared_ptr<MAL>& mal) :
		Gtk::ApplicationWindow(),
		anime_list_view(Gtk::manage(new AnimeListView(mal)))
	{
		add(*anime_list_view);
		anime_list_view->show_all();
	}


}
