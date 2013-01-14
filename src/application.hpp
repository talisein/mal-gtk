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
