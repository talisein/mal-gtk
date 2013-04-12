#include <glibmm/dispatcher.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include "mal.hpp"
#include "anime_list_view.hpp"
#include "manga_list_view.hpp"

namespace MAL {
	class MainWindow : public Gtk::ApplicationWindow {
	public:
		MainWindow(const std::shared_ptr<MAL>&);

	private:
	};
}
