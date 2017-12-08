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
#include <gtkmm/eventbox.h>

namespace sigc {
    SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
}

namespace MAL {

    namespace {
        void populate_anime_status_model(Glib::RefPtr<Gtk::ListStore>& model, const AnimeStatusColumns& columns ) {
            auto iter = model->append();
            iter->set_value(columns.text, Glib::ustring(to_string(AnimeStatus::WATCHING)));
            iter->set_value(columns.status, AnimeStatus::WATCHING);
            iter = model->append();
            iter->set_value(columns.text, Glib::ustring(to_string(AnimeStatus::COMPLETED)));
            iter->set_value(columns.status, AnimeStatus::COMPLETED);
            iter = model->append();
            iter->set_value(columns.text, Glib::ustring(to_string(AnimeStatus::ONHOLD)));
            iter->set_value(columns.status, AnimeStatus::ONHOLD);
            iter = model->append();
            iter->set_value(columns.text, Glib::ustring(to_string(AnimeStatus::DROPPED)));
            iter->set_value(columns.status, AnimeStatus::DROPPED);
            iter = model->append();
            iter->set_value(columns.text, Glib::ustring(to_string(AnimeStatus::PLANTOWATCH)));
            iter->set_value(columns.status, AnimeStatus::PLANTOWATCH);
        }
    }

    AnimeStatusCellRendererCombo::AnimeStatusCellRendererCombo() :
        Gtk::CellRendererCombo(),
        model(Gtk::ListStore::create(columns))
    {
        populate_anime_status_model(model, columns);
        property_model() = model;
        property_text_column() = columns.text.index();
        property_has_entry() = false;
    }

    AnimeStatusComboBox::AnimeStatusComboBox(bool with_none) :
        Gtk::ComboBox(),
        model(Gtk::ListStore::create(columns))
    {
        if (with_none) {
            auto iter = model->append();
            iter->set_value(columns.text, Glib::ustring("All"));
            iter->set_value(columns.status, AnimeStatus::NONE);
        }
        populate_anime_status_model(model, columns);
        set_model(model);
        pack_start(columns.text);
        set_anime_status(AnimeStatus::WATCHING);
    }

    AnimeStatus AnimeStatusComboBox::get_anime_status() const
    {
        auto iter = get_active();
        return iter->get_value(columns.status);
    }

    void AnimeStatusComboBox::set_anime_status(AnimeStatus status)
    {
        model->foreach_iter([this, status](const Gtk::TreeModel::iterator& iter) {
                if (iter->get_value(columns.status) == status) {
                    set_active(iter);
                    return true;
                }
                return false;
            });
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
        m_maximum_length_label.set_label(to_string(SeriesStatus::NOTYETAIRED));
        m_status_type_sizegroup->add_widget(*m_series_type_label);
        m_status_type_sizegroup->add_widget(*m_series_status_label);
        m_status_type_grid->attach(*type_box, 0, 0, 1, 1);
        m_status_type_grid->attach(*status_box, 1, 0, 1, 1);
        m_status_type_grid->set_column_spacing(5);
        m_status_type_grid->set_hexpand(false);
        m_series_status_label->set_vexpand(true);
        m_series_type_label->set_vexpand(true);
        m_series_status_label->set_hexpand(true);
        m_series_type_label->set_hexpand(true);
    }

    void AnimeDetailViewBase::display_item(const std::shared_ptr<MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);
        auto anime = std::static_pointer_cast<Anime>(item);

        m_series_status_label->set_label(to_string(anime->series_status));
        m_series_type_label->set_label(to_string(anime->series_type));

        if (anime->series_synopsis.empty()) {
            m_synopsis_frame->show_all();
            m_synopsis_label->set_markup("Fetching...");
            m_mal->refresh_anime_async(anime, [this](const auto& new_anime) {
                    if (!new_anime) {
                        m_synopsis_label->set_markup("Failed to fetch synopsis.");
                        return;
                    }
                    if (m_item->series_itemdb_id != new_anime->series_itemdb_id) {
                        return;
                    }

                    m_synopsis_label->set_markup(Glib::Markup::escape_text(m_item->series_synopsis));
                });
        }
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

    void AnimeDetailViewEditable::anime_status_changed()
    {
        auto status = m_anime_status_combo->get_anime_status();
        if (status == AnimeStatus::COMPLETED) {
            m_date_end_entry->set_sensitive(true);
            m_date_end_button->set_sensitive(true);
            m_date_end_grid->show();
            m_times_consumed_entry->show();
        } else {
            m_date_end_entry->set_sensitive(false);
            m_date_end_button->set_sensitive(false);
            m_date_end_grid->hide();
            m_times_consumed_entry->hide();
        }
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
        m_anime_status_combo->signal_changed().connect(sigc::mem_fun(*this, &AnimeDetailViewEditable::anime_status_changed));
    }

    void AnimeDetailViewEditable::display_item(const std::shared_ptr<MALItem>& item)
    {
        m_anime_status_changed_connection.block();
        auto const olditem = m_item;
        MALItemDetailViewEditable::display_item(item);
        auto anime = std::static_pointer_cast<Anime>(item);
        if (!olditem || olditem->series_itemdb_id != item->series_itemdb_id) {
            AnimeDetailViewBase::display_item(item);
            using Glib::ustring;
            m_episodes_entry->set_label(ustring::compose("/ %1 Episodes", anime->series_episodes));
            m_episodes_entry->set_entry_text(ustring::format(anime->episodes));
            m_anime_status_combo->set_anime_status(anime->status);
        }
        anime_status_changed();

        m_anime_status_changed_connection.unblock();
    }

    bool AnimeDetailViewEditable::update_list_model(const Gtk::TreeRow &row)
    {
        if (row.get_value(m_columns->item)->series_itemdb_id != m_item->series_itemdb_id)
            return false;
        MALItemDetailViewEditable::update_list_model(row);
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_notify_columns);

        auto const status = m_anime_status_combo->get_anime_status();
        auto const row_status = anime_status(row.get_value(columns->status));
        if ( status != row_status ) {
            row.set_value(columns->status, Glib::ustring(to_string(status)));
        }

        try {
            auto episodes = std::stoi(m_episodes_entry->get_entry_text());
            if ((status == AnimeStatus::COMPLETED) && (row_status != status)) {
                /* User just set status to Completed, so set episodes
                 * to series_episodes */
                auto anime = std::static_pointer_cast<Anime>(m_item);
                if (anime->series_episodes > 0) {
                    m_episodes_entry->set_entry_text(std::to_string(anime->series_episodes));
                    row.set_value(columns->episodes, static_cast<int>(anime->series_episodes));
                }
            } else {
                if (episodes != row.get_value(columns->episodes)) {
                    row.set_value(columns->episodes, episodes);
                }
            }
        } catch (std::exception e) {
            auto anime = std::static_pointer_cast<Anime>(m_item);
            m_episodes_entry->set_entry_text(std::to_string(anime->episodes));
        }

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
        m_status_column->add_attribute(status_cellrenderer->property_editable(), columns->status_editable);
        status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &AnimeListViewStatic::on_status_cr_changed));
//      status_cellrenderer->property_editable() = true;
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

        auto my_anime = m_mal->find_anime(anime);
        if (my_anime) {
            anime->status = my_anime->status;
        }

        auto status = anime->status;
        if (status == AnimeStatus::INVALID || status == AnimeStatus::NONE) {
            row.set_value(columns->status, Glib::ustring("Add To My Anime List..."));
            row.set_value(columns->status_editable, true);
        } else {
            row.set_value(columns->status, Glib::ustring(to_string(anime->status)));
            row.set_value(columns->status_editable, false);
        }
    }

    void AnimeListViewStatic::on_status_cr_changed(const Glib::ustring& path,
                                                   const Glib::ustring& new_text)
    {
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsStatic>(m_columns);
        Gtk::TreeModel::iterator iter = m_model->get_iter(path);

        if (iter) {
            auto const status = anime_status(new_text);
            auto anime = iter->get_value(columns->anime);

            if (status != anime->status) {
                auto new_anime = std::static_pointer_cast<Anime>(anime->clone());
                new_anime->status = status;
                new_anime->score = 0.0f;
                if (status == AnimeStatus::COMPLETED) {
                    new_anime->episodes = new_anime->series_episodes;
                } else {
                    new_anime->episodes = 0;
                }

                if (new_anime->status != AnimeStatus::INVALID) {
                    auto fn = [this, new_anime, columns](bool success) {
                        m_model->foreach_iter([this, new_anime, columns, success](const Gtk::TreeModel::iterator& it)->bool {
                                auto walked_anime = it->get_value(columns->anime);
                                if (walked_anime->series_itemdb_id == new_anime->series_itemdb_id) {
                                    if (success) {
                                        it->set_value(columns->status_editable, false);
                                    } else {
                                        it->set_value(columns->status, Glib::ustring("Add To My Anime List..."));
                                        it->set_value(columns->status_editable, true);
                                    }
                                    return true;
                                } else {
                                    return false;
                                }
                            });
                    };

                    m_mal->add_anime_async(*new_anime, fn);
                    iter->set_value(columns->status, Glib::ustring(to_string(status)));
                    iter->set_value(columns->anime, new_anime);
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
            row.set_value(columns->item, std::static_pointer_cast<MALItem>(new_anime));
            row.set_value(columns->anime, new_anime);
        }
        auto const status = anime_status(row.get_value(columns->status));
        if (status != anime->status) {
            is_changed = true;
            new_anime->status = status;
            row.set_value(columns->item, std::static_pointer_cast<MALItem>(new_anime));
            row.set_value(columns->anime, new_anime);
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
            auto status = anime_status(new_text);
            auto anime = iter->get_value(columns->anime);

            if (status != anime->status) {
                auto new_anime = std::static_pointer_cast<Anime>(anime->clone());
                iter->set_value(columns->status, Glib::ustring(to_string(status)));
                new_anime->status = status;
                anime = new_anime;
                iter->set_value(columns->anime, anime);

                if (status != AnimeStatus::INVALID) {
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

        m_refresh_button->set_image_from_icon_name("edit-find");
        m_refresh_button->set_tooltip_text("Search myanimelist.net for anime that maches the entered terms.");
        mal->signal_anime_search_completed.connect(sigc::mem_fun(*this, &AnimeSearchListPage::on_mal_update));
    }

    void AnimeSearchListPage::refresh()
    {
        m_mal->search_anime_async(m_search_entry->get_text());
    }

    void AnimeSearchListPage::on_mal_update()
    {
        using std::placeholders::_1;
        m_list_view->refresh_items(std::bind(&MAL::for_each_anime_search_result, m_mal, _1));
    }

    namespace {
        constexpr gint64 usec_per_sec = G_USEC_PER_SEC;
        constexpr gint64 sixty_fps_in_us = usec_per_sec / 60;
    }

    AnimeFilteredListPage::AnimeFilteredListPage(const std::shared_ptr<MAL>& mal,
                                                 const std::shared_ptr<AnimeModelColumnsEditable>& columns,
                                                 AnimeListViewEditable*      list_view,
                                                 AnimeDetailViewEditable*    detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_columns(columns),
        m_list_view(list_view),
        m_detail_view(detail_view),
        m_status_combo(Gtk::manage(new AnimeStatusComboBox(true))),
        last_pulse(g_get_monotonic_time())
    {
        m_list_view->set_visible_func(sigc::mem_fun(this, &AnimeFilteredListPage::m_visible_func));
        m_status_combo->signal_changed().connect(sigc::mem_fun(static_cast<MALItemListViewBase*>(m_list_view), &MALItemListViewEditable::refilter));

        auto label = Gtk::manage(new Gtk::Label("Filter: "));
        m_button_row->attach(*m_status_combo, -1, 0, 1, 1);
        m_button_row->attach(*label, -2, 0, 1, 1);
        m_status_combo->set_hexpand(true);
        m_status_combo->set_anime_status(AnimeStatus::WATCHING);
        m_status_combo->show();
        mal->signal_anime_added.connect(sigc::mem_fun(*this, &AnimeFilteredListPage::on_mal_update));
    }

    bool AnimeFilteredListPage::m_filter_func(const std::shared_ptr<MALItem>& item) const
    {
        auto anime = std::static_pointer_cast<Anime>(item);
        return anime->status == m_status_combo->get_anime_status();
    }

    bool AnimeFilteredListPage::m_visible_func(const Gtk::TreeModel::const_iterator& iter) const
    {
        auto anime = iter->get_value(m_columns->anime);
        if (G_LIKELY(anime)) {
            auto status = m_status_combo->get_anime_status();
            if (G_UNLIKELY(status == AnimeStatus::NONE))
                return true;
            else
                return status == anime->status;
        } else {
            return true;
        }
    }

    void AnimeFilteredListPage::refresh()
    {
        auto complete_cb = [this](bool success) { 
            m_refresh_button->set_sensitive(true);
            m_refresh_button->show();
            m_progressbar->hide();
            if (success)
                on_mal_update();
        };

        auto prog_cb = [this](int_fast64_t bytes) {
            gint64 now = g_get_monotonic_time();
            if ((now - last_pulse) >= sixty_fps_in_us) {
                last_pulse = now;
                m_progressbar->set_text(std::to_string(bytes) + " bytes");
                m_progressbar->pulse();
            }
        };

        m_refresh_button->set_sensitive(false);
        m_refresh_button->hide();
        m_progressbar->show();
        m_mal->get_anime_list_async(prog_cb, complete_cb);
    }

    void AnimeFilteredListPage::on_mal_update()
    {
        using std::placeholders::_1;
        m_list_view->refresh_items(std::bind(&MAL::for_each_anime, m_mal, _1));
    }
}
