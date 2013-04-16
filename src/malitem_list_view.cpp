#include "malitem_list_view.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <functional>
#include <glibmm/markup.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

namespace {
    static std::list<std::pair<const int, const Glib::ustring> > initialize_score_combo_map()
    {
        return {
            {0,  "Select Score"},
            {10, "(10) Masterpiece"},
            {9,  "(9) Great"},
            {8,  "(8) Very Good"},
            {7,  "(7) Good"},
            {6,  "(6) Fine"},
            {5,  "(5) Average"},
            {4,  "(4) Bad "},
            {3,  "(3) Very Bad"},
            {2,  "(2) Horrible"},
            {1,  "(1) Appalling"},
        };
    }
}

namespace sigc {
    SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
}

namespace MAL {

    ScoreComboBox::ScoreComboBox() :
        Gtk::ComboBox(false),
        m_columns(),
        m_model(Gtk::ListStore::create(m_columns)),
        m_score_list(initialize_score_combo_map())
    {
        std::for_each(std::begin(m_score_list), std::end(m_score_list),
                      std::bind(&ScoreComboBox::append_model,
                                this,
                                std::placeholders::_1));
        set_model(m_model);
        pack_start(m_columns.text);
    }

    void ScoreComboBox::append_model(const std::pair<const int, const Glib::ustring>& pair)
    {
        auto iter = m_model->append();
        iter->set_value(m_columns.score, pair.first);
        iter->set_value(m_columns.text, pair.second);
    }

    int ScoreComboBox::get_score() const
    {
        auto iter = get_active();
        return iter->get_value(m_columns.score);
    }

    void ScoreComboBox::set_score(const int score)
    {
        m_model->foreach_iter([&](const Gtk::TreeModel::iterator& iter)->bool {
                if (iter->get_value(m_columns.score) == score) {
                    set_active(iter);
                    return true;
                }
                return false;
            });
    }

    MALItemListViewNotifier::MALItemListViewNotifier(const std::shared_ptr<MALItemModelColumns> &columns,
                                                     const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> &cb) :
        m_notify_columns(columns),
        list_model_foreach(sigc::bind<-1>(cb, sigc::mem_fun(*this, &MALItemListViewNotifier::list_model_foreach_cb)))
    {
    }

    void MALItemListViewNotifier::notify_list_model()
    {
        list_model_foreach();
    }

    bool MALItemListViewNotifier::list_model_foreach_cb(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator &iter)
    {
        return update_list_model(*iter);
    }

	MALItemDetailViewBase::MALItemDetailViewBase(const std::shared_ptr<MAL>& mal) :
		m_mal  (mal),
		m_image(Gtk::manage(new Gtk::Image())),
		m_title(Gtk::manage(new Gtk::Label())),
        m_grid (Gtk::manage(new Gtk::Grid()))
	{
		set_vexpand(false);
        set_column_spacing(5);
        m_grid->set_row_spacing(5);
		//               left, top, width, height
		attach(*m_image,      0,   0,     1,      2);
		attach(*m_title,      1,   0,     2,      1);
        attach(*m_grid,       1,   1,     1,      1);

        m_title->set_alignment(Gtk::ALIGN_CENTER);
        m_title->set_hexpand(true);
        m_title->set_vexpand(false);
        m_grid->set_vexpand(true);

		m_signal_image_available.connect(sigc::mem_fun(*this, &MALItemDetailViewBase::on_image_available));
	}

	void MALItemDetailViewBase::display_item(const std::shared_ptr<const MALItem>& item) {
        decltype(item->series_itemdb_id) oldid = 0;
        if (m_item)
            oldid = m_item->series_itemdb_id;
        m_item = item;

		auto title_str = Glib::Markup::escape_text(item->series_title);
		title_str.insert(0, "<big><big><big>").append("</big></big></big><small><small>");
		std::for_each(std::begin(item->series_synonyms),
		              std::end(item->series_synonyms),
		              [&title_str](const std::string& alt){
			              title_str.append("\n")
                              .append(Glib::Markup::escape_text(alt));
		              });

		title_str.append("</small></small>");
		m_title->set_markup(title_str);
        m_title->set_ellipsize(Pango::ELLIPSIZE_END);
		show_all();

        if (oldid == 0 || oldid != m_item->series_itemdb_id) {
            std::thread t(std::bind(&MALItemDetailViewBase::do_fetch_image, this));
            t.detach();
        }
	}

	void MALItemDetailViewBase::do_fetch_image() {
		auto str     = m_mal->get_image_sync(*m_item);
		image_stream = Gio::MemoryInputStream::create();
		auto buf = g_malloc(str.size());
		std::memcpy(buf, str.c_str(), str.size());
		image_stream->add_data(buf, str.size(), g_free);
		m_signal_image_available();
	}

	void MALItemDetailViewBase::on_image_available() {
		auto pixbuf = Gdk::Pixbuf::create_from_stream(image_stream);
		m_image->set(pixbuf);
		m_image->show();
	}

    MALItemDetailViewStatic::MALItemDetailViewStatic(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        m_score(Gtk::manage(new Gtk::Label()))
    {
        m_grid->attach(*m_score, 0, 0, 1, 1);
    }

    void MALItemDetailViewStatic::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);
        std::stringstream ss;
        ss << "Average Score: ";
        ss.precision(3);
        ss << item->score;
        m_score->set_text(ss.str());
        m_score->show();
    }

    MALItemDetailViewEditable::MALItemDetailViewEditable(const std::shared_ptr<MAL> &mal,
                                                         const std::shared_ptr<MALItemModelColumnsEditable> &columns,
                                                         const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> &slot) :
        MALItemDetailViewBase(mal),
        MALItemListViewNotifier(columns, slot),
        m_columns(columns),
        m_score_grid(Gtk::manage(new Gtk::Grid())),
        m_score(Gtk::manage(new ScoreComboBox()))
    {
        auto label = Gtk::manage(new Gtk::Label("Score: "));
        m_score_grid->attach(*label, 0, 0, 1, 1);
        m_score_grid->attach(*m_score, 1, 0, 1, 1);
        m_grid->attach(*m_score_grid, 0, 0, 1, 1);
        m_score->signal_changed().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
    }

    void MALItemDetailViewEditable::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);

        auto score = static_cast<int>(item->score);
        m_score->set_score(score);
        m_score_grid->show_all();
    }

    bool MALItemDetailViewEditable::update_list_model(const Gtk::TreeRow &row)
    {
        if (row.get_value(m_columns->item)->series_itemdb_id != m_item->series_itemdb_id)
            return false;

        int score;
        score = m_score->get_score();

        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_notify_columns);
        if (score != row.get_value(columns->score))
            row.set_value(columns->score, score);
        return true;
    }
    
	MALItemListViewBase::MALItemListViewBase(const std::shared_ptr<MAL>& mal,
                                             const std::shared_ptr<MALItemModelColumns>& columns) :
		Gtk::Grid (),
        m_columns (columns),
		m_mal     (mal),
		m_model   (Gtk::ListStore::create(*m_columns)),
		m_treeview(Gtk::manage(new Gtk::TreeView(m_model)))
	{
		Gtk::ScrolledWindow *sw = Gtk::manage(new Gtk::ScrolledWindow());
		sw->add(*m_treeview);
		sw->set_hexpand(true);
		sw->set_vexpand(true);
		add(*sw);

		m_title_column = Gtk::manage(new Gtk::TreeViewColumn("Title", m_columns->series_title));
		m_title_column->set_sort_column(m_columns->series_title);
		m_title_column->set_expand(true);
        auto title_cr = static_cast<Gtk::CellRendererText*>(m_title_column->get_first_cell());
        title_cr->property_ellipsize().set_value(Pango::ELLIPSIZE_END);
		m_treeview->append_column(*m_title_column);

		auto season = Gtk::manage(new Gtk::TreeViewColumn("Season", m_columns->series_season));
		auto season_cr = season->get_first_cell();
		season_cr->set_alignment(1.0, 0.5);
		m_treeview->append_column(*season);
		season->set_sort_column(m_columns->series_start_date);

		show_all();
		m_signal_refreshed.connect(sigc::mem_fun(*this, &MALItemListViewBase::refresh_cb));
		m_treeview->signal_row_activated().connect(sigc::mem_fun(*this, &MALItemListViewBase::on_my_row_activated));
        m_treeview->set_rules_hint(true);
#if GTK_CHECK_VERSION(3,8,0)
        Glib::Value<bool> sc_val;
        sc_val.init(Glib::Value<bool>::value_type());
        sc_val.set(true);
        m_treeview->set_property_value("activate-on-single-click", sc_val);
#endif
	}

	void MALItemListViewBase::set_item_list(std::list<std::shared_ptr<const MALItem> >&& items) {
		m_items = std::move(items);
		m_signal_refreshed();
	}

    void MALItemListViewBase::set_filter_func(const sigc::slot<bool, const std::shared_ptr<const MALItem>&>& slot)
    {
        m_filter_func = slot;
    }

    void MALItemListViewBase::refilter() {
        m_signal_refreshed();
    }

    void MALItemListViewBase::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) {
        row.set_value(m_columns->series_title, Glib::ustring(item->series_title));
        row.set_value(m_columns->series_season, Glib::ustring(item->get_season_began()));
        auto sort_text = item->series_date_begin.substr(0,7);
        row.set_value(m_columns->series_start_date, Glib::ustring(sort_text));
        row.set_value(m_columns->item, item);
    }

	// Updates the tree model on the main thread.
	void MALItemListViewBase::refresh_cb() {
        m_model_changed_connection.block();
		m_model->clear();

		std::vector<std::shared_ptr<const MALItem> > items;
		items.reserve(m_items.size());
		std::copy(std::begin(m_items),
                  std::end(m_items),
		          std::back_inserter(items));
			
		std::sort(std::begin(items),
		          std::end(items),
		          [](const std::shared_ptr<const MALItem>& lhs, const std::shared_ptr<const MALItem>& rhs) {
			          auto season = lhs->series_date_begin.substr(0,7).compare(rhs->series_date_begin.substr(0,7));
			          if (season == 0)
				          return lhs->series_title.compare(rhs->series_title) < 0;
			          else
				          return season > 0;
		          });

		std::for_each(std::begin(items),
		              std::end(items),
		              [&](const std::shared_ptr<const MALItem>& item) {
                          if (m_filter_func) {
                              if (m_filter_func(item)) {
                                  auto iter = m_model->append();
                                  refresh_item_cb(item, *iter);
                              }
                          } else {
                              auto iter = m_model->append();
                              refresh_item_cb(item, *iter);
                          }
		              });
        m_model_changed_connection.unblock();
	}

	void MALItemListViewBase::on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*)
    {
		auto iter = m_model->get_iter(path);
        m_detailed_item = iter->get_value(m_columns->item);
		if (m_row_activated_cb) m_row_activated_cb(m_detailed_item);
	}

    MALItemListViewStatic::MALItemListViewStatic(const std::shared_ptr<MAL>& mal,
                                                 const std::shared_ptr<MALItemModelColumnsStatic>& columns) :
        MALItemListViewBase(mal, columns)
    {
		int num = m_treeview->append_column_numeric("Score", columns->score, "%.2f");
        m_score_column = m_treeview->get_column(num - 1);
    }

    void MALItemListViewStatic::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsStatic>(m_columns);
        row.set_value(columns->score, item->score);
    }

    MALItemListViewEditable::MALItemListViewEditable(const std::shared_ptr<MAL>& mal,
                                                     const std::shared_ptr<MALItemModelColumnsEditable>& columns) :
        MALItemListViewBase(mal, columns)
    {
		int num = m_treeview->append_column_numeric_editable("Score", columns->score, "%d");
        m_score_column = m_treeview->get_column(num - 1);
		m_model_changed_connection = m_model->signal_row_changed().connect(sigc::mem_fun(*this, &MALItemListViewEditable::on_model_changed));
    }

    void MALItemListViewEditable::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_columns);
        row.set_value(columns->score, static_cast<int>(item->score));
    }

    void MALItemListViewEditable::on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter)
    {
        auto original_item = iter->get_value(m_columns->item);
        auto item = original_item;
        bool is_changed = false;
        is_changed = model_changed_cb(item, *iter); // May change item!

        if (is_changed) {
            if (!m_detailed_item || m_detailed_item->series_itemdb_id == item->series_itemdb_id)
                m_row_activated_cb(item);
            send_item_update(item);
        }
    }

    bool MALItemListViewEditable::model_changed_cb(std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        bool is_changed = false;
        auto const columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_columns);

        auto const score = row.get_value(columns->score);
        if (score != static_cast<int>(item->score)) {
            is_changed = true;
            auto new_item = item->clone();
            new_item->score = score;
            item = new_item;
            row.set_value(columns->item, item);
        }

        return is_changed;
    }

    void MALItemListViewEditable::do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter& slot)
    {
        m_model->foreach(slot);
    }

	MALItemListPage::MALItemListPage(const std::shared_ptr<MAL>& mal,
                                     MALItemListViewBase* list_view,
                                     MALItemDetailViewBase* detail_view) :
		m_mal(mal),
		m_list_view(list_view),
        m_detail_view(detail_view),
        m_button_row(Gtk::manage(new Gtk::Grid())),
        m_refresh_button(Gtk::manage(new Gtk::Button("")))
	{
		set_orientation(Gtk::ORIENTATION_VERTICAL);
		m_refresh_button->set_always_show_image(true);
		auto icon = m_refresh_button->render_icon_pixbuf(Gtk::Stock::REFRESH, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		m_refresh_button->set_image(*image);
		m_refresh_button->set_tooltip_text("Refresh data from myanimelist.net\nThis is "
                                           "rarely necessary. If you enter some invalid"
                                           " data here such as a score of 100, refreshi"
                                           "ng will fix the score to what MAL says, e.g"
                                           ". 10. If you have modified your MAL data vi"
                                           "a the website since starting this program, "
                                           "a refresh will pull in those changes.");

        //if (m_detail_view)
        //detail_view->set_model_cb(sigc::mem_fun(*m_list_view, &MALItemListViewBase::do_model_foreach), columns);
        if (m_detail_view)
            list_view->set_row_activated_cb(sigc::mem_fun(*m_detail_view, &MALItemDetailViewBase::display_item));

		auto action = Gtk::Action::create();
		action->signal_activate().connect(sigc::mem_fun(*this, &MALItemListPage::refresh_async));
		m_refresh_button->set_related_action(action);
        m_refresh_button->set_can_default(true);
        m_refresh_button->set_receives_default(true);

		if (m_detail_view) attach(*m_detail_view, 0, 0, 3, 1);
        attach(*m_button_row, 0, 1, 1, 1);
		m_button_row->attach(*m_refresh_button, 0, 0, 1, 1);
		attach(*list_view, 0, 2, 1, 1);

		show_all();
		if (m_detail_view) m_detail_view->hide();
	}

	// Asynchronous call to refresh the anime list from MAL
	void MALItemListPage::refresh_async() {
		std::thread t(std::bind(&MALItemListPage::refresh, this));
		t.detach();
	}
}
