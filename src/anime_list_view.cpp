#include "anime_list_view.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <glibmm/markup.h>
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
			Anime anime = iter->get_value(columns_p->anime);
			const int episodes = iter->get_value(columns_p->episodes);
			bool is_changed = false;
			if (episodes != anime.episodes) {
				is_changed = true;
				anime.episodes = episodes;
				iter->set_value(columns_p->anime, anime);
			}
			const int score = iter->get_value(columns_p->score);
			if (score != anime.score) {
				is_changed = true;
				anime.score = score;
				iter->set_value(columns_p->anime, anime);
			}

			if (is_changed) {
				std::thread t(std::bind(&AnimeListView::send_anime_update, this, anime));
				t.detach();
                if (detailed_anime.series_animedb_id == anime.series_animedb_id)
                    row_activated_cb(anime);
			}
		}
	}

	void AnimeListView::on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text) {
		Gtk::TreeModel::iterator iter = model->get_iter(path);

		if (iter) {
			auto status = anime_status_from_string(new_text);
			Anime anime = iter->get_value(columns_p->anime);
			if (status != anime.status) {
				iter->set_value(columns_p->status, Glib::ustring(to_string(status)));
				anime.status = status;
				iter->set_value(columns_p->anime, anime);
				if (do_updates) {
					std::thread t(std::bind(&AnimeListView::send_anime_update, this, anime));
					t.detach();
				} else {
					anime.score = 0.0f;
					if (anime.status == COMPLETED)
						anime.episodes = anime.series_episodes;
                    else
                        anime.episodes = 0;
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
		button->set_tooltip_text("Refresh data from myanimelist.net\nThis is "
                                 "rarely necessary. If you enter some invalid"
                                 " data here such as a score of 100, refreshi"
                                 "ng will fix the score to what MAL says, e.g"
                                 ". 10. If you have modified your MAL data vi"
                                 "a the website since starting this program, "
                                 "a refresh will pull in those changes.");
		mal->signal_anime_added.connect(sigc::mem_fun(*this, &AnimeListPage::refresh_async));
		
		auto filter_label = Gtk::manage(new Gtk::Label("Filter: "));
		status_combo_box = Gtk::manage(new AnimeStatusComboBox());
		detail_view = Gtk::manage(new AnimeDetailView(mal));
		detail_view->set_model_cb(sigc::mem_fun(*list_view, &AnimeListView::do_model_foreach), list_view->columns_p);
		list_view->set_row_activated_cb(sigc::mem_fun(*detail_view, &AnimeDetailView::display_anime));

		status_combo_box->set_hexpand(true);
		button->signal_clicked().connect(sigc::mem_fun(*this, &AnimeListPage::refresh_async));
		status_combo_box->signal_changed().connect(sigc::mem_fun(*this, &AnimeListPage::on_filter_changed));

		attach(*detail_view, 0, 0, 3, 1);
		attach(*filter_label, 0, 1, 1, 1);
		attach_next_to(*status_combo_box, *filter_label, Gtk::POS_RIGHT, 1, 1);
		attach_next_to(*button, *status_combo_box, Gtk::POS_RIGHT, 1, 1);
		attach_next_to(*list_view, *filter_label, Gtk::POS_BOTTOM, 3, 1);
		show_all();
		detail_view->hide();

		refresh_async();
	}

	void AnimeListPage::on_filter_changed() {
		status_filter = status_combo_box->get_anime_status();
		list_view->set_status_filter(status_filter);
	}

	AnimeDetailView::AnimeDetailView(const std::shared_ptr<MAL>& mal_p) :
		mal(mal_p),
		image(Gtk::manage(new Gtk::Image())),
		title(Gtk::manage(new Gtk::Label())),
		increment_button(Gtk::manage(new Gtk::Button())),
		episodes(Gtk::manage(new Gtk::Entry())),
		series_episodes(Gtk::manage(new Gtk::Label())),
        score(Gtk::manage(new Gtk::Entry())),
		anime_status(Gtk::manage(new AnimeStatusComboBox()))
	{
        auto score_label = Gtk::manage(new Gtk::Label("Score: "));
        auto grid_right = Gtk::manage(new Gtk::Grid());
		set_vexpand(false);
        set_column_spacing(5);
        grid_right->set_row_spacing(5);
		//               left, top, width, height
		attach(*image,      0,   0,     1,      2);
		attach(*title,      1,   0,     2,      1);
        attach(*grid_right, 1,   1,     1,      1);
        grid_right->attach(*anime_status, 0, 0, 3, 1);
		grid_right->attach_next_to(*increment_button, *anime_status,     Gtk::POS_BOTTOM, 1, 1);
		grid_right->attach_next_to(*episodes,         *increment_button, Gtk::POS_RIGHT,  1, 1);
		grid_right->attach_next_to(*series_episodes,  *episodes,         Gtk::POS_RIGHT,  1, 1);
		grid_right->attach_next_to(*score_label,      *increment_button, Gtk::POS_BOTTOM, 1, 1);
		grid_right->attach_next_to(*score,            *score_label,      Gtk::POS_RIGHT,  1, 1);
        title->set_alignment(Gtk::ALIGN_CENTER);
        title->set_hexpand(true);
        title->set_vexpand(false);
        grid_right->set_vexpand(true);
		increment_button->set_always_show_image(true);
		auto icon = increment_button->render_icon_pixbuf(Gtk::Stock::ADD, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		increment_button->set_image(*image);
        increment_button->signal_clicked().connect(sigc::mem_fun(*this, &AnimeDetailView::on_increment_clicked));
		episodes->set_width_chars(4);
		episodes->set_has_frame(false);
        episodes->set_alignment(Gtk::ALIGN_END);

        episodes->signal_activate().connect(sigc::mem_fun(*this, &AnimeDetailView::update_list_model));
        anime_status->signal_changed().connect(sigc::mem_fun(*this, &AnimeDetailView::update_list_model));
        score->signal_activate().connect(sigc::mem_fun(*this, &AnimeDetailView::update_list_model));

		signal_image_available.connect(sigc::mem_fun(*this, &AnimeDetailView::on_image_available));
	}

	void AnimeDetailView::display_anime(const Anime& in) {
        auto oldid = anime.series_animedb_id;
		anime = in;
		auto title_str = Glib::Markup::escape_text(anime.series_title);
		title_str.insert(0, "<big><big><big>").append("</big></big></big><small><small>");
		std::for_each(std::begin(anime.series_synonyms),
		              std::end(anime.series_synonyms),
		              [&title_str](const std::string& alt){
			              title_str.append("\n")
				                   .append(Glib::Markup::escape_text(alt));
		              });

		title_str.append("</small></small>");
		title->set_markup(title_str);
		episodes->set_text(std::to_string(anime.episodes));
		series_episodes->set_text(std::to_string(anime.series_episodes).insert(0, "/ "));
		anime_status->set_active_text(to_string(anime.status));
        std::stringstream ss;
        ss.precision(3);
        ss << anime.score;
        score->set_text(ss.str());
		show_all();
        if (oldid != anime.series_animedb_id) {
            //image->hide();
            std::thread t(std::bind(&AnimeDetailView::do_fetch_image, this));
            t.detach();
        }
	}

    void AnimeDetailView::on_increment_clicked() {
        try {
            int eps = std::stoi(episodes->get_text());
            ++eps;
            episodes->set_text(std::to_string(eps));
            update_list_model();
        } catch ( std::exception e ) {
        }
    }

    void AnimeDetailView::update_list_model() {
        model_cb(sigc::mem_fun(*this, &AnimeDetailView::on_foreach));
    }

    bool AnimeDetailView::on_foreach(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter) {
        if (iter->get_value(columns->anime).series_animedb_id == anime.series_animedb_id) {
            try {
                if (iter->get_value(columns->episodes) != std::stoi(episodes->get_text())) {
                    iter->set_value(columns->episodes, std::stoi(episodes->get_text()));
                }
                std::stringstream ss1, ss2;
                ss1.precision(3);
                ss2.precision(3);
                ss1 << score->get_text();
                ss2 << iter->get_value(columns->score);
                if (ss1.str() != ss2.str()) {
                    iter->set_value(columns->score, std::stof(ss1.str()));
                }
            } catch (std::exception e) {
            }

            if (anime_status_from_string(iter->get_value(columns->status)) != anime_status->get_anime_status()) {
                iter->set_value(columns->status, Glib::ustring(to_string(anime_status->get_anime_status())));
            }

            return true;
        }
        return false;
    }

	void AnimeDetailView::do_fetch_image() {
		auto str     = mal->get_anime_image_sync(anime);
		image_stream = Gio::MemoryInputStream::create();
		auto buf = g_malloc(str.size());
		std::memcpy(buf, str.c_str(), str.size());
		image_stream->add_data(buf, str.size(), g_free);
		signal_image_available();
	}

	void AnimeDetailView::on_image_available() {
		auto pixbuf = Gdk::Pixbuf::create_from_stream(image_stream);
		image->set(pixbuf);
		image->show();
	}

	AnimeListView::AnimeListView(const std::shared_ptr<MAL>& mal_p,
	                             const AnimeStatus filter,
	                             const bool do_updates_) :
		Gtk::Grid(),
        columns_p(std::make_shared<AnimeModelColumns>()),
		mal(mal_p),
		do_updates(do_updates_),
		status_filter(filter),
		model_changed_functor(sigc::mem_fun(*this, &AnimeListView::on_model_changed)),
		model(Gtk::ListStore::create(*columns_p)),
		treeview(Gtk::manage(new Gtk::TreeView(model)))
	{
		Gtk::ScrolledWindow *sw = Gtk::manage(new Gtk::ScrolledWindow());
		sw->add(*treeview);
		sw->set_hexpand(true);
		sw->set_vexpand(true);
		add(*sw);

		auto title = Gtk::manage(new Gtk::TreeViewColumn("Title", columns_p->series_title));
		title->set_sort_column(columns_p->series_title);
		title->set_expand(true);
		treeview->append_column(*title);
		treeview->append_column("Airing Status", columns_p->series_status);
		auto season = Gtk::manage(new Gtk::TreeViewColumn("Season", columns_p->series_season));
		auto season_cr = season->get_first_cell();
		season_cr->set_alignment(1.0, 0.5);
		treeview->append_column(*season);
		season->set_sort_column(columns_p->series_start_date);

		if (do_updates) {
			treeview->append_column_numeric_editable("Score", columns_p->score, "%.0f");
		} else {
			treeview->append_column_numeric("Score", columns_p->score, "%.2f");
		}

		treeview->append_column("Type", columns_p->type);

		if (do_updates)
			treeview->append_column_numeric_editable("Seen", columns_p->episodes, "%d");

		treeview->append_column("Eps.", columns_p->series_episodes);
		auto crc            = Gtk::manage(new Gtk::TreeViewColumn("Viewing Status"));
		status_cellrenderer = Gtk::manage(new AnimeStatusCellRendererCombo());
		crc->pack_start(*status_cellrenderer);
		crc->add_attribute(status_cellrenderer->property_text(), columns_p->status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &AnimeListView::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;

		treeview->append_column(*crc);
		show_all();
		signal_refreshed.connect(sigc::mem_fun(*this, &AnimeListView::refresh_cb));
		treeview->signal_row_activated().connect(sigc::mem_fun(*this, &AnimeListView::on_my_row_activated));
        treeview->set_rules_hint(true);
#if GTK_CHECK_VERSION(3,8,0)
        Glib::Value<bool> sc_val;
        sc_val.init(Glib::Value<bool>::value_type());
        sc_val.set(true);
        treeview->set_property_value("activate-on-single-click", sc_val);
#endif
	}

	void AnimeListView::on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*) {
		auto iter = model->get_iter(path);
        detailed_anime = iter->get_value(columns_p->anime);
		row_activated_cb(detailed_anime);
        
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
			              iter->set_value(columns_p->series_title, Glib::ustring(anime.series_title));
			              iter->set_value(columns_p->series_status, Glib::ustring(to_string(anime.series_status)));
			              iter->set_value(columns_p->series_season, Glib::ustring(anime_season_from_date(anime.series_date_begin)));
			              auto sort_text = anime.series_date_begin.substr(0,7);
			              iter->set_value(columns_p->series_start_date, Glib::ustring(sort_text));
			              iter->set_value(columns_p->score, static_cast<float>(anime.score));
			              iter->set_value(columns_p->type, Glib::ustring(to_string(anime.series_type)));
			              iter->set_value(columns_p->episodes, static_cast<int>(anime.episodes));
			              iter->set_value(columns_p->series_episodes, static_cast<int>(anime.series_episodes));
			              if (do_updates) {
				              iter->set_value(columns_p->status, Glib::ustring(to_string(anime.status)));
			              } else {
				              iter->set_value(columns_p->status, Glib::ustring("Add To My Anime List..."));
			              }
			              iter->set_value(columns_p->anime, anime);
		              });
		model_changed_connection = model->signal_row_changed().connect(model_changed_functor);
	}


}
