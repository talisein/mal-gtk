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

#include "manga_list_view.hpp"
#include <iostream>
#include <cstring>
#include <glibmm/markup.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>

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

    MangaDetailViewBase::MangaDetailViewBase(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        m_status_type_grid(Gtk::manage(new Gtk::Grid())),
        m_status_type_sizegroup(Gtk::SizeGroup::create(Gtk::SIZE_GROUP_BOTH)),
        m_series_status_label(Gtk::manage(new FancyLabel())),
        m_series_type_label(Gtk::manage(new FancyLabel()))
    {
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_TOP);
        m_grid_left->attach_next_to(*m_status_type_grid, *m_series_date_grid,
                               Gtk::POS_TOP, 1, 1);
        m_status_type_sizegroup->add_widget(*m_series_type_label);
        m_status_type_sizegroup->add_widget(*m_series_status_label);
        m_status_type_sizegroup->add_widget(m_maximum_length_label);
        m_maximum_length_label.set_label(to_string(NOTYETPUBLISHED));
        m_status_type_grid->attach(*m_series_type_label, 0, 0, 1, 1);
        m_status_type_grid->attach(*m_series_status_label, 1, 0, 1, 1);
        m_status_type_grid->set_column_spacing(10);
        m_status_type_grid->set_hexpand(false);
        m_series_status_label->set_vexpand(true);
        m_series_type_label->set_vexpand(true);
        m_series_status_label->set_hexpand(true);
        m_series_type_label->set_hexpand(true);
    }

    void MangaDetailViewBase::display_item(const std::shared_ptr<MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);
        auto manga = std::static_pointer_cast<Manga>(item);

        m_series_status_label->set_label(to_string(manga->series_status));
        m_series_type_label->set_label(to_string(manga->series_type));

        if (manga->series_synopsis.empty()) {
            m_synopsis_frame->show_all();
            m_synopsis_label->set_markup("Fetching...");
            m_mal->refresh_manga_async(manga, [this](const auto& new_manga) {
                    if (!new_manga) {
                        m_synopsis_label->set_markup("Failed to fetch synopsis.");
                        return;
                    }
                    if (m_item->series_itemdb_id != new_manga->series_itemdb_id) {
                        return;
                    }

                    m_synopsis_label->set_markup(Glib::Markup::escape_text(m_item->series_synopsis));
                });
        }
    }

    MangaDetailViewStatic::MangaDetailViewStatic(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        MALItemDetailViewStatic(mal),
        MangaDetailViewBase(mal),
        m_chapters_grid(Gtk::manage(new Gtk::Grid())),
        m_volumes_grid(Gtk::manage(new Gtk::Grid())),
        m_series_chapters_label(Gtk::manage(new Gtk::Label())),
        m_series_volumes_label(Gtk::manage(new Gtk::Label()))
    {
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_volumes_grid, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_volumes_grid->attach(*m_series_volumes_label, 0, 0, 1, 1);
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_chapters_grid, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_chapters_grid->attach(*m_series_chapters_label, 0, 0, 1, 1);
    }

    void MangaDetailViewStatic::display_item(const std::shared_ptr<MALItem>& item)
    {
        MALItemDetailViewStatic::display_item(item);
        MangaDetailViewBase::display_item(item);

        auto manga = std::static_pointer_cast<Manga>(item);

        if (manga->series_chapters > 0) {
            m_series_chapters_label->set_text(std::to_string(manga->series_chapters) + ((manga->series_chapters>1)?" Chapters":" Chapter"));
            m_series_chapters_label->show();
        } else {
            m_series_chapters_label->hide();
        }

        if (manga->series_volumes > 0) {
            m_series_volumes_label->set_text(std::to_string(manga->series_volumes) + ((manga->series_volumes>1)?" Volumes":" Volume"));
            m_series_volumes_label->show();
        } else {
            m_series_volumes_label->hide();
        }
    }

    MangaDetailViewEditable::MangaDetailViewEditable(const std::shared_ptr<MAL>& mal,
                                                     const std::shared_ptr<MangaModelColumnsEditable>& columns,
                                                     const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>& slot) :
        MALItemDetailViewBase(mal),
        MALItemDetailViewEditable(mal, columns, slot),
        MangaDetailViewBase(mal),
        m_chapters_entry(Gtk::manage(new IncrementEntry())),
        m_volumes_entry(Gtk::manage(new IncrementEntry())),
        m_manga_status_combo(Gtk::manage(new MangaStatusComboBox()))
    {
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_chapters_entry, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_volumes_entry, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_grid_left->insert_next_to(*m_series_date_grid, Gtk::POS_BOTTOM);
        m_grid_left->attach_next_to(*m_manga_status_combo, *m_series_date_grid, Gtk::POS_BOTTOM, 1, 1);
        m_chapters_entry->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailViewEditable::notify_list_model));
        m_volumes_entry->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailViewEditable::notify_list_model));
        m_manga_status_changed_connection = m_manga_status_combo->signal_changed().connect(sigc::mem_fun(*this, &MangaDetailViewEditable::notify_list_model));
        m_reconsuming_label->set_text("Rereading:");
        m_fansub_group_label->set_text("Scan Group:");
        m_downloaded_items_entry->set_label("Downloaded Chapters");
        m_times_consumed_entry->set_label("Times Read");
        m_reconsume_value_label->set_text("Reread Value:");
    }

    void MangaDetailViewEditable::display_item(const std::shared_ptr<MALItem>& item)
    {
        m_manga_status_changed_connection.block();
        MALItemDetailViewEditable::display_item(item);
        MangaDetailViewBase::display_item(item);
        auto manga = std::static_pointer_cast<Manga>(item);
        m_chapters_entry->set_label("/ " + std::to_string(manga->series_chapters) + ((manga->series_chapters!=1)?" Chapters":" Chapter"));
        m_volumes_entry->set_label("/ " + std::to_string(manga->series_volumes) + ((manga->series_volumes!=1)?" Volumes":" Volume"));
        m_chapters_entry->set_entry_text(std::to_string(manga->chapters));
        m_volumes_entry->set_entry_text(std::to_string(manga->volumes));
		m_manga_status_combo->set_active_text(to_string(manga->status));

        if (manga->status == MANGACOMPLETED) {
            m_date_end_entry->set_sensitive(true);
            m_times_consumed_entry->show();
        } else {
            m_date_end_entry->set_sensitive(false);
            m_times_consumed_entry->hide();
        }

        m_manga_status_changed_connection.unblock();
    }

    bool MangaDetailViewEditable::update_list_model(const Gtk::TreeRow &row)
    {
        if (row.get_value(m_columns->item)->series_itemdb_id != m_item->series_itemdb_id)
            return false;
        MALItemDetailViewEditable::update_list_model(row);
        auto columns = std::dynamic_pointer_cast<MangaModelColumnsEditable>(m_notify_columns);

        try {
            auto chapters = std::stoi(m_chapters_entry->get_entry_text());
            if (chapters != row.get_value(columns->chapters))
                row.set_value(columns->chapters, chapters);
        } catch (const std::exception& e) {
            auto manga = std::static_pointer_cast<Manga>(m_item);
            m_chapters_entry->set_entry_text(std::to_string(manga->chapters));
        }

        try {
            auto volumes = std::stoi(m_volumes_entry->get_entry_text());
            if (volumes != row.get_value(columns->volumes))
                row.set_value(columns->volumes, volumes);
        } catch (const std::exception& e) {
            auto manga = std::static_pointer_cast<Manga>(m_item);
            m_volumes_entry->set_entry_text(std::to_string(manga->volumes));
        }

        auto status = m_manga_status_combo->get_manga_status();
        if (status != manga_status_from_string(row.get_value(columns->status)))
            row.set_value(columns->status, Glib::ustring(to_string(status)));

        return true;
    }

    MangaListViewBase::MangaListViewBase(const std::shared_ptr<MAL>& mal,
                                         const std::shared_ptr<MangaModelColumnsBase>& columns) :
        MALItemListViewBase(mal, columns),
        m_series_type_cellrenderer(Gtk::manage(new FancyCellRendererText())),
        m_series_status_cellrenderer(Gtk::manage(new FancyCellRendererText())),
        m_series_type_column(Gtk::manage(new Gtk::TreeViewColumn("Type", *m_series_type_cellrenderer))),
        m_series_status_column(Gtk::manage(new Gtk::TreeViewColumn("Pub. Status", *m_series_status_cellrenderer))),
        m_series_chapters_column(Gtk::manage(new Gtk::TreeViewColumn("Chs.", columns->series_chapters))),
        m_series_volumes_column(Gtk::manage(new Gtk::TreeViewColumn("Vols.", columns->series_volumes)))
    {
        m_series_type_column->add_attribute(m_series_type_cellrenderer->property_text(), columns->series_type);
        m_series_status_column->add_attribute(m_series_status_cellrenderer->property_text(), columns->series_status);
        m_treeview->append_column(*m_series_type_column);
        m_treeview->append_column(*m_series_chapters_column);
        m_treeview->append_column(*m_series_volumes_column);
        m_treeview->append_column(*m_series_status_column);
        m_treeview->move_column_after(*m_series_status_column, *m_title_column);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new manga list from MAL)
     * This method should take data from the item and put it on the row
     */
    void MangaListViewBase::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<MangaModelColumnsBase>(m_columns);
        auto manga = std::static_pointer_cast<Manga>(item);

        row.set_value(columns->series_type, Glib::ustring(to_string(manga->series_type)));
        row.set_value(columns->series_status, Glib::ustring(to_string(manga->series_status)));
        row.set_value(columns->series_chapters, static_cast<int>(manga->series_chapters));
        row.set_value(columns->series_volumes, static_cast<int>(manga->series_volumes));
        row.set_value(columns->manga, manga);
    }

    MangaListViewStatic::MangaListViewStatic(const std::shared_ptr<MAL>& mal,
                                             const std::shared_ptr<MangaModelColumnsStatic>& columns) :
        MALItemListViewBase(mal, columns),
        MALItemListViewStatic(mal, columns),
        MangaListViewBase(mal, columns),
        m_status_column(Gtk::manage(new Gtk::TreeViewColumn("Viewing Status")))
    {
		auto status_cellrenderer = Gtk::manage(new MangaStatusCellRendererCombo());
		m_status_column->pack_start(*status_cellrenderer);
		m_status_column->add_attribute(status_cellrenderer->property_text(), columns->status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &MangaListViewStatic::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;
		m_treeview->append_column(*m_status_column);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new manga list from MAL)
     * This method should take data from the item and put it on the row
     */
    void MangaListViewStatic::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        MangaListViewBase::refresh_item_cb(item, row);
        MALItemListViewStatic::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<MangaModelColumnsStatic>(m_columns);
        auto manga = std::static_pointer_cast<Manga>(item);
        auto status = manga->status;
        if (status == MANGASTATUS_INVALID || status == 0)
            row.set_value(columns->status, Glib::ustring("Add To My Manga List..."));
        else
            row.set_value(columns->status, Glib::ustring(to_string(manga->status)));
    }

	void MangaListViewStatic::on_status_cr_changed(const Glib::ustring& path,
                                                   const Glib::ustring& new_text)
    {
        auto columns = std::dynamic_pointer_cast<MangaModelColumnsStatic>(m_columns);
		Gtk::TreeModel::iterator iter = m_model->get_iter(path);

		if (iter) {
			auto status = manga_status_from_string(new_text);
            auto manga = iter->get_value(columns->manga);

			if (status != manga->status) {
                auto new_manga = std::static_pointer_cast<Manga>(manga->clone());
				new_manga->status = status;
                new_manga->score = 0.0f;
                if (new_manga->status == MANGACOMPLETED) {
                    new_manga->chapters = new_manga->series_chapters;
                    new_manga->volumes = new_manga->series_volumes;
                } else {
                    new_manga->chapters = 0;
                    new_manga->volumes = 0;
                }

                if (new_manga->status != MANGASTATUS_INVALID) {
                    m_mal->add_manga_async(*new_manga);

                    iter->set_value(columns->status, Glib::ustring(to_string(status)));
                    iter->set_value(columns->manga, new_manga);
                }
            }
		}
	}

    MangaListViewEditable::MangaListViewEditable(const std::shared_ptr<MAL>& mal,
                                                 const std::shared_ptr<MangaModelColumnsEditable>& columns) :
        MALItemListViewBase(mal, columns),
        MALItemListViewEditable(mal, columns),
        MangaListViewBase(mal, columns),
        m_status_column(Gtk::manage(new Gtk::TreeViewColumn("Viewing Status")))
    {
		auto status_cellrenderer = Gtk::manage(new MangaStatusCellRendererCombo());
		m_status_column->pack_start(*status_cellrenderer);
		m_status_column->add_attribute(status_cellrenderer->property_text(), columns->status);
		status_cellrenderer->signal_edited().connect(sigc::mem_fun(*this, &MangaListViewEditable::on_status_cr_changed));
		status_cellrenderer->property_editable() = true;
		m_treeview->append_column(*m_status_column);
        auto num = m_treeview->append_column_numeric_editable("Read", columns->chapters, "%d");
        m_chapters_column = m_treeview->get_column(num - 1);
        num = m_treeview->append_column_numeric_editable("Read", columns->volumes, "%d");
        m_volumes_column = m_treeview->get_column(num - 1);
        m_treeview->move_column_after(*m_chapters_column, *m_series_type_column);
        m_treeview->move_column_after(*m_volumes_column, *m_series_chapters_column);
        m_mal->signal_manga_detailed.connect(sigc::mem_fun(this, &MangaListViewEditable::on_detailed_item));
    }

    void MangaListViewEditable::get_details_for_item(const std::shared_ptr<MALItem>& item)
    {
        auto manga = std::static_pointer_cast<Manga>(item);
        if (!manga->has_details)
            m_mal->get_manga_details_async(manga);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new manga list from MAL)
     * This method should take data from the item and put it on the row
     */
    void MangaListViewEditable::refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        MangaListViewBase::refresh_item_cb(item, row);
        MALItemListViewEditable::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<MangaModelColumnsEditable>(m_columns);
        auto manga = std::static_pointer_cast<Manga>(item);

        row.set_value(columns->status, Glib::ustring(to_string(manga->status)));
        row.set_value(columns->chapters, static_cast<int>(manga->chapters));
        row.set_value(columns->volumes, static_cast<int>(manga->volumes));
    }

    /* Chain up!
     * Called when the tree model was changed due to editing.
     * This method should set the appropriate field in item from
     * the tree row, then set the item back into the model as
     * well.
     * Return true when the item value is different from the model value.
     */
    bool MangaListViewEditable::model_changed_cb(std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row)
    {
        bool is_changed = false;
        auto mal_changed = MALItemListViewEditable::model_changed_cb(item, row);

        auto const columns = std::dynamic_pointer_cast<MangaModelColumnsEditable>(m_columns);
        auto manga = std::static_pointer_cast<Manga>(item);
        auto new_manga = std::static_pointer_cast<Manga>(manga->clone());
        if (mal_changed)
            row.set_value(columns->manga, manga);

        auto const chapters = row.get_value(columns->chapters);
        if (chapters != manga->chapters) {
            is_changed = true;
            new_manga->chapters = chapters;
            item = manga = new_manga;
            row.set_value(columns->item, item);
            row.set_value(columns->manga, manga);
        }

        auto const volumes = row.get_value(columns->volumes);
        if (volumes != manga->volumes) {
            is_changed = true;
            new_manga->volumes = volumes;
            item = manga = new_manga;
            row.set_value(columns->item, item);
            row.set_value(columns->manga, manga);
        }

        auto const status = manga_status_from_string(row.get_value(columns->status));
        if (status != manga->status) {
            is_changed = true;
            new_manga->status = status;
            item = manga = new_manga;
            row.set_value(columns->item, item);
            row.set_value(columns->manga, manga);
        }

        return is_changed || mal_changed;
    }

    /* Called on main thread. Item should be transmitted back to MAL.net.
     */
    void MangaListViewEditable::send_item_update(const std::shared_ptr<MALItem>& item)
    {
        auto manga = std::static_pointer_cast<Manga>(item);
        m_mal->update_manga_async(manga);
    }

    void MangaListViewEditable::on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text)
    {
        auto columns = std::dynamic_pointer_cast<MangaModelColumnsEditable>(m_columns);
		Gtk::TreeModel::iterator iter = m_model->get_iter(path);

		if (iter) {
			auto status = manga_status_from_string(new_text);
            auto manga = iter->get_value(columns->manga);
            auto new_manga = std::static_pointer_cast<Manga>(manga->clone());

			if (status != manga->status) {
				iter->set_value(columns->status, Glib::ustring(to_string(status)));
				new_manga->status = status;
                manga = new_manga;
				iter->set_value(columns->manga, manga);

                if (status != MANGASTATUS_INVALID) {
                    send_item_update(new_manga);
                }
            }
		}
    }

    MangaSearchListPage::MangaSearchListPage(const std::shared_ptr<MAL>& mal,
                                             MangaListViewStatic*        list_view,
                                             MangaDetailViewStatic*      detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_search_entry(Gtk::manage(new Gtk::Entry()))
    {
        m_button_row->attach(*m_search_entry, -1, 0, 1, 1);
        m_search_entry->set_hexpand(true);
        m_search_entry->set_activates_default(true);
        m_search_entry->show();

		m_refresh_button->set_image_from_icon_name("edit-find");
		m_refresh_button->set_tooltip_text("Search myanimelist.net for manga that maches the entered terms.");
        mal->signal_manga_search_completed.connect(sigc::mem_fun(*this, &MangaSearchListPage::on_mal_update));
    }

    void MangaSearchListPage::refresh()
    {
        m_mal->search_manga_async(m_search_entry->get_text());
    }

    void MangaSearchListPage::on_mal_update()
    {
        using std::placeholders::_1;
        m_list_view->refresh_items(std::bind(&MAL::for_each_manga_search_result, m_mal, _1));
    }

    MangaFilteredListPage::MangaFilteredListPage(const std::shared_ptr<MAL>& mal,
                                                 const std::shared_ptr<MangaModelColumnsEditable>& columns,
                                                 MangaListViewEditable*      list_view,
                                                 MangaDetailViewEditable*    detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_columns(columns),
        m_list_view(list_view),
        m_detail_view(detail_view),
        m_status_combo(Gtk::manage(new MangaStatusComboBox()))
    {
        m_list_view->set_visible_func(sigc::mem_fun(this, &MangaFilteredListPage::m_visible_func));
        m_status_combo->signal_changed().connect(sigc::mem_fun(static_cast<MALItemListViewBase*>(m_list_view), &MALItemListViewEditable::refilter));
        auto label = Gtk::manage(new Gtk::Label("Filter: "));
        m_button_row->attach(*m_status_combo, -1, 0, 1, 1);
        m_button_row->attach(*label, -2, 0, 1, 1);
        m_status_combo->set_hexpand(true);
        m_status_combo->set_active_text(to_string(READING));
        m_status_combo->show();
        mal->signal_manga_added.connect(sigc::mem_fun(*this, &MangaFilteredListPage::on_mal_update));
    }

    bool MangaFilteredListPage::m_filter_func(const std::shared_ptr<MALItem>& item) const
    {
        auto manga = std::static_pointer_cast<Manga>(item);
        return manga->status == m_status_combo->get_manga_status();
    }

    bool MangaFilteredListPage::m_visible_func(const Gtk::TreeModel::const_iterator& iter) const
    {
        auto manga = iter->get_value(m_columns->manga);
        if (manga) {
            return m_status_combo->get_manga_status() == manga->status;
        } else {
            return true;
        }
    }

    void MangaFilteredListPage::refresh()
    {
		m_mal->get_manga_list_async();
    }

    void MangaFilteredListPage::on_mal_update()
    {
        using std::placeholders::_1;
        m_list_view->refresh_items(std::bind(&MAL::for_each_manga, m_mal, _1));
    }
}
