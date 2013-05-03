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
#include <glibmm/dispatcher.h>
#include <giomm/memoryinputstream.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeview.h>
#include <gtkmm/sizegroup.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/cellrenderercombo.h>
#include <sigc++/slot.h>
#include "manga.hpp"
#include "mal.hpp"
#include "malitem_list_view.hpp"
#include "increment_entry.hpp"
#include "fancy_label.hpp"

namespace MAL {

	class MangaStatusColumns final : public Gtk::TreeModel::ColumnRecord {
	public:
        Gtk::TreeModelColumn<Glib::ustring> text;
        Gtk::TreeModelColumn<MangaStatus> status;
		MangaStatusColumns() { add(text); add(status); };
	};

	class MangaStatusCellRendererCombo final : public Gtk::CellRendererCombo {
	public:
		MangaStatusCellRendererCombo();

	private:
		MangaStatusColumns columns;
		Glib::RefPtr<Gtk::ListStore> model;
	};

	class MangaStatusComboBox final : public Gtk::ComboBoxText {
	public:
		MangaStatusComboBox();
		MangaStatus get_manga_status() const;
	};

    class MangaModelColumnsBase : public virtual MALItemModelColumns
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring>           series_type;
        Gtk::TreeModelColumn<Glib::ustring>           series_status;
        Gtk::TreeModelColumn<gint>                    series_chapters;
        Gtk::TreeModelColumn<gint>                    series_volumes;
        Gtk::TreeModelColumn<std::shared_ptr<const Manga> > manga;

        MangaModelColumnsBase() {
            add(series_type);
            add(series_status);
            add(series_chapters);
            add(series_volumes);
            add(manga);
        }
    };

    class MangaModelColumnsEditable final : public MangaModelColumnsBase, public MALItemModelColumnsEditable
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> status;
        Gtk::TreeModelColumn<gint>          chapters;
        Gtk::TreeModelColumn<gint>          volumes;

        MangaModelColumnsEditable() {
            add(status);
            add(chapters);
            add(volumes);
        }
    };

    class MangaModelColumnsStatic final : public MangaModelColumnsBase, public MALItemModelColumnsStatic
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> status;

        MangaModelColumnsStatic() {
            add(status);
        }
    };

    class MangaListViewBase : public virtual MALItemListViewBase
    {
    public:
        MangaListViewBase(const std::shared_ptr<MAL>&,
                          const std::shared_ptr<MangaModelColumnsBase>&);

    protected:
        FancyCellRendererText *m_series_type_cellrenderer;
        FancyCellRendererText *m_series_status_cellrenderer;
        Gtk::TreeViewColumn   *m_series_type_column;
        Gtk::TreeViewColumn   *m_series_status_column;
        Gtk::TreeViewColumn   *m_series_chapters_column;
        Gtk::TreeViewColumn   *m_series_volumes_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new manga list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) override;
    };

    class MangaListViewStatic final : public MALItemListViewStatic, public MangaListViewBase
    {
    public:
        MangaListViewStatic(const std::shared_ptr<MAL>&,
                            const std::shared_ptr<MangaModelColumnsStatic>&);

    protected:
        Gtk::TreeViewColumn *m_status_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new manga list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row) override;

    private:
        void on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text);
    };

    class MangaListViewEditable final : public MALItemListViewEditable, public MangaListViewBase
    {
    public:
        MangaListViewEditable(const std::shared_ptr<MAL>&,
                              const std::shared_ptr<MangaModelColumnsEditable>&);

    protected:
        Gtk::TreeViewColumn *m_status_column;
        Gtk::TreeViewColumn *m_chapters_column;
        Gtk::TreeViewColumn *m_volumes_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new manga list from MAL)
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

    class MangaDetailViewBase: public virtual MALItemDetailViewBase {
    public:
        MangaDetailViewBase(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item) override;
        
    protected:
        Gtk::Grid                    *m_status_type_grid;
        Glib::RefPtr<Gtk::SizeGroup>  m_status_type_sizegroup;
        FancyLabel                   *m_series_status_label;
        FancyLabel                   *m_series_type_label;
        FancyLabel                    m_maximum_length_label;
    };

    class MangaDetailViewStatic final : public MALItemDetailViewStatic, public MangaDetailViewBase {
    public:
        MangaDetailViewStatic(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item) override;
        
    protected:
        Gtk::Grid                    *m_chapters_grid;
        Gtk::Grid                    *m_volumes_grid;
        Gtk::Label                   *m_series_chapters_label;
        Gtk::Label                   *m_series_volumes_label;
    };

    class MangaDetailViewEditable final : public MALItemDetailViewEditable, public MangaDetailViewBase {
    public:
        MangaDetailViewEditable(const std::shared_ptr<MAL>&,
                                const std::shared_ptr<MangaModelColumnsEditable>&,
                                const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>&);
        
        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item) override;
        
    protected:
        IncrementEntry      *m_chapters_entry;
        IncrementEntry      *m_volumes_entry;
		MangaStatusComboBox *m_manga_status_combo;

        /* Chain up */
        virtual bool update_list_model(const Gtk::TreeRow &row) override;
    };

    class MangaSearchListPage final : public MALItemListPage {
    public:
		MangaSearchListPage(const std::shared_ptr<MAL>&,
                            MangaListViewStatic*   list_view,
                            MangaDetailViewStatic* detail_view);

    protected:
        Gtk::Entry *m_search_entry;
		virtual void refresh() override;
        virtual void on_mal_update() override;
    };

    class MangaFilteredListPage final : public MALItemListPage {
    public:
		MangaFilteredListPage(const std::shared_ptr<MAL>& mal,
                              MangaListViewEditable*      list_view,
                              MangaDetailViewEditable*    detail_view);

    protected:
		virtual void refresh() override;
        virtual void on_mal_update() override;

    private:
        MangaListViewEditable* m_list_view;
        MangaDetailViewEditable* m_detail_view;
        MangaStatusComboBox *m_status_combo;

        bool m_filter_func(const std::shared_ptr<const MALItem>&) const;
    };
}
