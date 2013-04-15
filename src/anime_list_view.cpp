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
        m_series_status_label(Gtk::manage(new Gtk::Label())),
        m_series_type_label(Gtk::manage(new Gtk::Label()))
    {
        m_grid->attach(*m_status_type_grid, 0, -1, 1, 1);
        m_status_type_sizegroup->add_widget(*m_series_type_label);
        m_status_type_sizegroup->add_widget(*m_series_status_label);
        m_status_type_grid->attach(*m_series_type_label, 0, 0, 1, 1);
        m_status_type_grid->attach(*m_series_status_label, 1, 0, 1, 1);
        m_status_type_grid->set_column_spacing(10);
    }

    void AnimeDetailViewBase::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewBase::display_item(item);
        auto anime = std::static_pointer_cast<const Anime>(item);

        m_series_status_label->set_text(to_string(anime->series_status));
        m_series_type_label->set_text(to_string(anime->series_type));
    }

    AnimeDetailViewStatic::AnimeDetailViewStatic(const std::shared_ptr<MAL>& mal) :
        MALItemDetailViewBase(mal),
        MALItemDetailViewStatic(mal),
        AnimeDetailViewBase(mal),
        m_episodes_grid(Gtk::manage(new Gtk::Grid())),
        m_series_episodes_label(Gtk::manage(new Gtk::Label()))
    {
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_episodes_grid, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_episodes_grid->attach(*m_series_episodes_label, 0, 0, 1, 1);
    }

    void AnimeDetailViewStatic::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewStatic::display_item(item);
        AnimeDetailViewBase::display_item(item);

        auto anime = std::static_pointer_cast<const Anime>(item);
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
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_episodes_entry, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_grid->insert_next_to(*m_status_type_grid, Gtk::POS_BOTTOM);
        m_grid->attach_next_to(*m_anime_status_combo, *m_status_type_grid, Gtk::POS_BOTTOM, 1, 1);
        m_episodes_entry->signal_activate().connect(sigc::mem_fun(*this, &AnimeDetailViewEditable::notify_list_model));
        m_anime_status_combo->signal_changed().connect(sigc::mem_fun(*this, &AnimeDetailViewEditable::notify_list_model));
    }

    void AnimeDetailViewEditable::display_item(const std::shared_ptr<const MALItem>& item)
    {
        MALItemDetailViewEditable::display_item(item);
        AnimeDetailViewBase::display_item(item);
        auto anime = std::static_pointer_cast<const Anime>(item);
        m_episodes_entry->set_label("/ " + std::to_string(anime->series_episodes) + " Episodes");
        m_episodes_entry->set_entry_text(std::to_string(anime->episodes));
		m_anime_status_combo->set_active_text(to_string(anime->status));
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
            auto anime = std::static_pointer_cast<const Anime>(m_item);
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
        m_series_type_column(Gtk::manage(new Gtk::TreeViewColumn("Type", columns->series_type))),
        m_series_status_column(Gtk::manage(new Gtk::TreeViewColumn("Airing Status", columns->series_status))),
        m_series_episodes_column(Gtk::manage(new Gtk::TreeViewColumn("Eps.", columns->series_episodes)))
    {
        m_treeview->append_column(*m_series_type_column);
        m_treeview->append_column(*m_series_episodes_column);
        m_treeview->append_column(*m_series_status_column);
        m_treeview->move_column_after(*m_series_status_column, *m_title_column);
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new anime list from MAL)
     * This method should take data from the item and put it on the row
     */
    void AnimeListViewBase::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        MALItemListViewBase::refresh_item_cb(item, row);
        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsBase>(m_columns);
        auto anime = std::static_pointer_cast<const Anime>(item);

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
    void AnimeListViewStatic::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        AnimeListViewBase::refresh_item_cb(item, row);
        MALItemListViewStatic::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsStatic>(m_columns);
        auto anime = std::static_pointer_cast<const Anime>(item);
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
                    std::thread t(std::bind(&MAL::add_anime_sync, m_mal, *new_anime));
                    t.detach();
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
    }

    /* Chain up!
     * Called when m_items has changed (We have fetched a new anime list from MAL)
     * This method should take data from the item and put it on the row
     */
    void AnimeListViewEditable::refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        AnimeListViewBase::refresh_item_cb(item, row);
        MALItemListViewEditable::refresh_item_cb(item, row);

        auto columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_columns);
        auto anime = std::static_pointer_cast<const Anime>(item);

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
    bool AnimeListViewEditable::model_changed_cb(std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row)
    {
        bool is_changed = false;
        auto mal_changed = MALItemListViewEditable::model_changed_cb(item, row);
        
        auto const columns = std::dynamic_pointer_cast<AnimeModelColumnsEditable>(m_columns);
        auto anime = std::static_pointer_cast<const Anime>(item);
        auto new_anime = std::static_pointer_cast<Anime>(anime->clone());

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
    void AnimeListViewEditable::send_item_update(const std::shared_ptr<const MALItem>& item)
    {
        auto anime = std::static_pointer_cast<const Anime>(item);
        std::thread t(&AnimeListViewEditable::send_anime_update, this, anime);
        t.detach();
    }

    void AnimeListViewEditable::send_anime_update(const std::shared_ptr<const Anime>& anime)
    {
        auto success = m_mal->update_anime_sync(*anime);
        if (success) {
            auto iter = std::find_if(std::begin(m_items),
                                     std::end(m_items),
                                     [&anime](const std::shared_ptr<const MALItem>& a) {
                                         return anime->series_itemdb_id == a->series_itemdb_id;
                                     });
            if (iter != std::end(m_items)) {
                // Since we're off the main thread, do a threadsafe iter_swap
                std::list<std::shared_ptr<const MALItem>> l(1, anime);
                std::iter_swap(iter, std::begin(l));
            }
        }
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
                    std::thread t(std::bind(&AnimeListViewEditable::send_item_update,
                                            this,
                                            anime));
                    t.detach();
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
    }

    void AnimeSearchListPage::refresh()
    {
        auto list = m_mal->search_anime_sync(m_search_entry->get_text());
        std::list<std::shared_ptr<const MALItem> > item_list;
        std::move(std::begin(list), std::end(list), std::back_inserter(item_list));
		m_list_view->set_item_list(std::move(item_list));
    }

    AnimeFilteredListPage::AnimeFilteredListPage(const std::shared_ptr<MAL>& mal,
                                                 AnimeListViewEditable*      list_view,
                                                 AnimeDetailViewEditable*    detail_view) :
        MALItemListPage(mal, list_view, detail_view),
        m_list_view(list_view),
        m_detail_view(detail_view),
        m_status_combo(Gtk::manage(new AnimeStatusComboBox()))
    {
        m_status_combo->signal_changed().connect(sigc::mem_fun(*dynamic_cast<MALItemListViewBase*>(m_list_view), &MALItemListViewBase::refilter));
        auto label = Gtk::manage(new Gtk::Label("Filter: "));
        m_button_row->attach(*m_status_combo, -1, 0, 1, 1);
        m_button_row->attach(*label, -2, 0, 1, 1);
        m_status_combo->set_hexpand(true);
        m_status_combo->set_active_text(to_string(WATCHING));
        m_status_combo->show();
        list_view->set_filter_func(sigc::mem_fun(*this, &AnimeFilteredListPage::m_filter_func));

        refresh_async();
    }

    bool AnimeFilteredListPage::m_filter_func(const std::shared_ptr<const MALItem>& item) const
    {
        auto anime = std::static_pointer_cast<const Anime>(item);
        return anime->status == m_status_combo->get_anime_status();
    }

    void AnimeFilteredListPage::refresh()
    {
		auto list = m_mal->get_anime_list_sync();
        std::list<std::shared_ptr<const MALItem> > item_list;
        std::move(std::begin(list), std::end(list), std::back_inserter(item_list));
		m_list_view->set_item_list(std::move(item_list));
    }
}
