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
#include <string>
#include <ctime>
#include <set>
#include <glibmm/date.h>
#include "xml_writer.hpp"
#include "xml_reader.hpp"

namespace MAL {

    enum Priority {
        PRIORITY_INVALID = -1,
        PRIORITY_LOW     = 0,
        PRIORITY_MEDIUM  = 1,
        PRIORITY_HIGH    = 2
    };

    enum ReconsumeValue {
        RECONSUME_VALUE_INVALID  = -1,
        RECONSUME_VALUE_VERYLOW  = 1,
        RECONSUME_VALUE_LOW      = 2,
        RECONSUME_VALUE_MEDIUM   = 3,
        RECONSUME_VALUE_HIGH     = 4,
        RECONSUME_VALUE_VERYHIGH = 5,
    };

    Priority priority_from_int(const int i);
    ReconsumeValue reconsume_value_from_int(const int i);

    Priority priority_from_string(const Glib::ustring &);
    ReconsumeValue reconsume_value_from_string(const Glib::ustring &);

    Glib::ustring to_string(const Priority p);
    Glib::ustring to_string(const ReconsumeValue r);

	class MALItem {
	public:
		MALItem();
        MALItem(XmlReader&);
		virtual ~MALItem() = default;
        virtual std::shared_ptr<MALItem> clone() const;
        MALItem& operator=(const MALItem&) = default;
        MALItem(const MALItem&) = default;

        /* Chain up */
        virtual void serialize(XmlWriter&) const;

    public:

		int_fast64_t          series_itemdb_id;   //N
		std::string           series_title;       //D
        std::string           series_preferred_title;
		std::string           series_date_begin;  //D
		std::string           series_date_end;    //D
		std::string           image_url;          //D
		std::set<std::string> series_synonyms;    //D
		std::string           series_synopsis;    //D

		std::set<std::string> tags;               // We know how to serialize /
		std::string           date_start;         //Ddeserialize these fields
		std::string           date_finish;        //D
		int_fast64_t          id;                 //N<-- not serialized
		std::time_t           last_updated;       //N<-- not serialized
		float                 score;              //D
		bool                  enable_reconsuming; //D

		std::string           fansub_group;       //DWe know how to serialize
		std::string           comments;           // these fields, but not
		int_fast16_t          downloaded_items;   // deserialize
		int_fast16_t          times_consumed;     //
		ReconsumeValue        reconsume_value;    //
		Priority              priority;           //
		bool                  enable_discussion;  //

        bool                  has_details;

        virtual void update_from_details (const std::shared_ptr<MALItem>& details);
        virtual void update_from_list (const std::shared_ptr<MALItem>& item);

		void set_series_itemdb_id       (std::string&&);
		void set_series_title           (std::string&&);
		void set_series_preferred_title (std::string&&);
		void set_series_date_begin      (std::string&&);
		void set_series_date_end        (std::string&&);
		void set_image_url              (std::string&&);
		void add_series_synonym         (std::string&&);
		void set_series_synonyms        (std::string&&);
		void set_series_synopsis        (std::string&&);

		void add_tag                    (std::string&&);
		void set_tags                   (std::string&&);
		void set_date_start             (std::string&&);
		void set_date_finish            (std::string&&);
		void set_id                     (std::string&&);
		void set_last_updated           (std::string&&);
		void set_score                  (std::string&&);
		void set_enable_reconsuming     (std::string&&);

        void set_fansub_group           (std::string&&);
		void set_comments               (std::string&&);
		void set_downloaded_items       (std::string&&);
		void set_times_consumed         (std::string&&);
		void set_reconsume_value        (std::string&&);
		void set_priority               (std::string&&);
		void set_enable_discussion      (std::string&&);
        
        void set_has_details            (std::string&&);

        std::string get_season_began() const;
        Glib::Date get_date_began() const;
        Glib::Date get_date_ended() const;
        Glib::Date get_date(const std::string&) const;
    private:
        std::string get_season(Glib::Date&&) const;
        std::string get_season(const std::string&) const;
	};

}
