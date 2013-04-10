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
#include "manga.hpp"
#include "mal.hpp"

namespace MAL {

	class MangaStatusColumns : public Gtk::TreeModel::ColumnRecord {
	public:
        Gtk::TreeModelColumn<Glib::ustring> text;
        Gtk::TreeModelColumn<MangaStatus> status;
		MangaStatusColumns() { add(text); add(status); };
	};

	class MangaStatusCellRendererCombo : public Gtk::CellRendererCombo {
	public:
		MangaStatusCellRendererCombo();

	private:
		MangaStatusColumns columns;
		Glib::RefPtr<Gtk::ListStore> model;
	};

	class MangaStatusComboBox : public Gtk::ComboBoxText {
	public:
		MangaStatusComboBox();
		MangaStatus get_manga_status() const;
	};

    class MangaModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> series_title;
        Gtk::TreeModelColumn<Glib::ustring> series_status;
        Gtk::TreeModelColumn<float> score;
        Gtk::TreeModelColumn<Glib::ustring> type;
        Gtk::TreeModelColumn<gint> chapters;
        Gtk::TreeModelColumn<gint> volumes;
        Gtk::TreeModelColumn<gint> series_chapters;
        Gtk::TreeModelColumn<gint> series_volumes;
        Gtk::TreeModelColumn<Glib::ustring> status;
        Gtk::TreeModelColumn<Manga> manga;
        Gtk::TreeModelColumn<Glib::ustring> series_season;
        Gtk::TreeModelColumn<Glib::ustring> series_start_date;

        MangaModelColumns() { add(series_title);
            add(series_status);
            add(score);
            add(type);
            add(chapters);
            add(volumes);
            add(series_chapters);
            add(series_volumes);
            add(status);
            add(series_season);
            add(series_start_date);
            add(manga);
        } 
    };

	class MangaListView : public Gtk::Grid {
	public:
		MangaListView(const std::shared_ptr<MAL>&,
		              const MangaStatus filter = MANGASTATUS_INVALID,
		              const bool do_updates = true);

		void set_status_filter(const MangaStatus status);
		void set_manga_list(std::list<Manga>& manga);

		void do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter& slot) {model->foreach(slot);};
		void set_row_activated_cb(sigc::slot<void, const Manga&> slot) { row_activated_cb = slot;} ;

        std::shared_ptr<MangaModelColumns> columns_p;
	private:
		std::shared_ptr<MAL> mal;

		bool do_updates;
		MangaStatus status_filter;
		MangaStatusCellRendererCombo *status_cellrenderer;
		void on_status_cr_changed(const Glib::ustring& text, const Glib::ustring& iter);

		std::list<Manga> manga_list;
        Manga detailed_manga;
		Glib::Dispatcher signal_refreshed;
		void refresh_cb();
		
		const sigc::slot<void, const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&> model_changed_functor;
		sigc::connection model_changed_connection;
		void on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
		void send_manga_update(Manga manga);
		void send_manga_add(Manga manga);

		Glib::RefPtr<Gtk::ListStore> model;
		Gtk::TreeView *treeview;
		void on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
		sigc::slot<void, const Manga&> row_activated_cb;
		
	};

	class MangaDetailView : public Gtk::Grid {
	public:
		MangaDetailView(const std::shared_ptr<MAL>&);

		void display_manga(const Manga& manga);
		void set_model_cb(sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> slot, const std::shared_ptr<MangaModelColumns> &c) {model_cb = slot; columns = c;};
	private:
		std::shared_ptr<MAL> mal;
		Manga manga;
		sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> model_cb;
		bool on_foreach(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
        void on_increment_chap_clicked();
        void on_increment_vol_clicked();
        void update_list_model();

		Gtk::Image                          *image;
		Gtk::Label                          *title;
		Gtk::Button                         *increment_chap_button;
		Gtk::Button                         *increment_vol_button;
		Gtk::Entry                          *chapters;
		Gtk::Entry                          *volumes;
		Gtk::Label                          *series_chapters;
		Gtk::Label                          *series_volumes;
        Gtk::Entry                          *score;
		MangaStatusComboBox                 *manga_status;
        std::shared_ptr<MangaModelColumns>   columns;
		Glib::Dispatcher                     signal_image_available;
		Glib::RefPtr<Gio::MemoryInputStream> image_stream;
		void on_image_available();
		void do_fetch_image();

	};

	class MangaListPage : public Gtk::Grid {
	public:
		MangaListPage(const std::shared_ptr<MAL>&);
		void refresh_async();

    private:
		std::shared_ptr<MAL> mal;
		MangaStatus status_filter;
		MangaDetailView* detail_view;
		MangaListView* list_view;
		MangaStatusComboBox *status_combo_box;
		void on_filter_changed();
		void refresh();
	};


}
