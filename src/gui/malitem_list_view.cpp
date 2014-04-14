/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "malitem_list_view.hpp"
#include <array>
#include <iostream>
#include <cstring>
#include <functional>
#include <glibmm/markup.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>

namespace sigc {
    SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
}

namespace MAL {

    MALItemPriorityComboBox::MALItemPriorityComboBox()
    {
        append(to_string(PRIORITY_LOW));
        append(to_string(PRIORITY_MEDIUM));
        append(to_string(PRIORITY_HIGH));
        set_active_text(to_string(PRIORITY_LOW));
    }

    Priority MALItemPriorityComboBox::get_priority() const
    {
        return priority_from_string(get_active_text());
    }

    MALItemReconsumeValueComboBox::MALItemReconsumeValueComboBox() :
        invalid_text("Select...")
    {
        append(invalid_text);
        append(to_string(RECONSUME_VALUE_VERYLOW));
        append(to_string(RECONSUME_VALUE_LOW));
        append(to_string(RECONSUME_VALUE_MEDIUM));
        append(to_string(RECONSUME_VALUE_HIGH));
        append(to_string(RECONSUME_VALUE_VERYHIGH));
    }

    ReconsumeValue MALItemReconsumeValueComboBox::get_reconsume_value() const
    {
        auto const text = get_active_text();
        if (text == invalid_text)
            return RECONSUME_VALUE_INVALID;
        else
            return reconsume_value_from_string(text);
    }

    void MALItemReconsumeValueComboBox::set_reconsume_value(const ReconsumeValue value)
    {
        if (value == RECONSUME_VALUE_INVALID)
            set_active_text(invalid_text);
        else
            set_active_text(to_string(value));
    }

    ScoreComboBox::ScoreComboBox() :
        Gtk::ComboBox(false),
        m_columns(),
        m_model(Gtk::ListStore::create(m_columns))
    {
        const std::array< const std::pair<const int, const Glib::ustring>, 11> score_list = {{
                {0,  "Select Score"},
                {10, "(10) Masterpiece"},
                {9,  "(9) Great"},
                {8,  "(8) Very Good"},
                {7,  "(7) Good"},
                {6,  "(6) Fine"},
                {5,  "(5) Average"},
                {4,  "(4) Bad"},
                {3,  "(3) Very Bad"},
                {2,  "(2) Horrible"},
                {1,  "(1) Appalling"},
            }};

        std::for_each(std::begin(score_list), std::end(score_list),
                      [&](const std::pair<const int, const Glib::ustring>& pair) {
                          auto iter = m_model->append();
                          iter->set_value(m_columns.score, pair.first);
                          iter->set_value(m_columns.text, pair.second);
                      });
        set_model(m_model);
        pack_start(m_columns.text);
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
		m_mal                     (mal),
		m_image                   (Gtk::manage(new Gtk::Image())),
		m_title                   (Gtk::manage(new Gtk::Label())),
        m_grid                    (Gtk::manage(new Gtk::Grid())),
        m_grid_left               (Gtk::manage(new Gtk::Grid())),
        m_grid_center             (Gtk::manage(new Gtk::Grid())),
        m_grid_right              (Gtk::manage(new Gtk::Grid())),
        m_alt_title_grid          (Gtk::manage(new Gtk::Grid())),
        m_synopsis_frame          (Gtk::manage(new Gtk::Frame("Synopsis"))),
        m_synopsis_label          (Gtk::manage(new Gtk::Label())),
        m_series_date_grid        (Gtk::manage(new Gtk::Grid())),
        m_series_start_date_label (Gtk::manage(new DateLabel(""))),
        m_series_end_date_label   (Gtk::manage(new DateLabel(""))),
        m_series_date_sizegroup   (Gtk::SizeGroup::create(Gtk::SIZE_GROUP_BOTH))
	{
		set_vexpand(false);
        set_column_spacing(5);
        m_grid->set_row_spacing(5);
        m_grid->set_column_spacing(5);
        m_grid_left->set_row_spacing(5);
        m_grid_center->set_row_spacing(5);
        m_grid_right->set_row_spacing(5);
		//                       left, top, width, height
		attach(*m_image,            0,   0,     1,      4);
		attach(*m_title,            1,   0,     1,      1);
		attach(*m_alt_title_grid,   1,   1,     1,      1);
        attach(*m_grid,             1,   2,     1,      1);
        m_grid->attach(*m_grid_left,   0, 0, 1, 1);
        m_grid->attach(*m_grid_center, 1, 0, 1, 1);
        m_grid->attach(*m_grid_right,  2, 0, 1, 1);
        m_grid_right->attach(*m_synopsis_frame, 0, 0, 1, 3);
        m_title->set_alignment(Gtk::ALIGN_CENTER);
        m_title->set_hexpand(true);
        m_title->set_vexpand(false);
        m_alt_title_grid->set_hexpand(true);
        m_alt_title_grid->set_vexpand(false);
        //m_alt_title_grid->set_column_homogeneous(true);
        m_alt_title_grid->set_column_spacing(15);
        m_alt_title_grid->set_halign(Gtk::ALIGN_CENTER);
        m_alt_title_grid->set_column_homogeneous(true);
        //m_alt_title_grid->set_alignment(Gtk::ALIGN_CENTER);
        m_grid->set_vexpand(true);
        m_grid_left->set_vexpand(true);
        m_grid_center->set_vexpand(true);
        m_grid_right->set_vexpand(true);
        //m_alt_title_grid->set_hfill(true);
        auto sw = Gtk::manage(new Gtk::ScrolledWindow());
        sw->add(*m_synopsis_label);
        sw->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
        m_synopsis_frame->add(*sw);
        m_synopsis_frame->set_shadow_type(Gtk::SHADOW_IN);
        m_synopsis_frame->set_margin_left(10);
        m_synopsis_frame->set_margin_right(10);
        m_synopsis_frame->set_margin_bottom(10);
        m_synopsis_frame->set_vexpand(true);
        m_synopsis_label->set_justify(Gtk::JUSTIFY_FILL);
        m_synopsis_label->set_line_wrap(true);
        m_synopsis_label->set_line_wrap_mode(Pango::WRAP_WORD_CHAR);
        m_synopsis_label->set_padding(5,5);
        m_synopsis_label->set_line_wrap(true);
        m_synopsis_label->set_line_wrap(true);
        m_synopsis_label->set_valign(Gtk::ALIGN_START);
		m_signal_image_available.connect(sigc::mem_fun(*this, &MALItemDetailViewBase::on_image_available));
        m_grid_left->attach(*m_series_date_grid, 0, 1, 1, 1);
        
        m_series_date_grid->attach(*m_series_start_date_label, 0, 0, 1, 1);
        auto label = Gtk::manage(new Gtk::Label("–"));
        m_series_date_grid->attach(*label, 1, 0, 1, 1);
        m_series_date_grid->attach(*m_series_end_date_label,   2, 0, 1, 1);
        m_series_date_grid->set_column_spacing(5);
        m_series_date_grid->set_vexpand(true);
        m_series_start_date_label->set_vexpand(true);
        m_series_end_date_label->set_vexpand(true);
        label->set_vexpand(true);
        m_series_date_grid->set_halign(Gtk::ALIGN_CENTER);
        m_series_date_sizegroup->add_widget(*m_series_start_date_label);
        m_series_date_sizegroup->add_widget(*m_series_end_date_label);
        m_series_date_sizegroup->set_ignore_hidden(false);
    }

    namespace {
        static Gtk::Label* make_alt_label(const std::string& alt_title) {
            auto label = Gtk::manage(new Gtk::Label());
            auto str = Glib::Markup::escape_text(alt_title);
            str.insert(0, "<small><small>").append("</small></small>");
            label->set_markup(str);
            label->set_ellipsize(Pango::ELLIPSIZE_END);
            label->set_alignment(Gtk::ALIGN_CENTER);
            label->set_hexpand(true);
            return label;
        }

        static void add_alt_titles(Gtk::Grid *grid, const std::shared_ptr<MALItem>& item) {
            grid->foreach(sigc::mem_fun(grid, &Gtk::Grid::remove));

            const int rows = item->series_synonyms.size() / 3 + 1;
            const int top_row_elems = item->series_synonyms.size() % 3;
            std::vector<std::pair<int, int>> coords;
            coords.reserve(item->series_synonyms.size());
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < 3; ++j) {
                    coords.push_back(std::make_pair(j, i));
                }
            }

            auto alt_title_iter = std::begin(item->series_synonyms);

            auto top_row = Gtk::manage(new Gtk::Grid());
            top_row->set_hexpand(true);
            top_row->set_halign(Gtk::ALIGN_CENTER);
            top_row->set_column_spacing(15);
            top_row->set_column_homogeneous(true);
            for (int i = 0; i < top_row_elems; ++i, ++alt_title_iter) {
                auto label = make_alt_label(*alt_title_iter);
                top_row->attach(*label, i, 0, 1, 1);
            }
            grid->attach(*top_row, 0, -1, 3, 1);

            auto coord_iter = std::begin(coords);
            std::for_each(alt_title_iter,
                          std::end(item->series_synonyms),
                          [&](const std::string& alt_title) {
                              auto label = make_alt_label(alt_title);
                              grid->attach(*label,
                                           coord_iter->first,
                                           coord_iter->second,
                                           1, 1);
                              ++coord_iter;
                          });
        }
    }

	void MALItemDetailViewBase::display_item(const std::shared_ptr<MALItem>& item) {
        decltype(item->series_itemdb_id) oldid = 0;
        if (m_item)
            oldid = m_item->series_itemdb_id;
        m_item = item;

		auto title_str = Glib::Markup::escape_text(item->series_title);
		title_str.insert(0, "<big><big><big>").append("</big></big></big>");
		m_title->set_markup(title_str);
        m_title->set_ellipsize(Pango::ELLIPSIZE_END);

        add_alt_titles(m_alt_title_grid, item);
        m_synopsis_label->set_markup(Glib::Markup::escape_text(item->series_synopsis));
		show_all();
        if (item->series_synopsis.size() == 0) m_synopsis_frame->hide();

        if (oldid == 0 || oldid != m_item->series_itemdb_id) {
            do_fetch_image();
        }

        if (!m_series_start_date_label->set_date(item->series_date_begin)) {
            m_series_start_date_label->hide();
            m_series_date_grid->get_child_at(1, 0)->hide();
        } else {
            m_series_start_date_label->show();
            m_series_date_grid->get_child_at(1, 0)->show();
        }

        if (!m_series_end_date_label->set_date(item->series_date_end) 
            || item->series_date_end == item->series_date_begin) {
            m_series_end_date_label->hide();
            m_series_date_grid->get_child_at(1, 0)->hide();
        } else {
            m_series_end_date_label->show();
        }
	}

	void MALItemDetailViewBase::do_fetch_image() {
		m_mal->get_image_async(*m_item, [this](const std::string &str) {
                image_stream = Gio::MemoryInputStream::create();
                auto buf = g_malloc(str.size());
                std::memcpy(buf, str.c_str(), str.size());
                image_stream->add_data(buf, str.size(), g_free);
                on_image_available();
            });
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
        m_grid_left->attach_next_to(*m_score, *m_series_date_grid,
                               Gtk::POS_BOTTOM, 1, 1);
        m_score->set_alignment(Gtk::ALIGN_START);
    }

    void MALItemDetailViewStatic::display_item(const std::shared_ptr<MALItem>& item)
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
        m_score(Gtk::manage(new ScoreComboBox())),
        m_date_begin_label(Gtk::manage(new Gtk::Label("Began:", Gtk::ALIGN_START))),
        m_date_begin_entry(Gtk::manage(new DateEntry())),
        m_date_end_label(Gtk::manage(new Gtk::Label("Finished:", Gtk::ALIGN_START))),
        m_date_end_entry(Gtk::manage(new DateEntry())),
        m_reconsuming_label(Gtk::manage(new Gtk::Label("Rewatching:", Gtk::ALIGN_START))),
        m_reconsuming_switch(Gtk::manage(new Gtk::Switch())),
        m_downloaded_items_entry(Gtk::manage(new IncrementEntry("Downloaded Episodes"))),
        m_times_consumed_entry(Gtk::manage(new IncrementEntry("Times Watched"))),
        m_fansub_group_label(Gtk::manage(new Gtk::Label("Fansub Group:"))),
        m_fansub_group_entry(Gtk::manage(new Gtk::Entry())),
        m_priority_label(Gtk::manage(new Gtk::Label("Priority:"))),
        m_reconsume_value_label(Gtk::manage(new Gtk::Label("Rewatch Value:"))),
        m_priority_combo(Gtk::manage(new MALItemPriorityComboBox())),
        m_reconsume_value_combo(Gtk::manage(new MALItemReconsumeValueComboBox()))
    {
        auto label = Gtk::manage(new Gtk::Label("Score: "));
        m_score_grid->attach(*label, 0, 0, 1, 1);
        m_score_grid->attach(*m_score, 1, 0, 1, 1);
        m_grid_left->attach_next_to(*m_score_grid, *m_series_date_grid,
                                    Gtk::POS_BOTTOM, 1, 1);
        m_score_changed_connection = m_score->signal_changed().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
        auto grid = Gtk::manage(new Gtk::Grid());
        grid->attach(*m_date_begin_label, 0, 0, 1, 1);
        grid->attach(*m_date_begin_entry, 1, 0, 1, 1);
        grid->attach(*m_date_end_label, 0, 1, 1, 1);
        grid->attach(*m_date_end_entry, 1, 1, 1, 1);
        grid->set_column_spacing(5);
        grid->set_row_spacing(5);
        m_grid_left->insert_next_to(*m_score_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*grid, *m_score_grid, Gtk::POS_BOTTOM, 1, 2);

        m_date_begin_entry->signal_activate().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
        m_date_end_entry->signal_activate().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
        
        auto switchgrid = Gtk::manage(new Gtk::Grid());
        switchgrid->attach(*m_reconsuming_label, 0, 0, 1, 1);
        switchgrid->attach(*m_reconsuming_switch, 1, 0, 1, 1);
        switchgrid->set_column_spacing(5);
        m_grid_left->insert_next_to(*grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*switchgrid, *grid, Gtk::POS_BOTTOM, 1, 1);
        m_reconsuming_changed_connection = m_reconsuming_switch->property_active().signal_changed().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));

        auto fansubgrid = Gtk::manage(new Gtk::Grid());
        m_downloaded_items_entry->signal_activate().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
        m_times_consumed_entry->signal_activate().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
        m_grid_center->attach(*m_downloaded_items_entry, 0, 0, 1, 1);
        m_grid_center->attach(*m_times_consumed_entry, 0, 1, 1, 1);
        m_grid_center->attach(*fansubgrid, 0, 2, 1, 1);
        fansubgrid->set_column_spacing(5);
        fansubgrid->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
        fansubgrid->add(*m_fansub_group_label);
        fansubgrid->add(*m_fansub_group_entry);
        fansubgrid->set_column_spacing(5);
        m_fansub_group_entry->signal_activate().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
        m_grid_left->set_row_homogeneous(true);
        //m_grid_center->set_row_homogeneous(true);

        auto prioritygrid = Gtk::manage(new Gtk::Grid());
        prioritygrid->attach(*m_priority_label, 0, 0, 1, 1);
        prioritygrid->attach(*m_priority_combo, 1, 0, 1, 1);
        prioritygrid->set_column_spacing(5);
        m_grid_center->attach(*prioritygrid, 0, 3, 1, 1);
        m_priority_changed_connection = m_priority_combo->signal_changed().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));

        auto reconsume_valuegrid = Gtk::manage(new Gtk::Grid());
        reconsume_valuegrid->attach(*m_reconsume_value_label, 0, 0, 1, 1);
        reconsume_valuegrid->attach(*m_reconsume_value_combo, 1, 0, 1, 1);
        reconsume_valuegrid->set_column_spacing(5);
        m_grid_center->attach(*reconsume_valuegrid, 0, 4, 1, 1);
        m_reconsume_value_changed_connection = m_reconsume_value_combo->signal_changed().connect(sigc::mem_fun(*this, &MALItemDetailViewEditable::notify_list_model));
    }

    void MALItemDetailViewEditable::display_item(const std::shared_ptr<MALItem>& item)
    {
        m_score_changed_connection.block();
        m_reconsuming_changed_connection.block();
        m_priority_changed_connection.block();
        m_reconsume_value_changed_connection.block();

        auto const olditem = m_item;
        MALItemDetailViewBase::display_item(item);
        
        if (!olditem || item->series_itemdb_id != olditem->series_itemdb_id) {
            auto score = static_cast<int>(item->score);
            m_score->set_score(score);
            m_score->show_all();

            auto begin = item->get_date(item->date_start);
            if (begin.valid()) {
                m_date_begin_entry->set_date(begin);
            } else {
                m_date_begin_entry->set_text("");
            }

            auto end = item->get_date(item->date_finish);
            if (end.valid()) {
                m_date_end_entry->set_date(end);
            } else {
                m_date_end_entry->set_text("");
            }

            m_reconsuming_switch->set_active(item->enable_reconsuming);
        }

        if (item->has_details) {
            m_fansub_group_entry->set_sensitive(true);
            m_downloaded_items_entry->set_sensitive(true);
            m_times_consumed_entry->set_sensitive(true);
            m_priority_combo->set_sensitive(true);
            m_reconsume_value_combo->set_sensitive(true);
            
            m_fansub_group_entry->set_text(m_item->fansub_group);
            m_downloaded_items_entry->set_entry_text(std::to_string(item->downloaded_items));
            m_times_consumed_entry->set_entry_text(std::to_string(item->times_consumed));
            m_priority_combo->set_active_text(to_string(item->priority));
            m_reconsume_value_combo->set_reconsume_value(item->reconsume_value);
        } else {
            m_fansub_group_entry->set_sensitive(false);
            m_downloaded_items_entry->set_sensitive(false);
            m_times_consumed_entry->set_sensitive(false);
            m_priority_combo->set_sensitive(false);
            m_reconsume_value_combo->set_sensitive(false);

            m_fansub_group_entry->set_text(Glib::ustring());
            m_downloaded_items_entry->set_entry_text(Glib::ustring());
            m_times_consumed_entry->set_entry_text(Glib::ustring());
            m_priority_combo->set_active_text("None");
            m_reconsume_value_combo->set_active_text("None");
        }

        m_score_changed_connection.unblock();
        m_reconsuming_changed_connection.unblock();
        m_priority_changed_connection.unblock();
        m_reconsume_value_changed_connection.unblock();
    }

    bool MALItemDetailViewEditable::update_list_model(const Gtk::TreeRow &row)
    {
        if (row.get_value(m_columns->item)->series_itemdb_id != m_item->series_itemdb_id)
            return false;

        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_notify_columns);

        auto score = m_score->get_score();
        if (score != row.get_value(columns->score))
            row.set_value(columns->score, score);

        auto begin = m_date_begin_entry->get_date();
        if (begin.valid()) {
            auto str = begin.format_string("%F");
            if (str != row.get_value(columns->begin_date))
                row.set_value(columns->begin_date, str);
        }

        auto end = m_date_end_entry->get_date();
        if (end.valid()) {
            auto str = end.format_string("%F");
            if (str != row.get_value(columns->end_date))
                row.set_value(columns->end_date, str);
        }

        auto reconsuming = m_reconsuming_switch->get_active();
        if (reconsuming != row.get_value(columns->enable_reconsuming))
            row.set_value(columns->enable_reconsuming, reconsuming);

        if (m_item->has_details) {
            auto fansubgroup = m_fansub_group_entry->get_text();
            if (fansubgroup != row.get_value(columns->fansub_group))
                row.set_value(columns->fansub_group, fansubgroup);

            auto downloaded_items = std::stoi(m_downloaded_items_entry->get_entry_text());
            if (downloaded_items != row.get_value(columns->downloaded_items))
                row.set_value(columns->downloaded_items, downloaded_items);

            auto times_consumed = std::stoi(m_times_consumed_entry->get_entry_text());
            if (times_consumed != row.get_value(columns->times_consumed))
                row.set_value(columns->times_consumed, times_consumed);

            auto priority = priority_from_string(m_priority_combo->get_active_text());
            if (priority != row.get_value(columns->priority))
                row.set_value(columns->priority, priority);

            auto reconsume_value = m_reconsume_value_combo->get_reconsume_value();
            if (reconsume_value != row.get_value(columns->reconsume_value))
                row.set_value(columns->reconsume_value, reconsume_value);
        }

        return true;
    }

	MALItemListViewBase::MALItemListViewBase(const std::shared_ptr<MAL>& mal,
                                             const std::shared_ptr<MALItemModelColumns>& columns) :
		Gtk::Grid    (),
        m_columns    (columns),
		m_mal        (mal),
		m_root_model (Gtk::ListStore::create(*m_columns)),
        m_model      (Gtk::TreeModelFilter::create(m_root_model)),
		m_treeview   (Gtk::manage(new Gtk::TreeView(m_model)))
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
		m_treeview->signal_row_activated().connect(sigc::mem_fun(*this, &MALItemListViewBase::on_my_row_activated));
        m_treeview->set_rules_hint(true);
        m_root_model->set_default_sort_func(sigc::mem_fun(*this, &MALItemListViewBase::malitem_comparitor));
        m_root_model->set_sort_column(Gtk::TreeSortable::DEFAULT_SORT_COLUMN_ID, Gtk::SORT_DESCENDING);
#if GTK_CHECK_VERSION(3,8,0)
        m_treeview->set_activate_on_single_click(true);
#endif
	}

    int MALItemListViewBase::malitem_comparitor(const Gtk::TreeModel::iterator& a, const Gtk::TreeModel::iterator& b)
    {
        auto & season_column = m_columns->series_start_date;
        auto season = a->get_value(season_column).compare(0, 7, b->get_value(season_column));
        if (season == 0) {
            auto & title_column = m_columns->series_title;
            return b->get_value(title_column).compare(a->get_value(title_column));
        } else {
            return season;
        }
    }

    void MALItemListViewBase::set_filter_func(const sigc::slot<bool, const std::shared_ptr<MALItem>&>& slot)
    {
        m_filter_func = slot;
    }

    void MALItemListViewBase::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row) {
        row.set_value(m_columns->series_title, Glib::ustring(item->series_title));
        row.set_value(m_columns->series_season, Glib::ustring(item->get_season_began()));
        auto sort_text = item->series_date_begin.substr(0,7);
        row.set_value(m_columns->series_start_date, Glib::ustring(sort_text));
        row.set_value(m_columns->item, item);
    }

    /** Clear the list view and repopulate from the for_each_functor
     *
     * Because the MALItems are stored behind a std::mutex, we can not
     * access them directly. Instead the mutex is taken and
     * std::for_each(items.begin(), items.end(), ItemFunctor) is
     * called. Unfortunately, ItemFunctor can not be a public version
     * of append_item() because the model_changed_connection needs to
     * be blocked.
     *
     * So, sadly, this method is called with a functor to the for_each
     * functor, and the argument to that functor is a functor that is
     * fed the MALItem.
     *
     * @for_each_functor: functor to a std::for_each-like function
     */
    void MALItemListViewBase::refresh_items(const std::function<void (const std::function<void (const std::shared_ptr<MALItem>&)>&)>& for_each_functor)
    {
        m_root_model->clear();
        m_model_changed_connection.block();
        for_each_functor(std::bind(&MALItemListViewBase::append_item, this, std::placeholders::_1));
        m_model_changed_connection.unblock();
    }

    void MALItemListViewBase::append_item(const std::shared_ptr<MALItem>& item)
    {
        if (m_filter_func) {
            if (m_filter_func(item)) {
                auto iter = m_root_model->append();
                refresh_item_cb(item, *iter);
            }
        } else {
            auto iter = m_root_model->append();
            refresh_item_cb(item, *iter);
        }
    }

	void MALItemListViewBase::on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*)
    {
		auto iter = m_model->get_iter(path);
        m_detailed_item = iter->get_value(m_columns->item);
		if (m_row_activated_cb) {
            m_row_activated_cb(m_detailed_item);
        }
	}

    MALItemListViewStatic::MALItemListViewStatic(const std::shared_ptr<MAL>& mal,
                                                 const std::shared_ptr<MALItemModelColumnsStatic>& columns) :
        MALItemListViewBase(mal, columns)
    {
		int num = m_treeview->append_column_numeric("Score", columns->score, "%.2f");
        m_score_column = m_treeview->get_column(num - 1);
    }

    void MALItemListViewStatic::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsStatic>(m_columns);
        row.set_value(columns->score, item->score);
    }

    MALItemListViewEditable::MALItemListViewEditable(const std::shared_ptr<MAL>& mal,
                                                     const std::shared_ptr<MALItemModelColumnsEditable>& columns) :
        MALItemListViewBase(mal, columns),
        m_score_column(Gtk::manage(new Gtk::TreeViewColumn("Score"))),
        m_score_cellrenderer(Gtk::manage(new CellRendererScore()))
    {
        m_score_column->pack_start(*m_score_cellrenderer);
        m_score_column->add_attribute(m_score_cellrenderer->property_score(), columns->score);
        //m_score_column->set_alignment(Pango::ALIGN_CENTER);
        m_score_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &MALItemListViewEditable::score_edited_cb));
        m_model_changed_connection = m_root_model->signal_row_changed().connect(sigc::mem_fun(*this, &MALItemListViewEditable::on_model_changed));
        m_treeview->append_column(*m_score_column);
		m_treeview->signal_row_activated().connect([this](const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*)->void{
                auto iter = this->m_model->get_iter(path);
                auto detailed_item = iter->get_value(m_columns->item);
                if (this->m_row_activated_cb && !this->m_detailed_item->has_details) {
                    this->get_details_for_item(detailed_item);
                }
                    });
    }

    void MALItemListViewEditable::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_columns);
        row.set_value(columns->score, static_cast<int>(item->score));
        row.set_value(columns->begin_date, Glib::ustring(item->date_start));
        row.set_value(columns->end_date, Glib::ustring(item->date_finish));
        row.set_value(columns->enable_reconsuming, item->enable_reconsuming);
        if (item->has_details) {
            row.set_value(columns->fansub_group, Glib::ustring(item->fansub_group));
            row.set_value(columns->downloaded_items, static_cast<int>(item->downloaded_items));
            row.set_value(columns->times_consumed, static_cast<int>(item->times_consumed));
            row.set_value(columns->priority, item->priority);
            row.set_value(columns->reconsume_value, item->reconsume_value);
        }
    }

    void MALItemListViewEditable::on_detailed_item_cb(const Gtk::TreeRow& row)
    {
        if (m_detailed_item->has_details) {
            auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_columns);
            row.set_value(columns->fansub_group, Glib::ustring(m_detailed_item->fansub_group));
            row.set_value(columns->downloaded_items, static_cast<int>(m_detailed_item->downloaded_items));
            row.set_value(columns->times_consumed, static_cast<int>(m_detailed_item->times_consumed));
            row.set_value(columns->priority, m_detailed_item->priority);
            row.set_value(columns->reconsume_value, m_detailed_item->reconsume_value);
        }
    }

    void MALItemListViewEditable::on_detailed_item()
    {
        if (m_detailed_item->has_details) {
            m_model_changed_connection.block();
            auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_columns);
            m_model->foreach_iter([this, &columns](const Gtk::TreeModel::iterator& iter)->bool {
                    if (this->m_detailed_item->series_itemdb_id == iter->get_value(columns->item)->series_itemdb_id) {
                        this->on_detailed_item_cb(*iter);
                        if (m_row_activated_cb) m_row_activated_cb(m_detailed_item);
                        return true;
                    } else {
                        return false;
                    }
                });
            m_model_changed_connection.unblock();
        }
    }

    void MALItemListViewEditable::on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator& iter)
    {
        bool is_changed = false;
        auto item = iter->get_value(m_columns->item);
        is_changed = model_changed_cb(item, *iter); // May change item!

        if (is_changed) {
            // model_changed_cb has set an updated item into the model.
            item = iter->get_value(m_columns->item);
            if (!m_detailed_item || m_detailed_item->series_itemdb_id == item->series_itemdb_id)
                m_row_activated_cb(item);
            send_item_update(item);
        }
    }

    bool MALItemListViewEditable::model_changed_cb(std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
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

        auto const begin = row.get_value(columns->begin_date);
        if (begin != item->date_start) {
            is_changed = true;
            auto new_item = item->clone();
            new_item->date_start = begin;
            item = new_item;
            row.set_value(columns->item, item);
        }

        auto const end = row.get_value(columns->end_date);
        if (end != item->date_finish) {
            is_changed = true;
            auto new_item = item->clone();
            new_item->date_finish = end;
            item = new_item;
            row.set_value(columns->item, item);
        }

        auto const reconsuming = row.get_value(columns->enable_reconsuming);
        if (reconsuming != item->enable_reconsuming) {
            is_changed = true;
            auto new_item = item->clone();
            new_item->enable_reconsuming = reconsuming;
            item = new_item;
            row.set_value(columns->item, item);
        }

        if (item->has_details) {
            auto const fansubgroup = row.get_value(columns->fansub_group);
            if (fansubgroup != item->fansub_group) {
                is_changed = true;
                auto new_item = item->clone();
                new_item->fansub_group = fansubgroup;
                item = new_item;
                row.set_value(columns->item, item);
            }

            auto const downloaded_items = row.get_value(columns->downloaded_items);
            if (downloaded_items != item->downloaded_items) {
                is_changed = true;
                auto new_item = item->clone();
                new_item->downloaded_items = downloaded_items;
                item = new_item;
                row.set_value(columns->item, item);
            }

            auto const times_consumed = row.get_value(columns->times_consumed);
            if (times_consumed != item->times_consumed) {
                is_changed = true;
                auto new_item = item->clone();
                new_item->times_consumed = times_consumed;
                item = new_item;
                row.set_value(columns->item, item);
            }

            auto const priority = row.get_value(columns->priority);
            if (priority != item->priority) {
                is_changed = true;
                auto new_item = item->clone();
                new_item->priority = priority;
                item = new_item;
                row.set_value(columns->item, item);
            }

            auto const reconsume_value = row.get_value(columns->reconsume_value);
            if (reconsume_value != item->reconsume_value) {
                is_changed = true;
                auto new_item = item->clone();
                new_item->reconsume_value = reconsume_value;
                item = new_item;
                row.set_value(columns->item, item);
            }
        }

        return is_changed;
    }

    void MALItemListViewEditable::score_edited_cb(const Glib::ustring& path, const Glib::ustring& text)
    {
        auto iter = m_model->get_iter(path);
        auto columns = std::dynamic_pointer_cast<MALItemModelColumnsEditable>(m_columns);

        if (iter) {
            auto const score = m_score_cellrenderer->get_score_from_string(text);
            auto item = iter->get_value(columns->item);
            if ( score != static_cast<int>(item->score) ) {
                iter->set_value(columns->score, score);
            }
        }
    }

    void MALItemListViewEditable::do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter& slot)
    {
        m_root_model->foreach(slot);
    }

	MALItemListPage::MALItemListPage(const std::shared_ptr<MAL>& mal,
                                     MALItemListViewBase* list_view,
                                     MALItemDetailViewBase* detail_view) :
		m_mal(mal),
		m_list_view(list_view),
        m_detail_view(detail_view),
        m_button_row(Gtk::manage(new Gtk::Grid())),
        m_refresh_button(Gtk::manage(new Gtk::Button(""))),
        m_progressbar(Gtk::manage(new Gtk::ProgressBar()))
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
		action->signal_activate().connect(sigc::mem_fun(*this, &MALItemListPage::refresh));
		m_refresh_button->set_related_action(action);
        m_refresh_button->set_can_default(true);
        m_refresh_button->set_receives_default(true);
        m_progressbar->set_show_text(true);
		if (m_detail_view) attach(*m_detail_view, 0, 0, 1, 1);
        attach(*m_button_row, 0, 1, 1, 1);
		m_button_row->attach(*m_refresh_button, 0, 0, 1, 1);
		attach(*list_view, 0, 2, 1, 1);
        m_button_row->attach_next_to(*m_progressbar, *m_refresh_button, Gtk::POS_RIGHT, 1, 1);
		show_all();
		if (m_detail_view) m_detail_view->hide();
        m_progressbar->hide();
	}
}
