#include "manga_list_view.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <glibmm/markup.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

namespace MAL {

	MangaStatusCellRendererCombo::MangaStatusCellRendererCombo() :
		Gtk::CellRendererCombo(),
		model(Gtk::ListStore::create(columns))
	{
		auto iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(READING)));
		iter->set_value(columns.status, READING);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(MANGACOMPLETED)));
		iter->set_value(columns.status, MANGACOMPLETED);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(MANGAONHOLD)));
		iter->set_value(columns.status, MANGAONHOLD);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(MANGADROPPED)));
		iter->set_value(columns.status, MANGADROPPED);
		iter = model->append();
		iter->set_value(columns.text, Glib::ustring(to_string(PLANTOREAD)));
		iter->set_value(columns.status, PLANTOREAD);
		property_model() = model;
		property_text_column() = columns.text.index();
		property_has_entry() = false;
	}

	MangaStatusComboBox::MangaStatusComboBox() {
        append(to_string(READING));
        append(to_string(MANGACOMPLETED));
        append(to_string(MANGAONHOLD));
        append(to_string(MANGADROPPED));
        append(to_string(PLANTOREAD));
        set_active_text(to_string(READING));
	}

	MangaStatus MangaStatusComboBox::get_manga_status() const {
        return manga_status_from_string(get_active_text());
	}

	void MangaListView::on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter) {
		if (do_updates) {
			bool is_changed = false;
			Manga manga = iter->get_value(columns_p->manga);

			auto const chapters = iter->get_value(columns_p->chapters);
			if (chapters != manga.chapters) {
				is_changed = true;
				manga.chapters = chapters;
				iter->set_value(columns_p->manga, manga);
			}
            auto const volumes = iter->get_value(columns_p->volumes);
            if (volumes != manga.volumes) {
                is_changed = true;
                manga.volumes = volumes;
                iter->set_value(columns_p->manga, manga);
            }
			auto const score = iter->get_value(columns_p->score);
			if (score != manga.score) {
				is_changed = true;
				manga.score = score;
				iter->set_value(columns_p->manga, manga);
			}

			if (is_changed) {
				std::thread t(std::bind(&MangaListView::send_manga_update, this, manga));
				t.detach();
                if (detailed_manga.series_mangadb_id == manga.series_mangadb_id)
                    row_activated_cb(manga);
			}
		}
	}

	void MangaListView::on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text) {
		Gtk::TreeModel::iterator iter = model->get_iter(path);

		if (iter) {
			auto status = manga_status_from_string(new_text);
			Manga manga = iter->get_value(columns_p->manga);
			if (status != manga.status) {
				iter->set_value(columns_p->status, Glib::ustring(to_string(status)));
				manga.status = status;
				iter->set_value(columns_p->manga, manga);
				if (do_updates) {
					std::thread t(std::bind(&MangaListView::send_manga_update, this, manga));
					t.detach();
				} else {
					manga.score = 0.0f;
					if (manga.status == MANGACOMPLETED)
                    {
						manga.chapters = manga.series_chapters;
						manga.volumes = manga.series_volumes;
                    }
                    else
                    {
                        manga.chapters = 0;
                        manga.volumes = 0;
                    }
					std::thread t(std::bind(&MangaListView::send_manga_add, this, manga));
					t.detach();
				}
			}
		}
		
	}

	// NOT Executed on the main thread. Be careful!
	void MangaListView::send_manga_update(Manga manga) {
		auto success = mal->update_manga_sync(manga);
		if (success) {
			auto iter = std::find_if(std::begin(manga_list),
			                         std::end(manga_list),
			                         [&manga](const Manga& m) {
				                         return manga.series_mangadb_id == m.series_mangadb_id;
			                         });
			if (iter != std::end(manga_list)) {
				// Since we're off the main thread, do a threadsafe iter_swap
				std::list<Manga> l(1, manga);
				std::iter_swap(iter, std::begin(l));
			}
		}
	}

	// NOT Executed on the main thread. Be careful!
	void MangaListView::send_manga_add(Manga manga) {
		mal->add_manga_sync(manga);
	}

	MangaListPage::MangaListPage(const std::shared_ptr<MAL>& mal_p) :
		mal(mal_p),
		status_filter(READING),
		list_view(Gtk::manage(new MangaListView(mal_p, status_filter)))
	{
		set_orientation(Gtk::ORIENTATION_VERTICAL);
		auto button = Gtk::manage(new Gtk::Button(""));
		button->set_always_show_image(true);
		auto icon = button->render_icon_pixbuf(Gtk::Stock::REFRESH, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		button->set_image(*image);
		button->set_tooltip_text("Refresh data from mymangalist.net\nThis is "
                                 "rarely necessary. If you enter some invalid"
                                 " data here such as a score of 100, refreshi"
                                 "ng will fix the score to what MAL says, e.g"
                                 ". 10. If you have modified your MAL data vi"
                                 "a the website since starting this program, "
                                 "a refresh will pull in those changes.");
		mal->signal_manga_added.connect(sigc::mem_fun(*this, &MangaListPage::refresh_async));
		
		auto filter_label = Gtk::manage(new Gtk::Label("Filter: "));
		status_combo_box = Gtk::manage(new MangaStatusComboBox());
		detail_view = Gtk::manage(new MangaDetailView(mal));
		detail_view->set_model_cb(sigc::mem_fun(*list_view, &MangaListView::do_model_foreach), list_view->columns_p);
		list_view->set_row_activated_cb(sigc::mem_fun(*detail_view, &MangaDetailView::display_manga));

		status_combo_box->set_hexpand(true);
		button->signal_clicked().connect(sigc::mem_fun(*this, &MangaListPage::refresh_async));
		status_combo_box->signal_changed().connect(sigc::mem_fun(*this, &MangaListPage::on_filter_changed));

		attach(*detail_view, 0, 0, 3, 1);
		attach(*filter_label, 0, 1, 1, 1);
		attach_next_to(*status_combo_box, *filter_label, Gtk::POS_RIGHT, 1, 1);
		attach_next_to(*button, *status_combo_box, Gtk::POS_RIGHT, 1, 1);
		attach_next_to(*list_view, *filter_label, Gtk::POS_BOTTOM, 3, 1);
		show_all();
		detail_view->hide();

		refresh_async();
	}

	void MangaListPage::on_filter_changed() {
		status_filter = status_combo_box->get_manga_status();
		list_view->set_status_filter(status_filter);
	}

	MangaDetailView::MangaDetailView(const std::shared_ptr<MAL>& mal_p) :
		mal(mal_p),
		image(Gtk::manage(new Gtk::Image())),
		title(Gtk::manage(new Gtk::Label())),
		increment_chap_button(Gtk::manage(new Gtk::Button())),
		increment_vol_button(Gtk::manage(new Gtk::Button())),
		chapters(Gtk::manage(new Gtk::Entry())),
		volumes(Gtk::manage(new Gtk::Entry())),
		series_chapters(Gtk::manage(new Gtk::Label())),
		series_volumes(Gtk::manage(new Gtk::Label())),
        score(Gtk::manage(new Gtk::Entry())),
		manga_status(Gtk::manage(new MangaStatusComboBox()))
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
        grid_right->attach(*manga_status, 0, 0, 3, 1);
		grid_right->attach_next_to(*increment_chap_button, *manga_status,     Gtk::POS_BOTTOM, 1, 1);
		grid_right->attach_next_to(*chapters,         *increment_chap_button, Gtk::POS_RIGHT,  1, 1);
		grid_right->attach_next_to(*series_chapters,  *chapters,         Gtk::POS_RIGHT,  1, 1);
		grid_right->attach_next_to(*increment_vol_button, *increment_chap_button,     Gtk::POS_BOTTOM, 1, 1);
		grid_right->attach_next_to(*volumes,         *increment_vol_button, Gtk::POS_RIGHT,  1, 1);
		grid_right->attach_next_to(*series_volumes,  *volumes,         Gtk::POS_RIGHT,  1, 1);
		grid_right->attach_next_to(*score_label,      *increment_vol_button, Gtk::POS_BOTTOM, 1, 1);
		grid_right->attach_next_to(*score,            *score_label,      Gtk::POS_RIGHT,  1, 1);
        title->set_alignment(Gtk::ALIGN_CENTER);
        title->set_hexpand(true);
        title->set_vexpand(false);
        grid_right->set_vexpand(true);

		increment_chap_button->set_always_show_image(true);
		auto icon = increment_chap_button->render_icon_pixbuf(Gtk::Stock::ADD, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		increment_chap_button->set_image(*image);
        increment_chap_button->signal_clicked().connect(sigc::mem_fun(*this, &MangaDetailView::on_increment_chap_clicked));

		increment_vol_button->set_always_show_image(true);
		icon = increment_vol_button->render_icon_pixbuf(Gtk::Stock::ADD, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		image = Gtk::manage(new Gtk::Image(icon));
		increment_vol_button->set_image(*image);
        increment_vol_button->signal_clicked().connect(sigc::mem_fun(*this, &MangaDetailView::on_increment_vol_clicked));

		chapters->set_width_chars(4);
		chapters->set_has_frame(false);
        chapters->set_alignment(Gtk::ALIGN_END);

		volumes->set_width_chars(4);
		volumes->set_has_frame(false);
        volumes->set_alignment(Gtk::ALIGN_END);

        chapters->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailView::update_list_model));
        volumes->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailView::update_list_model));
        manga_status->signal_changed().connect(sigc::mem_fun(*this, &MangaDetailView::update_list_model));
        score->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailView::update_list_model));

		signal_image_available.connect(sigc::mem_fun(*this, &MangaDetailView::on_image_available));
	}

	void MangaDetailView::display_manga(const Manga& in) {
        auto oldid = manga.series_mangadb_id;
		manga = in;
		auto title_str = Glib::Markup::escape_text(manga.series_title);
		title_str.insert(0, "<big><big><big>").append("</big></big></big><small><small>");
		std::for_each(std::begin(manga.series_synonyms),
		              std::end(manga.series_synonyms),
		              [&title_str](const std::string& alt){
			              title_str.append("\n")
                              .append(Glib::Markup::escape_text(alt));
		              });

		title_str.append("</small></small>");
		title->set_markup(title_str);
		chapters->set_text(std::to_string(manga.chapters));
		volumes->set_text(std::to_string(manga.volumes));
		series_chapters->set_text(std::to_string(manga.series_chapters).insert(0, "/ ") + " chapters");
		series_volumes->set_text(std::to_string(manga.series_volumes).insert(0, "/ ") + " volumes");
		manga_status->set_active_text(to_string(manga.status));
        std::stringstream ss;
        ss.precision(3);
        ss << manga.score;
        score->set_text(ss.str());
		show_all();
        if (oldid != manga.series_mangadb_id) {
            std::thread t(std::bind(&MangaDetailView::do_fetch_image, this));
            t.detach();
        }
	}

    void MangaDetailView::on_increment_chap_clicked() {
        try {
            int chaps = std::stoi(chapters->get_text());
            ++chaps;
            chapters->set_text(std::to_string(chaps));
            update_list_model();
        } catch ( std::exception e ) {
        }
    }

    void MangaDetailView::on_increment_vol_clicked() {
        try {
            int vols = std::stoi(volumes->get_text());
            ++vols;
            volumes->set_text(std::to_string(vols));
            update_list_model();
        } catch ( std::exception e ) {
        }
    }

    void MangaDetailView::update_list_model() {
        model_cb(sigc::mem_fun(*this, &MangaDetailView::on_foreach));
    }

    bool MangaDetailView::on_foreach(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter) {
        if (iter->get_value(columns->manga).series_mangadb_id == manga.series_mangadb_id) {
            try {
                if (iter->get_value(columns->chapters) != std::stoi(chapters->get_text())) {
                    iter->set_value(columns->chapters, std::stoi(chapters->get_text()));
                }

                if (iter->get_value(columns->volumes) != std::stoi(volumes->get_text())) {
                    iter->set_value(columns->volumes, std::stoi(volumes->get_text()));
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

            if (manga_status_from_string(iter->get_value(columns->status)) != manga_status->get_manga_status()) {
                iter->set_value(columns->status, Glib::ustring(to_string(manga_status->get_manga_status())));
            }

            return true;
        }
        return false;
    }

	void MangaDetailView::do_fetch_image() {
		auto str     = mal->get_manga_image_sync(manga);
		image_stream = Gio::MemoryInputStream::create();
		auto buf = g_malloc(str.size());
		std::memcpy(buf, str.c_str(), str.size());
		image_stream->add_data(buf, str.size(), g_free);
		signal_image_available();
	}

	void MangaDetailView::on_image_available() {
		auto pixbuf = Gdk::Pixbuf::create_from_stream(image_stream);
		image->set(pixbuf);
		image->show();
	}

	MangaListView::MangaListView(const std::shared_ptr<MAL>& mal_p,
	                             const MangaStatus filter,
	                             const bool do_updates_) :
		Gtk::Grid(),
        columns_p(std::make_shared<MangaModelColumns>()),
		mal(mal_p),
		do_updates(do_updates_),
		status_filter(filter),
		model_changed_functor(sigc::mem_fun(*this, &MangaListView::on_model_changed)),
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
        auto title_cr = static_cast<Gtk::CellRendererText*>(title->get_first_cell());
        title_cr->property_ellipsize().set_value(Pango::ELLIPSIZE_END);
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
			treeview->append_column_numeric_editable("Read", columns_p->chapters, "%d");
		treeview->append_column("Chs", columns_p->series_chapters);

		if (do_updates)
			treeview->append_column_numeric_editable("Read", columns_p->volumes, "%d");
		treeview->append_column("Vols", columns_p->series_volumes);

		auto crc            = Gtk::manage(new Gtk::TreeViewColumn("Reading Status"));
		status_cellrenderer = Gtk::manage(new MangaStatusCellRendererCombo());
		crc->pack_start(*status_cellrenderer);
		crc->add_attribute(status_cellrenderer->property_text(), columns_p->status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &MangaListView::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;

		treeview->append_column(*crc);
		show_all();
		signal_refreshed.connect(sigc::mem_fun(*this, &MangaListView::refresh_cb));
		treeview->signal_row_activated().connect(sigc::mem_fun(*this, &MangaListView::on_my_row_activated));
        treeview->set_rules_hint(true);
#if GTK_CHECK_VERSION(3,8,0)
        Glib::Value<bool> sc_val;
        sc_val.init(Glib::Value<bool>::value_type());
        sc_val.set(true);
        treeview->set_property_value("activate-on-single-click", sc_val);
#endif
	}

	void MangaListView::on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*) {
		auto iter = model->get_iter(path);
        detailed_manga = iter->get_value(columns_p->manga);
		row_activated_cb(detailed_manga);
	}

	void MangaListView::set_status_filter(const MangaStatus status) {
		status_filter = status;
		refresh_cb();
	}

	void MangaListView::set_manga_list(std::list<Manga>& manga) {
		std::swap(manga_list, manga);
		signal_refreshed();
	}

	// Pulls the latest manga list from MAL. Should be done off the
	// main thread.
	void MangaListPage::refresh() {
		auto list = mal->get_manga_list_sync();
		list_view->set_manga_list(list);
	}

	// Asynchronous call to refresh the manga list from MAL
	void MangaListPage::refresh_async() {
		std::thread t(std::bind(&MangaListPage::refresh, this));
		t.detach();
	}

	// Updates the tree model on the main thread.
	void MangaListView::refresh_cb() {
		if (model_changed_connection.connected())
			model_changed_connection.disconnect();
		model->clear();
		auto iter = std::end(manga_list);
		if (status_filter != MANGASTATUS_INVALID) {
			iter = std::partition(std::begin(manga_list),
			                      std::end(manga_list),
			                      [&](const Manga& manga) {
				                      return manga.status == status_filter;
			                      });
		}
		std::vector<Manga> mangas;
		mangas.reserve(std::distance(std::begin(manga_list), iter));
		std::copy(std::begin(manga_list),
		          iter,
		          std::back_inserter(mangas));
			
		std::sort(std::begin(mangas),
		          std::end(mangas),
		          [](const Manga& lhs, const Manga& rhs) {
			          auto season = lhs.series_date_begin.substr(0,7).compare(rhs.series_date_begin.substr(0,7));
			          if (season == 0)
				          return lhs.series_title.compare(rhs.series_title) < 0;
			          else
				          return season > 0;
		          });
		
		
		std::for_each(std::begin(mangas),
		              std::end(mangas),
		              [&](const Manga& manga) {
			              auto iter = model->append();
			              iter->set_value(columns_p->series_title, Glib::ustring(manga.series_title));
			              iter->set_value(columns_p->series_status, Glib::ustring(to_string(manga.series_status)));
			              iter->set_value(columns_p->series_season, Glib::ustring(manga_season_from_date(manga.series_date_begin)));
			              auto sort_text = manga.series_date_begin.substr(0,7);
			              iter->set_value(columns_p->series_start_date, Glib::ustring(sort_text));
			              iter->set_value(columns_p->score, static_cast<float>(manga.score));
			              iter->set_value(columns_p->type, Glib::ustring(to_string(manga.series_type)));
			              iter->set_value(columns_p->chapters, static_cast<int>(manga.chapters));
			              iter->set_value(columns_p->series_chapters, static_cast<int>(manga.series_chapters));
			              iter->set_value(columns_p->volumes, static_cast<int>(manga.volumes));
			              iter->set_value(columns_p->series_volumes, static_cast<int>(manga.series_volumes));
			              if (do_updates) {
				              iter->set_value(columns_p->status, Glib::ustring(to_string(manga.status)));
			              } else {
				              iter->set_value(columns_p->status, Glib::ustring("Add To My Manga List..."));
			              }
			              iter->set_value(columns_p->manga, manga);
		              });
		model_changed_connection = model->signal_row_changed().connect(model_changed_functor);
	}


}
