#pragma once
#include <memory>
#include <giomm/memoryinputstream.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeview.h>
#include <sigc++/slot.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/cellrenderercombo.h>
#include "anime.hpp"
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

    class AnimeModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> series_title;
        Gtk::TreeModelColumn<Glib::ustring> series_status;
        Gtk::TreeModelColumn<float> score;
        Gtk::TreeModelColumn<Glib::ustring> type;
        Gtk::TreeModelColumn<gint> episodes;
        Gtk::TreeModelColumn<gint> series_episodes;
        Gtk::TreeModelColumn<Glib::ustring> status;
        Gtk::TreeModelColumn<Anime> anime;
        Gtk::TreeModelColumn<Glib::ustring> series_season;
        Gtk::TreeModelColumn<Glib::ustring> series_start_date;

        AnimeModelColumns() { add(series_title);
            add(series_status);
            add(score);
            add(type);
            add(episodes);
            add(series_episodes);
            add(status);
            add(series_season);
            add(series_start_date);
            add(anime);
        } 
    };

	class AnimeListView : public Gtk::Grid {
	public:
		AnimeListView(const std::shared_ptr<MAL>&,
		              const AnimeStatus filter = ANIMESTATUS_INVALID,
		              const bool do_updates = true);

		void set_status_filter(const AnimeStatus status);
		void set_anime_list(std::list<Anime>& anime);

		void do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter& slot) {model->foreach(slot);};
		void set_row_activated_cb(sigc::slot<void, const Anime&> slot) { row_activated_cb = slot;} ;

        std::shared_ptr<AnimeModelColumns> columns_p;
	private:
		std::shared_ptr<MAL> mal;

		bool do_updates;
		AnimeStatus status_filter;
		AnimeStatusCellRendererCombo *status_cellrenderer;
		void on_status_cr_changed(const Glib::ustring& text, const Glib::ustring& iter);

		std::list<Anime> anime_list;
        Anime detailed_anime;
		Glib::Dispatcher signal_refreshed;
		void refresh_cb();
		
		const sigc::slot<void, const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&> model_changed_functor;
		sigc::connection model_changed_connection;
		void on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
		void send_anime_update(Anime anime);
		void send_anime_add(Anime anime);

		Glib::RefPtr<Gtk::ListStore> model;
		Gtk::TreeView *treeview;
		void on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
		sigc::slot<void, const Anime&> row_activated_cb;
		
	};

	class AnimeDetailView : public Gtk::Grid {
	public:
		AnimeDetailView(const std::shared_ptr<MAL>&);

		void display_anime(const Anime& anime);
		void set_model_cb(sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> slot, const std::shared_ptr<AnimeModelColumns> &c) {model_cb = slot; columns = c;};
	private:
		std::shared_ptr<MAL> mal;
		Anime anime;
		sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> model_cb;
		bool on_foreach(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
        void on_increment_clicked();
        void update_list_model();

		Gtk::Image                         *image;
		Gtk::Label                         *title;
		Gtk::Button                        *increment_button;
		Gtk::Entry                         *episodes;
		Gtk::Label                         *series_episodes;
        Gtk::Entry                         *score;
		AnimeStatusComboBox                *anime_status;
        std::shared_ptr<AnimeModelColumns>  columns;
		Glib::Dispatcher                signal_image_available;
		Glib::RefPtr<Gio::MemoryInputStream> image_stream;
		void on_image_available();
		void do_fetch_image();

	};

	class AnimeListPage : public Gtk::Grid {
	public:
		AnimeListPage(const std::shared_ptr<MAL>&);
		void refresh_async();


	private:
		
		std::shared_ptr<MAL> mal;
		AnimeStatus status_filter;
		AnimeDetailView* detail_view;
		AnimeListView* list_view;
		AnimeStatusComboBox *status_combo_box;
		void on_filter_changed();
		void refresh();

	};


}
