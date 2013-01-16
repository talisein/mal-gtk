#pragma once
#include <memory>
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


	class AnimeListView : public Gtk::Grid {
	public:
		AnimeListView(const std::shared_ptr<MAL>&,
		              const AnimeStatus filter = ANIMESTATUS_INVALID,
		              const bool do_updates = true);

		void set_status_filter(const AnimeStatus status);
		void set_anime_list(std::list<Anime>& anime);
	private:
		std::shared_ptr<MAL> mal;
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
		} columns;

		bool do_updates;
		AnimeStatus status_filter;
		AnimeStatusCellRendererCombo *status_cellrenderer;
		void on_status_cr_changed(const Glib::ustring& text, const Glib::ustring& iter);

		std::list<Anime> anime_list;
		Glib::Dispatcher signal_refreshed;
		void refresh_cb();
		
		const sigc::slot<void, const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&> model_changed_functor;
		sigc::connection model_changed_connection;
		void on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
		void send_anime_update(Anime anime);
		void send_anime_add(Anime anime);

		Glib::RefPtr<Gtk::ListStore> model;
		Gtk::TreeView *treeview;
	};

	class AnimeListPage : public Gtk::Grid {
	public:
		AnimeListPage(const std::shared_ptr<MAL>&);
		void refresh_async();

	private:
		std::shared_ptr<MAL> mal;
		AnimeStatus status_filter;
		AnimeListView* list_view;
		AnimeStatusComboBox *status_combo_box;
		void on_filter_changed();
		void refresh();

	};


}
