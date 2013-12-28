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
#include <giomm/memoryinputstream.h>
#include <glibmm/dispatcher.h>
#include <glibmm/property.h>
#include <gtkmm/bin.h>
#include <gtkmm/grid.h>
#include <gtkmm/switch.h>
#include <gtkmm/treeview.h>
#include <gtkmm/frame.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/cellrenderercombo.h>
#include <gtkmm/sizegroup.h>
#include <gtkmm/progressbar.h>
#include <sigc++/slot.h>
#include "malitem.hpp"
#include "mal.hpp"
#include "increment_entry.hpp"
#include "date_widgets.hpp"
#include "cellrendererscore.hpp"

namespace MAL {
	class MALItemPriorityComboBox final : public Gtk::ComboBoxText {
	public:
		MALItemPriorityComboBox();
		Priority get_priority() const;
	};

	class MALItemReconsumeValueComboBox final : public Gtk::ComboBoxText {
	public:
		MALItemReconsumeValueComboBox();
		ReconsumeValue get_reconsume_value() const;
        void set_reconsume_value(const ReconsumeValue value);

    private:
        const Glib::ustring invalid_text;
	};

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

    class MALItemModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Gtk::TreeModelColumn<Glib::ustring> series_title;
        Gtk::TreeModelColumn<std::shared_ptr<MALItem> > item;
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
        Gtk::TreeModelColumn<Glib::ustring> begin_date;
        Gtk::TreeModelColumn<Glib::ustring> end_date;
        Gtk::TreeModelColumn<bool> enable_reconsuming;
        Gtk::TreeModelColumn<Glib::ustring> fansub_group;
        Gtk::TreeModelColumn<int> downloaded_items;
        Gtk::TreeModelColumn<int> times_consumed;
        Gtk::TreeModelColumn<ReconsumeValue> reconsume_value;
        Gtk::TreeModelColumn<Priority> priority;
        
        MALItemModelColumnsEditable() : MALItemModelColumns() {
            add(score); add(begin_date); add(end_date); add(enable_reconsuming);
            add(fansub_group); add(downloaded_items); add(times_consumed);
            add(reconsume_value); add(priority);
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
		virtual void display_item(const std::shared_ptr<MALItem>& item);

	protected:
		std::shared_ptr<MAL>                 m_mal;
        std::shared_ptr<MALItem>             m_item;
		Gtk::Image                          *m_image;
		Gtk::Label                          *m_title;
        Gtk::Grid                           *m_grid;
        Gtk::Grid                           *m_grid_left;
        Gtk::Grid                           *m_grid_center;
        Gtk::Grid                           *m_grid_right;
        Gtk::Grid                           *m_alt_title_grid;
        Gtk::Frame                          *m_synopsis_frame;
        Gtk::Label                          *m_synopsis_label;
        Gtk::Grid                           *m_series_date_grid;
        DateLabel                           *m_series_start_date_label;
        DateLabel                           *m_series_end_date_label;
    private:
		Glib::Dispatcher                     m_signal_image_available;
		Glib::RefPtr<Gio::MemoryInputStream> image_stream;
        Glib::RefPtr<Gtk::SizeGroup>         m_series_date_sizegroup;
		void on_image_available();
		void do_fetch_image();
	};

    class MALItemDetailViewStatic : public virtual MALItemDetailViewBase {
    public:
        MALItemDetailViewStatic(const std::shared_ptr<MAL>&);

        /* You must chain up */
        virtual void display_item(const std::shared_ptr<MALItem>& item);
        
    protected:
        Gtk::Label *m_score;
    };

    class MALItemDetailViewEditable : public virtual MALItemDetailViewBase, public MALItemListViewNotifier {
    public:
        MALItemDetailViewEditable(const std::shared_ptr<MAL>&,
                                  const std::shared_ptr<MALItemModelColumnsEditable>&,
                                  const sigc::slot<void, const Gtk::TreeModel::SlotForeachPathAndIter&>&);
        
        /* You must chain up */
        virtual void display_item(const std::shared_ptr<MALItem>& item);
        
    protected:
        /* Chain up */
        virtual bool update_list_model(const Gtk::TreeRow &row);
        std::shared_ptr<MALItemModelColumns> m_columns;

        Gtk::Grid      *m_score_grid;
        ScoreComboBox  *m_score;
        Gtk::Label     *m_date_begin_label;
        DateEntry      *m_date_begin_entry;
        Gtk::Label     *m_date_end_label;
        DateEntry      *m_date_end_entry;
        Gtk::Label     *m_reconsuming_label;
        Gtk::Switch    *m_reconsuming_switch;
        IncrementEntry *m_downloaded_items_entry;
        IncrementEntry *m_times_consumed_entry;
        Gtk::Label     *m_fansub_group_label;
        Gtk::Entry     *m_fansub_group_entry;
        Gtk::Label     *m_priority_label;
        Gtk::Label     *m_reconsume_value_label;
        MALItemPriorityComboBox       *m_priority_combo;
        MALItemReconsumeValueComboBox *m_reconsume_value_combo;

    private:
        sigc::connection m_score_changed_connection;
        sigc::connection m_reconsuming_changed_connection;
        sigc::connection m_priority_changed_connection;
        sigc::connection m_reconsume_value_changed_connection;
    };

	class MALItemListViewBase : public Gtk::Grid {
	public:
        MALItemListViewBase() = delete;
		MALItemListViewBase(const std::shared_ptr<MAL>&,
                            const std::shared_ptr<MALItemModelColumns>&);
        MALItemListViewBase& operator=(const MALItemListViewBase&) = delete;
        MALItemListViewBase(const MALItemListViewBase&) = delete;
        virtual ~MALItemListViewBase() = default;

        void set_filter_func(const sigc::slot<bool, const std::shared_ptr<MALItem>&>& slot);


        void refresh_items(const std::function<void (const std::function<void (const std::shared_ptr<MALItem>&)>& )>& for_each_functor);
        /*template<typename UnaryForeachFunctor>
        UnaryForeachFunctor refresh_items(UnaryForeachFunctor&& for_each_functor) {
            m_model->clear();
            m_model_changed_connection.block();
            auto ret = for_each_functor(std::bind(&MALItemListViewBase::append_item, this, std::placeholders::_1));
            m_model_changed_connection.unblock();
            return ret;
        }*/

		void do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter& slot) {m_model->foreach(slot);};
		void set_row_activated_cb(sigc::slot<void, const std::shared_ptr<MALItem>&> slot) { m_row_activated_cb = slot;} ;
        std::shared_ptr<MALItemModelColumns> m_columns;

	protected:
		std::shared_ptr<MAL>                        m_mal;
		Glib::RefPtr<Gtk::ListStore>                m_model;
		Gtk::TreeView                              *m_treeview;
        std::shared_ptr<MALItem>                    m_detailed_item;
        Gtk::TreeViewColumn                        *m_title_column;
        Gtk::TreeViewColumn                        *m_season_column;

        // Use if you connect to signal_row_changed
        sigc::connection                          m_model_changed_connection;


        /* Chain up!
         * Called when m_items has changed (We have fetched a new anime list from MAL)
         * This method should take data from the item and put it on the row
         */
        virtual void                     refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row);
		sigc::slot<void, const std::shared_ptr<MALItem>&> m_row_activated_cb;

    private:
		void on_my_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
        sigc::slot<bool, const std::shared_ptr<MALItem>&> m_filter_func;
        void append_item(const std::shared_ptr<MALItem>& item);
        int malitem_comparitor(const Gtk::TreeModel::iterator&, const Gtk::TreeModel::iterator&);
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
        virtual void refresh_item_cb(const std::shared_ptr<MALItem>& item, const Gtk::TreeRow& row);
    };

    class MALItemListViewEditable : public virtual MALItemListViewBase {
    public:
        MALItemListViewEditable(const std::shared_ptr<MAL>&,
                                const std::shared_ptr<MALItemModelColumnsEditable>&);

        void do_model_foreach(const Gtk::TreeModel::SlotForeachPathAndIter&);

    protected:
        Gtk::TreeViewColumn *m_score_column;
        CellRendererScore *m_score_cellrenderer;

        /* 
         * Should be implemented as calling e.g. MAL::get_detailed_anime_async
         */
        virtual void get_details_for_item(const std::shared_ptr<MALItem>& item) = 0;

        /* 
         * Must be hooked up to e.g. MAL::signal_anime_detailed
         */
        void on_detailed_item();

        /* Chain up!
         *
         * Called from on_detailed_item() with the correct iter for
         * m_detailed_item. Should set the detailed info into the tree
         * model (so that it will react predicably to changes).
         */
        virtual void on_detailed_item_cb(const Gtk::TreeRow& row);

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
        virtual void send_item_update(const std::shared_ptr<MALItem>& item) = 0;

    private:
        void on_model_changed(const Gtk::TreeModel::Path&, const Gtk::TreeModel::iterator&);
        void score_edited_cb(const Glib::ustring& path, const Glib::ustring& text);
    };

	class MALItemListPage : public Gtk::Grid {
	public:
		MALItemListPage(const std::shared_ptr<MAL>   &mal,
                        MALItemListViewBase*   list_view,
                        MALItemDetailViewBase* detail_view);

    protected:
		std::shared_ptr<MAL>   m_mal;
		MALItemListViewBase   *m_list_view;
		MALItemDetailViewBase *m_detail_view;
        Gtk::Grid             *m_button_row;
        Gtk::Button           *m_refresh_button;
        Gtk::ProgressBar      *m_progressbar;

        /* Refresh the List View */
		virtual void refresh() = 0;

        /* Callback on main thread when MAL has a new list */
        virtual void on_mal_update() {};
	};



}
