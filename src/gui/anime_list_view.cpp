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

#include "anime_list_view.hpp"
#include <iostream>
#include <cstring>
#include <glibmm/markup.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/eventbox.h>

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
        append(to_string(WATCHING));
        append(to_string(COMPLETED));
        append(to_string(ONHOLD));
        append(to_string(DROPPED));
        append(to_string(PLANTOWATCH));
        set_active_text(to_string(WATCHING));
	}

	AnimeStatus AnimeStatusComboBox::get_anime_status() const {
        return anime_status_from_string(get_active_text());
	}

    AnimeDetailViewBase::AnimeDetailViewBase(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        m_status_type_grid(Gtk::manage(new Gtk::Grid())),
        m_status_type_sizegroup(Gtk::SizeGroup::create(Gtk::SIZE_GROUP_BOTH)),
        m_series_status_label(Gtk::manage(new FancyLabel())),
        m_series_type_label(Gtk::manage(new FancyLabel()))
    {
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_TOP);
        m_grid_left->attach_next_to(*m_status_type_grid, *m_series_date_grid,
                               Gtk::POS_TOP, 1, 1);
        auto type_box = Gtk::manage(new Gtk::EventBox());
        type_box->add(*m_series_type_label);
        type_box->set_visible_window(true);
        auto status_box = Gtk::manage(new Gtk::EventBox());
        status_box->add(*m_series_status_label);
        status_box->set_visible_window(true);
        m_status_type_sizegroup->add_widget(m_maximum_length_label);
        m_maximum_length_label.set_label(to_string(NOTYETAIRED));
        m_status_type_sizegroup->add_widget(*m_series_type_label);
        m_status_type_sizegroup->add_widget(*m_series_status_label);
        m_status_type_grid->attach(*type_box, 0, 0, 1, 1);
        m_status_type_grid->attach(*status_box, 1, 0, 1, 1);
        m_status_type_grid->set_column_spacing(5);
        m_series_status_label->set_vexpand(true);
        m_series_type_label->set_vexpand(true);
    }

    void AnimeDetailViewBase::display_item(const std::shared_ptr<MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);
        auto anime = std::static_pointer_cast<Anime>(item);

        m_series_status_label->set_label(to_string(anime->series_status));
        m_series_type_label->set_label(to_string(anime->series_type));
    }

    AnimeDetailViewStatic::AnimeDetailViewStatic(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        MALItemDetailViewStatic(mal),
        AnimeDetailViewBase(mal),
        m_episodes_grid(Gtk::manage(new Gtk::Grid())),
        m_series_episodes_label(Gtk::manage(new Gtk::Label()))
    {
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_episodes_grid, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_episodes_grid->attach(*m_series_episodes_label, 0, 0, 1, 1);
    }

    void AnimeDetailViewStatic::display_item(const std::shared_ptr<MALItem>& item)
    {
        MALItemDetailViewStatic::display_item(item);
        AnimeDetailViewBase::display_item(item);

        auto anime = std::static_pointer_cast<Anime>(item);
        m_series_episodes_label->set_text(std::to_string(anime->series_episodes) + " Episodes");
    }

    AnimeDetailViewEditable::AnimeDetailViewEditable(const std::shared_ptr<MAL>& mal,
                                                     const std::shared_ptr<AnimeModelColumnsEditable>& columns,
                                                     const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>& slot) :
        MALItemDetailViewBase(mal),
        MALItemDetailViewEditable(mal, columns, slot),
        AnimeDetailViewBase(mal),
        m_episodes_entry(Gtk::manage(new IncrementEntry())),
        m_anime_status_combo(Gtk::manage(new AnimeStatusComboBox()))
    {
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_episodes_entry, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_anime_status_combo, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_episodes_entry->signal_activate().connect(sigc::mem_fun(*this, &AnimeDetailViewEditable::notify_list_model));
        m_anime_status_changed_connection = m_anime_status_combo->signal_changed().connect(sigc::mem_fun(*this, &AnimeDetailViewEditable::notify_list_model));
    }

    void AnimeDetailViewEditable::display_item(const std::shared_ptr<MALItem>& item)
    {
        m_anime_status_changed_connection.block();
        auto const olditem = m_item;
        MALItemDetailViewEditable::display_item(item);
        auto anime = std::static_pointer_cast<Anime>(item);
        if (!olditem || olditem->series_itemdb_id != item->series_itemdb_id) {
            AnimeDetailViewBase::display_item(item);
            m_episodes_entry->set_label("/ " + std::to_string(anime->series_episodes) + " Episodes");
            m_episodes_entry->set_entry_text(std::to_string(anime->episodes));
            m_anime_status_combo->set_active_text(to_string(anime->status));
        }

        if (anime->status == COMPLETED) {
            m_date_end_entry->set_sensitive(true);
            m_times_consumed_entry->show();
        } else {
            m_date_end_entry->set_sensitive(false);
            m_times_consumed_entry->hide();
        }

        m_anime_status_changed_connection.unblock();
    }

    bool AnimeDetailViewEditable::update_list_model(const Gtk::TreeRow &row)
    {
        if (row.get_value(m_columns->item)->series_itemdb_id != m_item->series_itemdb_id)
            return false;
        MALItemDetailViewEditable::update_list_model(row);
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_notify_columns);

        try {
            auto episodes = std::stoi(m_episodes_entry->get_entry_text());
            if (episodes != row.get_value(columns->episodes))
                row.set_value(columns->episodes, episodes);
        } catch (std::exception e) {
            auto anime = std::static_pointer_cast<Anime>(m_item);
            m_episodes_entry->set_entry_text(std::to_string(anime->episodes));
        }

        auto status = m_anime_status_combo->get_anime_status();
        if (status != row.get_value(columns->status))
            row.set_value(columns->status, Glib::ustring(to_string(status)));

        return true;
    }

    AnimeListViewBase::AnimeListViewBase(const std::shared_ptr<MAL>& mal,
                                         const std::shared_ptr<AnimeModelColumnsBase>& columns) :
        MALItemListViewBase(mal, columns),
        m_series_type_cellrenderer(Gtk::manage(new FancyCellRendererText())),
        m_series_status_cellrenderer(Gtk::manage(new FancyCellRendererText())),
        m_series_type_column(Gtk::manage(new Gtk::TreeViewColumn("Type", *m_series_type_cellrenderer))),
        m_series_status_column(Gtk::manage(new Gtk::TreeViewColumn("Airing Status", *m_series_status_cellrenderer))),
        m_series_episodes_column(Gtk::manage(new Gtk::TreeViewColumn("Eps.", columns->series_episodes)))
    {
        m_series_type_column->add_attribute(m_series_type_cellrenderer->property_text(), columns->series_type);
        m_series_status_column->add_attribute(m_series_status_cellrenderer->property_text(), columns->series_status);
        m_treeview->append_column(*m_series_type_column);
        m_treeview->append_column(*m_series_episodes_column);
        m_treeview->append_column(*m_series_status_column);
        m_treeview->move_column_after(*m_series_status_column, *m_title_column);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new anime list from MAL)
     * This method should take data from the item and put it on the row
     */
    void AnimeListViewBase::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsBase>(m_columns);
        auto anime = std::static_pointer_cast<Anime>(item);

        row.set_value(columns->series_type, Glib::ustring(to_string(anime->series_type)));
        row.set_value(columns->series_status, Glib::ustring(to_string(anime->series_status)));
        row.set_value(columns->series_episodes, static_cast<int>(anime->series_episodes));
        row.set_value(columns->anime, anime);
    }

    AnimeListViewStatic::AnimeListViewStatic(const std::shared_ptr<MAL>& mal,
                                             const std::shared_ptr<AnimeModelColumnsStatic>& columns) :
        MALItemListViewBase(mal, columns),
        MALItemListViewStatic(mal, columns),
        AnimeListViewBase(mal, columns),
        m_status_column(Gtk::manage(new Gtk::TreeViewColumn("Viewing Status")))
    {
		auto status_cellrenderer = Gtk::manage(new AnimeStatusCellRendererCombo());
		m_status_column->pack_start(*status_cellrenderer);
		m_status_column->add_attribute(status_cellrenderer->property_text(), columns->status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &AnimeListViewStatic::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;
		m_treeview->append_column(*m_status_column);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new anime list from MAL)
     * This method should take data from the item and put it on the row
     */
    void AnimeListViewStatic::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        AnimeListViewBase::refresh_item_cb(item, row);
        MALItemListViewStatic::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsStatic>(m_columns);
        auto anime = std::static_pointer_cast<Anime>(item);
        auto status = anime->status;
        if (status == ANIMESTATUS_INVALID || status == 0)
            row.set_value(columns->status, Glib::ustring("Add To My Anime List..."));
        else
            row.set_value(columns->status, Glib::ustring(to_string(anime->status)));
    }

	void AnimeListViewStatic::on_status_cr_changed(const Glib::ustring& path,
                                                   const Glib::ustring& new_text)
    {
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsStatic>(m_columns);
		Gtk::TreeModel::iterator iter = m_model->get_iter(path);

		if (iter) {
			auto status = anime_status_from_string(new_text);
            auto anime = iter->get_value(columns->anime);
			if (status != anime->status) {
                auto new_anime = std::static_pointer_cast<Anime>(anime->clone());
				iter->set_value(columns->status, Glib::ustring(to_string(status)));
				new_anime->status = status;
				iter->set_value(columns->anime, anime);
                new_anime->score = 0.0f;
                if (anime->status == COMPLETED)
                    new_anime->episodes = anime->series_episodes;
                else
                    new_anime->episodes = 0;
                if (anime->status != ANIMESTATUS_INVALID) {
                    m_mal->add_anime_async(*new_anime);
                }
            }
		}
	}

    AnimeListViewEditable::AnimeListViewEditable(const std::shared_ptr<MAL>& mal,
                                                 const std::shared_ptr<AnimeModelColumnsEditable>& columns) :
        MALItemListViewBase(mal, columns),
        MALItemListViewEditable(mal, columns),
        AnimeListViewBase(mal, columns),
        m_status_column(Gtk::manage(new Gtk::TreeViewColumn("Viewing Status")))
    {
		auto status_cellrenderer = Gtk::manage(new AnimeStatusCellRendererCombo());
		m_status_column->pack_start(*status_cellrenderer);
		m_status_column->add_attribute(status_cellrenderer->property_text(), columns->status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &AnimeListViewEditable::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;
		m_treeview->append_column(*m_status_column);
        auto num = m_treeview->append_column_numeric_editable("Seen", columns->episodes, "%d");
        m_episodes_column = m_treeview->get_column(num - 1);
        m_treeview->move_column_after(*m_episodes_column, *m_series_type_column);
        m_mal->signal_anime_detailed.connect(sigc::mem_fun(this, &AnimeListViewEditable::on_detailed_item));
    }

    void AnimeListViewEditable::get_details_for_item(const std::shared_ptr<MALItem>& item)
    {
        auto anime = std::static_pointer_cast<Anime>(item);
        if (!anime->has_details)
            m_mal->get_anime_details_async(anime);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new anime list from MAL)
     * This method should take data from the item and put it on the row
     */
    void AnimeListViewEditable::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        AnimeListViewBase::refresh_item_cb(item, row);
        MALItemListViewEditable::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_columns);
        auto anime = std::static_pointer_cast<Anime>(item);

        row.set_value(columns->status, Glib::ustring(to_string(anime->status)));
        row.set_value(columns->episodes, static_cast<int>(anime->episodes));
    }

    /* Chain up!
     * Called when the tree model was changed due to editing.
     * This method should set the appropriate field in item from
     * the tree row, then set the item back into the model as
     * well. 
     * Return true when the item value is different from the model value.
     */
    bool AnimeListViewEditable::model_changed_cb(std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        bool is_changed = false;
        auto mal_changed = MALItemListViewEditable::model_changed_cb(item, row);
        
        auto const columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_columns);
        auto anime = std::static_pointer_cast<Anime>(item);
        auto new_anime = std::static_pointer_cast<Anime>(anime->clone());
        if (mal_changed)
            row.set_value(columns->anime, anime);

        auto const episodes = row.get_value(columns->episodes);
        if (episodes != anime->episodes) {
            is_changed = true;
            new_anime->episodes = episodes;
            item = anime = new_anime;
            row.set_value(columns->item, item);
            row.set_value(columns->anime, anime);
        }

        auto const status = anime_status_from_string(row.get_value(columns->status));
        if (status != anime->status) {
            is_changed = true;
            new_anime->status = status;
            item = anime = new_anime;
            row.set_value(columns->item, item);
            row.set_value(columns->anime, anime);
        }

        return is_changed || mal_changed;
    }

    /* Called on main thread. Item should be transmitted back to MAL.net.
     */
    void AnimeListViewEditable::send_item_update(const std::shared_ptr<MALItem>& item)
    {
        auto anime = std::static_pointer_cast<Anime>(item);
        m_mal->update_anime_async(anime);
    }

    void AnimeListViewEditable::on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text)
    {
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_columns);
		Gtk::TreeModel::iterator iter = m_model->get_iter(path);

		if (iter) {
			auto status = anime_status_from_string(new_text);
            auto anime = iter->get_value(columns->anime);

			if (status != anime->status) {
                auto new_anime = std::static_pointer_cast<Anime>(anime->clone());
				iter->set_value(columns->status, Glib::ustring(to_string(status)));
				new_anime->status = status;
                anime = new_anime;
				iter->set_value(columns->anime, anime);

                if (status != ANIMESTATUS_INVALID) {
                    send_item_update(anime);
                }
            }
		}
    }
    
    AnimeSearchListPage::AnimeSearchListPage(const std::shared_ptr<MAL>& mal,
                                             AnimeListViewStatic*        list_view,
                                             AnimeDetailViewStatic*      detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_search_entry(Gtk::manage(new Gtk::Entry()))
    {
        m_button_row->attach(*m_search_entry, -1, 0, 1, 1);
        m_search_entry->set_hexpand(true);
        m_search_entry->set_activates_default(true);
        m_search_entry->show();

		auto icon = m_refresh_button->render_icon_pixbuf(Gtk::Stock::FIND, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		m_refresh_button->set_image(*image);
		m_refresh_button->set_tooltip_text("Search myanimelist.net for anime that maches the entered terms.");
        mal->signal_anime_search_completed.connect(sigc::mem_fun(*this, &AnimeSearchListPage::on_mal_update));
    }

    void AnimeSearchListPage::refresh()
    {
        m_mal->search_anime_async(m_search_entry->get_text());
    }

    void AnimeSearchListPage::on_mal_update()
    {
        /* Because MAL::for_each_anime is implemented as a template,
         * we can't use std::mem_fn. Instead we need to use a lambda
         * so that there is a definite type.
         */
        m_list_view->refresh_items([&](std::function<void (const std::shared_ptr<MALItem>&)>&& f)->void {
                m_mal->for_each_anime_search_result(std::forward<std::function<void (const std::shared_ptr<MALItem>&)>>(f));
            });
    }

    AnimeFilteredListPage::AnimeFilteredListPage(const std::shared_ptr<MAL>& mal,
                                                 AnimeListViewEditable*      list_view,
                                                 AnimeDetailViewEditable*    detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_list_view(list_view),
        m_detail_view(detail_view),
        m_status_combo(Gtk::manage(new AnimeStatusComboBox()))
    {
        m_status_combo->signal_changed().connect(sigc::mem_fun(this, &AnimeFilteredListPage::on_mal_update));
        auto label = Gtk::manage(new Gtk::Label("Filter: "));
        m_button_row->attach(*m_status_combo, -1, 0, 1, 1);
        m_button_row->attach(*label, -2, 0, 1, 1);
        m_status_combo->set_hexpand(true);
        m_status_combo->set_active_text(to_string(WATCHING));
        m_status_combo->show();
        list_view->set_filter_func(sigc::mem_fun(*this, &AnimeFilteredListPage::m_filter_func));
        mal->signal_anime_added.connect(sigc::mem_fun(*this, &AnimeFilteredListPage::on_mal_update));
    }

    bool AnimeFilteredListPage::m_filter_func(const std::shared_ptr<MALItem>& item) const
    {
        auto anime = std::static_pointer_cast<Anime>(item);
        return anime->status == m_status_combo->get_anime_status();
    }

    void AnimeFilteredListPage::refresh()
    {
        m_mal->get_anime_list_async();
    }

    void AnimeFilteredListPage::on_mal_update()
    {
        /* Because MAL::for_each_anime is implemented as a template,
         * we can't use std::mem_fn. Instead we need to use a lambda
         * so that there is a definite type.
         */
        m_list_view->refresh_items([&](std::function<void (const std::shared_ptr<MALItem>&)>&& f)->void {
                m_mal->for_each_anime(std::forward<std::function<void (const std::shared_ptr<MALItem>&)>>(f));
            }
            );
    }
}
