#pragma once
#include <memory>
#include <giomm/memoryinputstream.h>
#include <glibmm/dispatcher.h>
#include <glibmm/property.h>
#include <gtkmm/bin.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/treeview.h>
#include <sigc++/slot.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/cellrenderercombo.h>
#include "malitem.hpp"
#include "mal.hpp"

namespace MAL {
	class ScoreColumns final : public Gtk::TreeModel::ColumnRecord {
	public:
        Gtk::TreeModelColumn<Glib::ustring> text;
        Gtk::TreeModelColumn<int> score;
		ScoreColumns() { add(text); add(score); };
	};

	class ScoreComboBox final : public Gtk::ComboBox {
	public:
		ScoreComboBox();
		int get_score() const;
        void set_score(const int score);
        
    private:
         ScoreColumns                       m_columns;
         Glib::RefPtr<Gtk::ListStore>       m_model;
	};

    class ScoreCellRendererCombo final : public Gtk::CellRendererCombo {
    public:
        ScoreCellRendererCombo();

        Glib::PropertyProxy<gint> property_score();
        gint get_score_from_string(const Glib::ustring&) const;
    private:
        ScoreColumns m_columns;
        Glib::RefPtr<Gtk::ListStore> m_model;

        void score_changed_cb();
        Glib::Property<gint> m_score_property;
    };

    class MALItemModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> series_title;
        Gtk::TreeModelColumn<std::shared_ptr<const MALItem> > item;
        Gtk::TreeModelColumn<Glib::ustring> series_season;
        Gtk::TreeModelColumn<Glib::ustring> series_start_date;

        MALItemModelColumns() { add(series_title);
            add(item);
            add(series_season);
            add(series_start_date);
        } 
    };

    class MALItemModelColumnsStatic : public virtual MALItemModelColumns
    {
    public:
        Gtk::TreeModelColumn<float> score;
        MALItemModelColumnsStatic() : MALItemModelColumns() {
            add(score);
        }
    };

    class MALItemModelColumnsEditable : public virtual MALItemModelColumns
    {
    public:
        Gtk::TreeModelColumn<int> score;
        MALItemModelColumnsEditable() : MALItemModelColumns() {
            add(score);
        }
    };

    class MALItemListViewNotifier
    {
    public:
        MALItemListViewNotifier(const std::shared_ptr<MALItemModelColumns> &columns,
                                const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&> &c);
        virtual ~MALItemListViewNotifier() = default;

    protected:
        std::shared_ptr<MALItemModelColumns> m_notify_columns;

        void notify_list_model();

        /* Chain up */
        virtual bool update_list_model(const Gtk::TreeRow &row) = 0;

    private:
        bool list_model_foreach_cb(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
        sigc::slot<void> list_model_foreach;
    };

	class MALItemDetailViewBase : public Gtk::Grid {
	public:
		MALItemDetailViewBase(const std::shared_ptr<MAL>&);
        virtual ~MALItemDetailViewBase() = default;

        /* You must chain up */
		virtual void display_item(const std::shared_ptr<const MALItem>& item);

	protected:
		std::shared_ptr<MAL>                 m_mal;
        std::shared_ptr<const MALItem>       m_item;
		Gtk::Image                          *m_image;
		Gtk::Label                          *m_title;
        Gtk::Grid                           *m_grid;

    private:
		Glib::Dispatcher                     m_signal_image_available;
		Glib::RefPtr<Gio::MemoryInputStream> image_stream;
		void on_image_available();
		void do_fetch_image();
	};

    class MALItemDetailViewStatic : public virtual MALItemDetailViewBase {
    public:
        MALItemDetailViewStatic(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item);
        
    protected:
        Gtk::Label *m_score;
    };

    class MALItemDetailViewEditable : public virtual MALItemDetailViewBase, public MALItemListViewNotifier {
    public:
        MALItemDetailViewEditable(const std::shared_ptr<MAL>&,
                                  const std::shared_ptr<MALItemModelColumnsEditable>&,
                                  const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>&);
        
        /* You must chain up */
        virtual void display_item(const std::shared_ptr<const MALItem>& item);
        
    protected:
        /* Chain up */
        virtual bool update_list_model(const Gtk::TreeRow &row);
        std::shared_ptr<MALItemModelColumns> m_columns;

        Gtk::Grid     *m_score_grid;
        ScoreComboBox *m_score;
    };

	class MALItemListViewBase : public Gtk::Grid {
	public:
        MALItemListViewBase() = delete;
		MALItemListViewBase(const std::shared_ptr<MAL>&,
                            const std::shared_ptr<MALItemModelColumns>&);
        MALItemListViewBase& operator=(const MALItemListViewBase&) = delete;
        MALItemListViewBase(const MALItemListViewBase&) = delete;
        virtual ~MALItemListViewBase() = default;

		void set_item_list(std::list<std::shared_ptr<const MALItem>>&& items);
        void set_filter_func(const sigc::slot<bool, const std::shared_ptr<const MALItem>&>& slot);
        void refilter();

		void do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter& slot) {m_model->foreach(slot);};
		void set_row_activated_cb(sigc::slot<void, const std::shared_ptr<const MALItem>&> slot) { m_row_activated_cb = slot;} ;
        std::shared_ptr<MALItemModelColumns> m_columns;

	protected:
		std::shared_ptr<MAL>                       m_mal;
		Glib::RefPtr<Gtk::ListStore>               m_model;
		Gtk::TreeView                              *m_treeview;
		std::list<std::shared_ptr<const MALItem> > m_items;
        std::shared_ptr<const MALItem>             m_detailed_item;
        Gtk::TreeViewColumn                        *m_title_column;
        Gtk::TreeViewColumn                        *m_season_column;

        // Use if you connect to signal_row_changed
        sigc::connection                          m_model_changed_connection;


        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void                     refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row);
		sigc::slot<void, const std::shared_ptr<const MALItem>&> m_row_activated_cb;

    private:
		Glib::Dispatcher                 m_signal_refreshed;

		void                             refresh_cb();
		void on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
        sigc::slot<bool, const std::shared_ptr<const MALItem>&> m_filter_func;
	};

    class MALItemListViewStatic : public virtual MALItemListViewBase {
    public:
        MALItemListViewStatic(const std::shared_ptr<MAL>&,
                              const std::shared_ptr<MALItemModelColumnsStatic>&);

    protected:
        Gtk::TreeViewColumn *m_score_column;
        
        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row);
    };

    class MALItemListViewEditable : public virtual MALItemListViewBase {
    public:
        MALItemListViewEditable(const std::shared_ptr<MAL>&,
                                const std::shared_ptr<MALItemModelColumnsEditable>&);

        void do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter&);

    protected:
        Gtk::TreeViewColumn *m_score_column;
        ScoreCellRendererCombo *m_score_cellrenderer;

        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void refresh_item_cb(const std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row);

        /* Chain up!
         * Called when the tree model was changed due to editing.
         * This method should set the appropriate field in item from
         * the tree row, then set the item back into the model as
         * well. 
         * Return true when the item value is different from the model value.
         */
        virtual bool model_changed_cb(std::shared_ptr<const MALItem>& item, const Gtk::TreeRow& row);

        /* Called on main thread. Item should be transmitted back to MAL.net.
         */
        virtual void send_item_update(const std::shared_ptr<const MALItem>& item) = 0;

    private:
        void on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
        void score_edited_cb(const Glib::ustring& path, const Glib::ustring& text);
    };

	class MALItemListPage : public Gtk::Grid {
	public:
		MALItemListPage(const std::shared_ptr<MAL>   &mal,
                        MALItemListViewBase*   list_view,
                        MALItemDetailViewBase* detail_view);
		void refresh_async();

    protected:
		std::shared_ptr<MAL>   m_mal;
		MALItemListViewBase   *m_list_view;
		MALItemDetailViewBase *m_detail_view;
        Gtk::Grid             *m_button_row;
        Gtk::Button           *m_refresh_button;

        /* Refresh the List View. This method is called in a separate thread! */
		virtual void refresh() = 0;
	};



}
