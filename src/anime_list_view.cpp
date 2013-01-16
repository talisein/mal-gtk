#include "anime_list_view.hpp"
#include <iostream>
#include <thread>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

namespace MAL {

	AnimeStatusCellRendererCombo::AnimeStatusCellRendererCombo() :
		Gtk::CellRendererCombo(),
		model(Gtk::ListStore::create(columns))
	{
		auto iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(WATCHING)));
		iter->set_value(columns.status, WATCHING);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(COMPLETED)));
		iter->set_value(columns.status, COMPLETED);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(ONHOLD)));
		iter->set_value(columns.status, ONHOLD);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(DROPPED)));
		iter->set_value(columns.status, DROPPED);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(PLANTOWATCH)));
		iter->set_value(columns.status, PLANTOWATCH);
		property_model() = model;
		property_text_column() = columns.text.index();
		property_has_entry() = false;
	}

	AnimeStatusComboBox::AnimeStatusComboBox() {
			append("Watching");
			append("Completed");
			append("On Hold");
			append("Dropped");
			append("Plan To Watch");
			set_active_text("Watching");
	}

	AnimeStatus AnimeStatusComboBox::get_anime_status() const {
		auto const text = get_active_text();
		if (text.compare("Watching") == 0)
			return WATCHING;
		else if (text.compare("Completed") == 0)
			return COMPLETED;
		else if (text.compare("On Hold") == 0)
			return ONHOLD;
		else if (text.compare("Dropped") == 0)
			return DROPPED;
		else if (text.compare("Plan To Watch") == 0)
			return PLANTOWATCH;
		else
			return ANIMESTATUS_INVALID;
	}

	void AnimeListView::on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter) {
		if (do_updates) {
			Anime anime = iter->get_value(columns.anime);
			const int episodes = iter->get_value(columns.episodes);
			bool is_changed = false;
			if (episodes != anime.episodes) {
				is_changed = true;
				std::cerr << "Episodes is now " << episodes << std::endl;
				anime.episodes = episodes;
				iter->set_value(columns.anime, anime);
			}
			const int score = iter->get_value(columns.score);
			if (score != anime.score) {
				is_changed = true;
				std::cerr << "Score is now " << score << std::endl;
				anime.score = score;
				iter->set_value(columns.anime, anime);
			}

			if (is_changed) {
				std::thread t(std::bind(&AnimeListView::send_anime_update, this, anime));
				t.detach();
			}
		}
	}

	void AnimeListView::on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text) {
		Gtk::TreeModel::iterator iter = model->get_iter(path);

		if (iter) {
			auto status = anime_status_from_string(new_text);
			Anime anime = iter->get_value(columns.anime);
			if (status != anime.status) {
				std::cerr << "Status is now " << to_string(status) << std::endl;
				iter->set_value(columns.status, Glib::ustring(to_string(status)));
				anime.status = status;
				iter->set_value(columns.anime, anime);
				if (do_updates) {
					std::thread t(std::bind(&AnimeListView::send_anime_update, this, anime));
					t.detach();
				} else {
					anime.score = 0;
					if (anime.status == COMPLETED)
						anime.episodes = anime.series_episodes;
					std::thread t(std::bind(&AnimeListView::send_anime_add, this, anime));
					t.detach();
				}
			}
		}
		
	}

	// NOT Executed on the main thread. Be careful!
	void AnimeListView::send_anime_update(Anime anime) {
		auto success = mal->update_anime_sync(anime);
		if (success) {
			auto iter = std::find_if(std::begin(anime_list),
			                         std::end(anime_list),
			                         [&anime](const Anime& a) {
				                         return anime.series_animedb_id == a.series_animedb_id;
			                         });
			if (iter != std::end(anime_list)) {
				// Since we're off the main thread, do a threadsafe iter_swap
				std::list<Anime> l(1, anime);
				std::iter_swap(iter, std::begin(l));
			}
		}
	}

	// NOT Executed on the main thread. Be careful!
	void AnimeListView::send_anime_add(Anime anime) {
		mal->add_anime_sync(anime);
	}

	AnimeListPage::AnimeListPage(const std::shared_ptr<MAL>& mal_p) :
		mal(mal_p),
		status_filter(WATCHING),
		list_view(Gtk::manage(new AnimeListView(mal_p, status_filter)))
	{
		set_orientation(Gtk::ORIENTATION_VERTICAL);
		auto button = Gtk::manage(new Gtk::Button(""));
		button->set_always_show_image(true);
		auto icon = button->render_icon_pixbuf(Gtk::Stock::REFRESH, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		button->set_image(*image);
		button->set_tooltip_text("Refresh data from myanimelist.net\nThis is rarely necessary. If you enter some invalid data here such as a score of 100, refreshing will fix the score to what MAL says, e.g. 10. If you have modified your MAL data via the website since starting this program, a refresh will pull in those changes.");

		status_combo_box = Gtk::manage(new AnimeStatusComboBox());
		add(*status_combo_box);
		attach_next_to(*button, *status_combo_box, Gtk::POS_RIGHT, 1, 1);
		attach_next_to(*list_view, *status_combo_box, Gtk::POS_BOTTOM, 2, 1);
		status_combo_box->set_hexpand(true);
		button->signal_clicked().connect(sigc::mem_fun(*this, &AnimeListPage::refresh_async));
		status_combo_box->signal_changed().connect(sigc::mem_fun(*this, &AnimeListPage::on_filter_changed));
		show_all();
		mal->signal_anime_added.connect(sigc::mem_fun(*this, &AnimeListPage::refresh_async));
		refresh_async();
	}

	void AnimeListPage::on_filter_changed() {
		status_filter = status_combo_box->get_anime_status();
		list_view->set_status_filter(status_filter);
	}

	AnimeListView::AnimeListView(const std::shared_ptr<MAL>& mal_p,
	                             const AnimeStatus filter,
	                             const bool do_updates_) :
		Gtk::Grid(),
		mal(mal_p),
		do_updates(do_updates_),
		status_filter(filter),
		model_changed_functor(sigc::mem_fun(*this, &AnimeListView::on_model_changed)),
		model(Gtk::ListStore::create(columns)),
		treeview(Gtk::manage(new Gtk::TreeView(model)))
	{
		Gtk::ScrolledWindow *sw = Gtk::manage(new Gtk::ScrolledWindow());
		sw->add(*treeview);
		sw->set_hexpand(true);
		sw->set_vexpand(true);
		add(*sw);

		auto title = Gtk::manage(new Gtk::TreeViewColumn("Title", columns.series_title));
		title->set_sort_column(columns.series_title);
		title->set_expand(true);
		treeview->append_column(*title);
		treeview->append_column("Airing Status", columns.series_status);
		auto season = Gtk::manage(new Gtk::TreeViewColumn("Season", columns.series_season));
		auto season_cr = season->get_first_cell();
		season_cr->set_alignment(1.0, 0.5);
		treeview->append_column(*season);
		season->set_sort_column(columns.series_start_date);

		if (do_updates) {
			treeview->append_column_numeric_editable("Score", columns.score, "%.0f");
		} else {
			treeview->append_column_numeric("Score", columns.score, "%.2f");
		}

		treeview->append_column("Type", columns.type);

		if (do_updates)
			treeview->append_column_numeric_editable("Seen", columns.episodes, "%d");

		treeview->append_column("Eps.", columns.series_episodes);
		auto crc            = Gtk::manage(new Gtk::TreeViewColumn("Viewing Status"));
		status_cellrenderer = Gtk::manage(new AnimeStatusCellRendererCombo());
		crc->pack_start(*status_cellrenderer);
		crc->add_attribute(status_cellrenderer->property_text(), columns.status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &AnimeListView::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;

		treeview->append_column(*crc);
		show_all();
		signal_refreshed.connect(sigc::mem_fun(*this, &AnimeListView::refresh_cb));
	}

	void AnimeListView::set_status_filter(const AnimeStatus status) {
		status_filter = status;
		refresh_cb();
	}

	void AnimeListView::set_anime_list(std::list<Anime>& anime) {
		std::swap(anime_list, anime);
		signal_refreshed();
	}

	// Pulls the latest anime list from MAL. Should be done off the
	// main thread.
	void AnimeListPage::refresh() {
		auto list = mal->get_anime_list_sync();
		list_view->set_anime_list(list);
	}

	// Asynchronous call to refresh the anime list from MAL
	void AnimeListPage::refresh_async() {
		std::thread t(std::bind(&AnimeListPage::refresh, this));
		t.detach();
	}

	// Updates the tree model on the main thread.
	void AnimeListView::refresh_cb() {
		if (model_changed_connection.connected())
			model_changed_connection.disconnect();
		model->clear();
		auto iter = std::end(anime_list);
		if (status_filter != ANIMESTATUS_INVALID) {
			iter = std::partition(std::begin(anime_list),
			                      std::end(anime_list),
			                      [&](const Anime& anime) {
				                      return anime.status == status_filter;
			                      });
		}
		std::vector<Anime> animes;
		animes.reserve(std::distance(std::begin(anime_list), iter));
		std::copy(std::begin(anime_list),
		          iter,
		          std::back_inserter(animes));
			
		std::sort(std::begin(animes),
		          std::end(animes),
		          [](const Anime& lhs, const Anime& rhs) {
			          auto season = lhs.series_date_begin.substr(0,7).compare(rhs.series_date_begin.substr(0,7));
			          if (season == 0)
				          return lhs.series_title.compare(rhs.series_title) < 0;
			          else
				          return season > 0;
		          });
		
		
		std::for_each(std::begin(animes),
		              std::end(animes),
		              [&](const Anime& anime) {
			              auto iter = model->append();
			              iter->set_value(columns.series_title, Glib::ustring(anime.series_title));
			              iter->set_value(columns.series_status, Glib::ustring(to_string(anime.series_status)));
			              iter->set_value(columns.series_season, Glib::ustring(anime_season_from_date(anime.series_date_begin)));
			              auto sort_text = anime.series_date_begin.substr(0,7);
			              iter->set_value(columns.series_start_date, Glib::ustring(sort_text));
			              iter->set_value(columns.score, static_cast<float>(anime.score));
			              iter->set_value(columns.type, Glib::ustring(to_string(anime.series_type)));
			              iter->set_value(columns.episodes, static_cast<int>(anime.episodes));
			              iter->set_value(columns.series_episodes, static_cast<int>(anime.series_episodes));
			              if (do_updates) {
				              iter->set_value(columns.status, Glib::ustring(to_string(anime.status)));
			              } else {
				              iter->set_value(columns.status, Glib::ustring("Add To My Anime List..."));
			              }
			              iter->set_value(columns.anime, anime);
		              });
		model_changed_connection = model->signal_row_changed().connect(model_changed_functor);
	}


}
