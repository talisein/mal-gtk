#pragma once
#include <memory>
#include <giomm/memoryinputstream.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeview.h>
#include <gtkmm/sizegroup.h>
#include <sigc++/slot.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/cellrenderercombo.h>
#include "anime.hpp"
#include "mal.hpp"
#include "malitem_list_view.hpp"

namespace MAL {

	class AnimeStatusColumns : public Gtk::TreeModel::ColumnRecord {
	public:
        Gtk::TreeModelColumn<Glib::ustring> text;
        Gtk::TreeModelColumn<AnimeStatus> status;
		AnimeStatusColumns() { add(text); add(status); };
	};

	class AnimeStatusCellRendererCombo : public Gtk::CellRendererCombo {
	public:
		AnimeStatusCellRendererCombo();

	private:
		AnimeStatusColumns columns;
		Glib::RefPtr<Gtk::ListStore> model;
	};

	class AnimeStatusComboBox : public Gtk::ComboBoxText {
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
        Gtk::TreeModelColumn<std::shared_ptr<Anime> > anime;

        AnimeModelColumnsBase() {
            add(series_type);
            add(series_status);
            add(series_episodes);
            add(anime);
        }
    };

    class AnimeModelColumnsEditable : public AnimeModelColumnsBase, public MALItemModelColumnsEditable
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> status;
        Gtk::TreeModelColumn<gint>          episodes;

        AnimeModelColumnsEditable() {
            add(status);
            add(episodes);
        }
    };

    class AnimeModelColumnsStatic : public AnimeModelColumnsBase, public MALItemModelColumnsStatic
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
        Gtk::TreeViewColumn *m_series_type_column;
        Gtk::TreeViewColumn *m_series_status_column;
        Gtk::TreeViewColumn *m_series_episodes_column;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row);
    };

    class AnimeListViewStatic : public MALItemListViewStatic, public AnimeListViewBase
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
        virtual void refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row);

    private:
        void on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text);
    };

    class AnimeListViewEditable : public MALItemListViewEditable, public AnimeListViewBase
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
        virtual void refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row);

        /* Chain up!
         * Called when the tree model was changed due to editing.
         * This method should set the appropriate field in item from
         * the tree row, then set the item back into the model as
         * well. 
         * Return true when the item value is different from the model value.
         */
        virtual bool model_changed_cb(std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row);

        /* Called on main thread. Item should be transmitted back to MAL.net.
         */
        virtual void send_item_update(const std::shared_ptr<MALItem>& item);

    private:
        void on_status_cr_changed(const Glib::ustring& path, const Glib::ustring& new_text);
        void send_anime_update(const std::shared_ptr<Anime>& anime);
    };

    class AnimeDetailViewBase: public virtual MALItemDetailViewBase {
    public:
        AnimeDetailViewBase(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<MALItem>& item);
        
    protected:
        Gtk::Grid                    *m_status_type_grid;
        Glib::RefPtr<Gtk::SizeGroup>  m_status_type_sizegroup;
        Gtk::Label                   *m_series_status_label;
        Gtk::Label                   *m_series_type_label;
    };

    class AnimeDetailViewStatic: public MALItemDetailViewStatic, public AnimeDetailViewBase {
    public:
        AnimeDetailViewStatic(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<MALItem>& item);
        
    protected:
        Gtk::Grid                    *m_episodes_grid;
        Gtk::Label                   *m_series_episodes_label;
    };

    class AnimeDetailViewEditable : public MALItemDetailViewEditable, public AnimeDetailViewBase {
    public:
        AnimeDetailViewEditable(const std::shared_ptr<MAL>&,
                                const std::shared_ptr<AnimeModelColumnsEditable>&,
                                const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>&);
        
        /* You must chain up */
        virtual void display_item(const std::shared_ptr<MALItem>& item);
        
    protected:
        Gtk::Grid           *m_episodes_grid;
        Gtk::Label          *m_series_episodes_label;
        Gtk::Entry          *m_episodes_entry;
        Gtk::Button         *m_episodes_button;
		AnimeStatusComboBox *m_anime_status_combo;

        /* Chain up */
        virtual bool update_list_model(const Gtk::TreeRow &row);

    private:
        void increment_button_cb();
    };

    class AnimeSearchListPage : public MALItemListPage {
    public:
		AnimeSearchListPage(const std::shared_ptr<MAL>&,
                            AnimeListViewStatic*   list_view,
                            AnimeDetailViewStatic* detail_view);

    protected:
        Gtk::Entry *m_search_entry;
		virtual void refresh();

    };

    class AnimeFilteredListPage : public MALItemListPage {
    public:
		AnimeFilteredListPage(const std::shared_ptr<MAL>& mal,
                              AnimeListViewEditable*      list_view,
                              AnimeDetailViewEditable*    detail_view);

    protected:
		virtual void refresh();

    private:
        AnimeListViewEditable* m_list_view;
        AnimeDetailViewEditable* m_detail_view;
        AnimeStatusComboBox *m_status_combo;

        bool m_filter_func(const std::shared_ptr<MALItem>&) const;
    };
}
