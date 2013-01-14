#include <glibmm/dispatcher.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeview.h>
#include <sigc++/slot.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/cellrenderercombo.h>
#include "mal.hpp"

namespace MAL {
	class AnimeStatusColumns : public Gtk::TreeModel::ColumnRecord {
	public:
			Gtk::TreeModelColumn<Glib::ustring> text;
			Gtk::TreeModelColumn<AnimeStatus> status;
		AnimeStatusColumns() { add(text); add(status); };
	};

	class AnimeStatusCellRendererCombo : public Gtk::CellRendererCombo {
	public:
		AnimeStatusCellRendererCombo();

	private:
		AnimeStatusColumns columns;
		Glib::RefPtr<Gtk::ListStore> model;
	};

	class AnimeStatusComboBox : public Gtk::ComboBoxText {
	public:
		AnimeStatusComboBox();
		AnimeStatus get_anime_status() const;
	};

	class AnimeListView : public Gtk::Grid {
	public:
		AnimeListView(const std::shared_ptr<MAL>&);

		void refresh_async();
	private:
		std::shared_ptr<MAL> mal;
		class AnimeModelColumns : public Gtk::TreeModel::ColumnRecord
		{
		public:
			Gtk::TreeModelColumn<Glib::ustring> series_title;
			Gtk::TreeModelColumn<Glib::ustring> series_status;
			Gtk::TreeModelColumn<gint> score;
			Gtk::TreeModelColumn<Glib::ustring> type;
			Gtk::TreeModelColumn<gint> episodes;
			Gtk::TreeModelColumn<gint> series_episodes;
			Gtk::TreeModelColumn<Glib::ustring> status;
			Gtk::TreeModelColumn<Anime> anime;

			AnimeModelColumns() { add(series_title);
				add(series_status);
				add(score);
				add(type);
				add(episodes);
				add(series_episodes);
				add(status);
				add(anime);
			} 
		} columns;
		

		AnimeStatus status_filter;
		AnimeStatusComboBox *status_combo_box;
		void on_filter_changed();

		AnimeStatusCellRendererCombo *status_cellrenderer;
		void on_status_cr_changed(const Glib::ustring& text, const Glib::ustring& iter);

		std::list<Anime> anime_list;
		void refresh();
		void refresh_cb();
		Glib::Dispatcher signal_refreshed;
		
		const sigc::slot<void, const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&> model_changed_functor;
		sigc::connection model_changed_connection;
		void on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
		void send_anime_update(Anime anime);

		Glib::RefPtr<Gtk::ListStore> model;
		Gtk::TreeView *treeview;
	};

	class MainWindow : public Gtk::ApplicationWindow {
	public:
		MainWindow(const std::shared_ptr<MAL>&);

	private:

		AnimeListView *anime_list_view;

	};

}
