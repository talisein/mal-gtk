#include <thread>
#include "main_window.hpp"
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>

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
				std::thread t(std::bind(&AnimeListView::send_anime_update, this, anime));
				t.detach();
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

	AnimeListView::AnimeListView(const std::shared_ptr<MAL>& mal_p) :
		Gtk::Grid(),
		mal(mal_p),
		status_filter(WATCHING),
		model_changed_functor(sigc::mem_fun(*this, &AnimeListView::on_model_changed)),
		model(Gtk::ListStore::create(columns)),
		treeview(Gtk::manage(new Gtk::TreeView(model)))
	{
		set_orientation(Gtk::ORIENTATION_VERTICAL);
		Gtk::ScrolledWindow *sw = Gtk::manage(new Gtk::ScrolledWindow());
		sw->add(*treeview);
		sw->set_hexpand(true);
		sw->set_vexpand(true);
		status_combo_box = Gtk::manage(new AnimeStatusComboBox());
		add(*status_combo_box);
		add(*sw);
		status_combo_box->signal_changed().connect(sigc::mem_fun(*this, &AnimeListView::on_filter_changed));
		treeview->append_column("Title", columns.series_title);
		treeview->append_column("Status", columns.series_status);
		treeview->append_column_numeric_editable("Score", columns.score, "%d");
		treeview->append_column("Type", columns.type);
		treeview->append_column_numeric_editable("Seen", columns.episodes, "%d");
		treeview->append_column("Eps.", columns.series_episodes);
		auto crc            = Gtk::manage(new Gtk::TreeViewColumn("Status"));
		status_cellrenderer = Gtk::manage(new AnimeStatusCellRendererCombo());
		crc->pack_start(*status_cellrenderer);
		crc->add_attribute(status_cellrenderer->property_text(), columns.status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &AnimeListView::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;

		treeview->append_column(*crc);
		show_all();
		signal_refreshed.connect(sigc::mem_fun(*this, &AnimeListView::refresh_cb));
		refresh_async();
	}

	void AnimeListView::on_filter_changed() {
		status_filter = status_combo_box->get_anime_status();
		refresh_cb();
	}

	// Pulls the latest anime list from MAL. Should be done off the
	// main thread.
	void AnimeListView::refresh() {
		auto list = mal->get_anime_list_sync();
		std::swap(anime_list, list);

		signal_refreshed();
	}

	// Asynchronous call to refresh the anime list from MAL
	void AnimeListView::refresh_async() {
		std::thread t(std::bind(&AnimeListView::refresh, this));
		t.detach();
	}

	// Updates the tree model on the main thread.
	void AnimeListView::refresh_cb() {
		if (model_changed_connection.connected())
			model_changed_connection.disconnect();
		model->clear();
		auto iter = std::partition(std::begin(anime_list),
		                           std::end(anime_list),
		                           [&](const Anime& anime) {
			                           return anime.status == status_filter;
		                           });
		std::for_each(std::begin(anime_list),
		              iter,
		              [&](const Anime& anime) {
			              auto iter = model->append();
			              iter->set_value(columns.series_title, Glib::ustring(anime.series_title));
			              iter->set_value(columns.series_status, Glib::ustring(to_string(anime.series_status)));
			              iter->set_value(columns.score, static_cast<int>(anime.score));
			              iter->set_value(columns.type, Glib::ustring(to_string(anime.series_type)));
			              iter->set_value(columns.episodes, static_cast<int>(anime.episodes));
			              iter->set_value(columns.series_episodes, static_cast<int>(anime.series_episodes));
			              iter->set_value(columns.status, Glib::ustring(to_string(anime.status)));
			              iter->set_value(columns.anime, anime);
		              });
		model_changed_connection = model->signal_row_changed().connect(model_changed_functor);
	}

	MainWindow::MainWindow(const std::shared_ptr<MAL>& mal) :
		Gtk::ApplicationWindow(),
		anime_list_view(Gtk::manage(new AnimeListView(mal)))
	{
		add(*anime_list_view);
		anime_list_view->show_all();
	}


}
