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

#pragma once
#include <memory>
#include <algorithm>
#include <sigc++/slot.h>
#include <glibmm/dispatcher.h>
#include <giomm/memoryinputstream.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeview.h>
#include <gtkmm/sizegroup.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/cellrenderercombo.h>
#include <gtkmm/cssprovider.h>
#include "anime.hpp"
#include "mal.hpp"
#include "malitem_list_view.hpp"
#include "increment_entry.hpp"
#include "fancy_label.hpp"

namespace MAL {
	class AnimeStatusColumns final : public Gtk::TreeModel::ColumnRecord {
	public:
        Gtk::TreeModelColumn<Glib::ustring> text;
        Gtk::TreeModelColumn<AnimeStatus> status;
		AnimeStatusColumns() { add(text); add(status); };
	};

	class AnimeStatusCellRendererCombo final : public Gtk::CellRendererCombo {
	public:
		AnimeStatusCellRendererCombo();

	private:
		AnimeStatusColumns columns;
		Glib::RefPtr<Gtk::ListStore> model;
	};

	class AnimeStatusComboBox final : public Gtk::ComboBoxText {
	public:
		AnimeStatusComboBox();
		AnimeStatus get_anime_status() const;
	};

    class AnimeModelColumnsBase : public virtual MALItemModelColumns
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring>           series_type;
        Gtk::TreeModelColumn<Glib::ustring>           series_status;
        Gtk::TreeModelColumn<gint>                    series_episodes;
        Gtk::TreeModelColumn<std::shared_ptr<const Anime> > anime;

        AnimeModelColumnsBase() {
            add(series_type);
            add(series_status);
            add(series_episodes);
            add(anime);
        }
    };

    class AnimeModelColumnsEditable final : public AnimeModelColumnsBase, public MALItemModelColumnsEditable
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> status;
        Gtk::TreeModelColumn<gint>          episodes;

        AnimeModelColumnsEditable() {
            add(status);
            add(episodes);
        }
    };

    class AnimeModelColumnsStatic final : public AnimeModelColumnsBase, public MALItemModelColumnsStatic
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> status;

        AnimeModelColumnsStatic() {
            add(status);
        }
    };

    class AnimeListViewBase : public virtual MALItemListViewBase
    {
    public:
        AnimeListViewBase(const std::shared_ptr<MAL>&,
                          const std::shared_ptr<AnimeModelColumnsBase>&);

    protected:
        FancyCellRendererText *m_series_type_cellrenderer;
        FancyCellRendererText *m_series_status_cellrenderer;
        Gtk::TreeViewColumn   *m_series_type_column;
        Gtk::TreeViewColumn   *m_series_status_column;
        Gtk::TreeViewColumn   *m_series_episodes_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) override;
    };

    class AnimeListViewStatic final : public MALItemListViewStatic, public AnimeListViewBase
    {
    public:
        AnimeListViewStatic(const std::shared_ptr<MAL>&,
                            const std::shared_ptr<AnimeModelColumnsStatic>&);

    protected:
        Gtk::TreeViewColumn *m_status_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) override;

    private:
        void on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text);
    };

    class AnimeListViewEditable final : public MALItemListViewEditable, public AnimeListViewBase
    {
    public:
        AnimeListViewEditable(const std::shared_ptr<MAL>&,
                              const std::shared_ptr<AnimeModelColumnsEditable>&);

    protected:
        Gtk::TreeViewColumn *m_status_column;
        Gtk::TreeViewColumn *m_episodes_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) override;

        /* Chain up!
         * Called when the tree model was changed due to editing.
         * This method should set the appropriate field in item from
         * the tree row, then set the item back into the model as
         * well. 
         * Return true when the item value is different from the model value.
         */
        virtual bool model_changed_cb(std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) override;

        /* Called on main thread. Item should be transmitted back to MAL.net.
         */
        virtual void send_item_update(const std::shared_ptr<const MALItem>& item) override;

    private:
        void on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text);
    };

    class AnimeDetailViewBase: public virtual MALItemDetailViewBase {
    public:
        AnimeDetailViewBase(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item) override;
        
    protected:
        Gtk::Grid                    *m_status_type_grid;
        Glib::RefPtr<Gtk::SizeGroup>  m_status_type_sizegroup;
        FancyLabel                   *m_series_status_label;
        FancyLabel                   *m_series_type_label;
        FancyLabel                    m_maximum_length_label;
    };

    class AnimeDetailViewStatic final : public MALItemDetailViewStatic, public AnimeDetailViewBase {
    public:
        AnimeDetailViewStatic(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item) override;
        
    protected:
        Gtk::Grid                    *m_episodes_grid;
        Gtk::Label                   *m_series_episodes_label;
    };

    class AnimeDetailViewEditable final : public MALItemDetailViewEditable, public AnimeDetailViewBase {
    public:
        AnimeDetailViewEditable(const std::shared_ptr<MAL>&,
                                const std::shared_ptr<AnimeModelColumnsEditable>&,
                                const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>&);
        
        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item) override;
        
    protected:
        IncrementEntry      *m_episodes_entry;
		AnimeStatusComboBox *m_anime_status_combo;

        /* Chain up */
        virtual bool update_list_model(const Gtk::TreeRow &row) override;

    private:
        sigc::connection m_anime_status_changed_connection;
    };

    class AnimeSearchListPage final : public MALItemListPage {
    public:
		AnimeSearchListPage(const std::shared_ptr<MAL>&,
                            AnimeListViewStatic*   list_view,
                            AnimeDetailViewStatic* detail_view);

    protected:
        Gtk::Entry *m_search_entry;
		virtual void refresh() override;
        virtual void on_mal_update() override;

    };

    class AnimeFilteredListPage final : public MALItemListPage {
    public:
		AnimeFilteredListPage(const std::shared_ptr<MAL>& mal,
                              AnimeListViewEditable*      list_view,
                              AnimeDetailViewEditable*    detail_view);

    protected:
		virtual void refresh() override;
		virtual void on_mal_update() override;

    private:
        AnimeListViewEditable* m_list_view;
        AnimeDetailViewEditable* m_detail_view;
        AnimeStatusComboBox *m_status_combo;

        bool m_filter_func(const std::shared_ptr<const MALItem>&) const;
    };
}
