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

    MangaDetailViewBase::MangaDetailViewBase(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        m_status_type_grid(Gtk::manage(new Gtk::Grid())),
        m_status_type_sizegroup(Gtk::SizeGroup::create(Gtk::SIZE_GROUP_BOTH)),
        m_series_status_label(Gtk::manage(new FancyLabel())),
        m_series_type_label(Gtk::manage(new FancyLabel()))
    {
        m_grid->attach(*m_status_type_grid, 0, -1, 1, 1);
        m_status_type_sizegroup->add_widget(*m_series_type_label);
        m_status_type_sizegroup->add_widget(*m_series_status_label);
        m_status_type_sizegroup->add_widget(m_maximum_length_label);
        m_maximum_length_label.set_label(to_string(NOTYETPUBLISHED));
        m_status_type_grid->attach(*m_series_type_label, 0, 0, 1, 1);
        m_status_type_grid->attach(*m_series_status_label, 1, 0, 1, 1);
        m_status_type_grid->set_column_spacing(10);
    }

    void MangaDetailViewBase::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);
        auto manga = std::static_pointer_cast<const Manga>(item);

        m_series_status_label->set_label(to_string(manga->series_status));
        m_series_type_label->set_label(to_string(manga->series_type));
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
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_volumes_grid, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_volumes_grid->attach(*m_series_volumes_label, 0, 0, 1, 1);
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_chapters_grid, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_chapters_grid->attach(*m_series_chapters_label, 0, 0, 1, 1);
    }

    void MangaDetailViewStatic::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewStatic::display_item(item);
        MangaDetailViewBase::display_item(item);

        auto manga = std::static_pointer_cast<const Manga>(item);

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
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_chapters_entry, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_volumes_entry, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_manga_status_combo, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_chapters_entry->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailViewEditable::notify_list_model));
        m_volumes_entry->signal_activate().connect(sigc::mem_fun(*this, &MangaDetailViewEditable::notify_list_model));
        m_manga_status_combo->signal_changed().connect(sigc::mem_fun(*this, &MangaDetailViewEditable::notify_list_model));
    }

    void MangaDetailViewEditable::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewEditable::display_item(item);
        MangaDetailViewBase::display_item(item);
        auto manga = std::static_pointer_cast<const Manga>(item);
        m_chapters_entry->set_label("/ " + std::to_string(manga->series_chapters) + ((manga->series_chapters!=1)?" Chapters":" Chapter"));
        m_volumes_entry->set_label("/ " + std::to_string(manga->series_volumes) + ((manga->series_volumes!=1)?" Volumes":" Volume"));
        m_chapters_entry->set_entry_text(std::to_string(manga->chapters));
        m_volumes_entry->set_entry_text(std::to_string(manga->volumes));
		m_manga_status_combo->set_active_text(to_string(manga->status));
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
        } catch (std::exception e) {
            auto manga = std::static_pointer_cast<const Manga>(m_item);
            m_chapters_entry->set_entry_text(std::to_string(manga->chapters));
        }

        try {
            auto volumes = std::stoi(m_volumes_entry->get_entry_text());
            if (volumes != row.get_value(columns->volumes))
                row.set_value(columns->volumes, volumes);
        } catch (std::exception e) {
            auto manga = std::static_pointer_cast<const Manga>(m_item);
            m_volumes_entry->set_entry_text(std::to_string(manga->volumes));
        }

        auto status = m_manga_status_combo->get_manga_status();
        if (status != row.get_value(columns->status))
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
    void MangaListViewBase::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<MangaModelColumnsBase>(m_columns);
        auto manga = std::static_pointer_cast<const Manga>(item);

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
    void MangaListViewStatic::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        MangaListViewBase::refresh_item_cb(item, row);
        MALItemListViewStatic::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<MangaModelColumnsStatic>(m_columns);
        auto manga = std::static_pointer_cast<const Manga>(item);
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
				iter->set_value(columns->status, Glib::ustring(to_string(status)));
                auto new_manga = std::static_pointer_cast<Manga>(manga->clone());
				new_manga->status = status;
                manga = new_manga;
				iter->set_value(columns->manga, manga);

                new_manga->score = 0.0f;
                if (new_manga->status == MANGACOMPLETED) {
                    new_manga->chapters = manga->series_chapters;
                    new_manga->volumes = manga->series_volumes;
                } else {
                    new_manga->chapters = 0;
                    new_manga->volumes = 0;
                }
                if (new_manga->status != MANGASTATUS_INVALID) {
                    std::thread t(std::bind(&MAL::add_manga_sync, std::ref(m_mal), std::ref(*new_manga)));
                    t.detach();
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
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new manga list from MAL)
     * This method should take data from the item and put it on the row
     */
    void MangaListViewEditable::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        MangaListViewBase::refresh_item_cb(item, row);
        MALItemListViewEditable::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<MangaModelColumnsEditable>(m_columns);
        auto manga = std::static_pointer_cast<const Manga>(item);

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
    bool MangaListViewEditable::model_changed_cb(std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        bool is_changed = false;
        auto mal_changed = MALItemListViewEditable::model_changed_cb(item, row);
        
        auto const columns = std::dynamic_pointer_cast<MangaModelColumnsEditable>(m_columns);
        auto manga = std::static_pointer_cast<const Manga>(item);
        auto new_manga = std::static_pointer_cast<Manga>(manga->clone());

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
    void MangaListViewEditable::send_item_update(const std::shared_ptr<const MALItem>& item)
    {
        auto manga = std::static_pointer_cast<const Manga>(item);
        std::thread t(std::bind(&MangaListViewEditable::send_manga_update, this, manga));
        t.detach();
    }

    void MangaListViewEditable::send_manga_update(const std::shared_ptr<const Manga>& manga)
    {
        auto success = m_mal->update_manga_sync(*manga);
        if (success) {
            auto iter = std::find_if(std::begin(m_items),
                                     std::end(m_items),
                                     [&manga](const std::shared_ptr<const MALItem>& a) {
                                         return manga->series_itemdb_id == a->series_itemdb_id;
                                     });
            if (iter != std::end(m_items)) {
                // Since we're off the main thread, do a threadsafe iter_swap
                std::list<std::shared_ptr<const MALItem>> l(1, manga);
                std::iter_swap(iter, std::begin(l));
            }
        }
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
                    std::thread t(std::bind(&MangaListViewEditable::send_item_update,
                                            this,
                                            manga));
                    t.detach();
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

		auto icon = m_refresh_button->render_icon_pixbuf(Gtk::Stock::FIND, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		m_refresh_button->set_image(*image);
		m_refresh_button->set_tooltip_text("Search myanimelist.net for manga that maches the entered terms.");
    }

    void MangaSearchListPage::refresh()
    {
        auto list = m_mal->search_manga_sync(m_search_entry->get_text());
        std::list<std::shared_ptr<const MALItem> > item_list;
        std::move(std::begin(list), std::end(list), std::back_inserter(item_list));
		m_list_view->set_item_list(std::move(item_list));
    }

    MangaFilteredListPage::MangaFilteredListPage(const std::shared_ptr<MAL>& mal,
                                                 MangaListViewEditable*      list_view,
                                                 MangaDetailViewEditable*    detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_list_view(list_view),
        m_detail_view(detail_view),
        m_status_combo(Gtk::manage(new MangaStatusComboBox()))
    {
        m_status_combo->signal_changed().connect(sigc::mem_fun(*dynamic_cast<MALItemListViewBase*>(m_list_view), &MALItemListViewBase::refilter));
        auto label = Gtk::manage(new Gtk::Label("Filter: "));
        m_button_row->attach(*m_status_combo, -1, 0, 1, 1);
        m_button_row->attach(*label, -2, 0, 1, 1);
        m_status_combo->set_hexpand(true);
        m_status_combo->set_active_text(to_string(READING));
        m_status_combo->show();
        list_view->set_filter_func(sigc::mem_fun(*this, &MangaFilteredListPage::m_filter_func));

        refresh_async();
    }

    bool MangaFilteredListPage::m_filter_func(const std::shared_ptr<const MALItem>& item) const
    {
        auto manga = std::static_pointer_cast<const Manga>(item);
        return manga->status == m_status_combo->get_manga_status();
    }

    void MangaFilteredListPage::refresh()
    {
		auto list = m_mal->get_manga_list_sync();
        std::list<std::shared_ptr<const MALItem> > item_list;
        std::move(std::begin(list), std::end(list), std::back_inserter(item_list));
		m_list_view->set_item_list(std::move(item_list));
    }
}
