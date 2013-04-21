#pragma once
#include <memory>
#include <string>
#include <ctime>
#include <set>
#include <glibmm/date.h>

namespace MAL {

	class MALItem {
	public:
		MALItem();
		virtual ~MALItem() = default;
        virtual std::shared_ptr<MALItem> clone() const;
        MALItem& operator=(const MALItem&) = default;
        MALItem(const MALItem&) = default;
    public:

		int_fast64_t          series_itemdb_id;
		std::string           series_title;
		std::string           series_date_begin;
		std::string           series_date_end;
		std::string           image_url;
		std::set<std::string> series_synonyms;
		std::string           series_synopsis;

		std::set<std::string> tags;               // We know how to serialize /
		std::string           date_start;         // deserialize these fields
		std::string           date_finish;        //
		int_fast64_t          id;                 //<-- not serialized
		std::time_t           last_updated;       //<-- not serialized
		float                 score;              //
		bool                  enable_reconsuming; //

		std::string           fansub_group;       // We know how to serialize
		std::string           comments;           // these fields, but not
		int_fast16_t          downloaded_items;   // deserialize
		int_fast16_t          times_consumed;     //
		int_fast16_t          reconsume_value;    //
		int_fast16_t          priority;           //
		bool                  enable_discussion;  //

		void set_series_itemdb_id    (std::string&&);
		void set_series_title        (std::string&&);
		void set_series_date_begin   (std::string&&);
		void set_series_date_end     (std::string&&);
		void set_image_url           (std::string&&);
		void set_series_synonyms     (std::string&&);
		void set_series_synopsis     (std::string&&);

		void set_tags                (std::string&&);
		void set_date_start          (std::string&&);
		void set_date_finish         (std::string&&);
		void set_id                  (std::string&&);
		void set_last_updated        (std::string&&);
		void set_score               (std::string&&);
		void set_enable_reconsuming  (std::string&&);

		void set_fansub_group        (std::string&&);
		void set_comments            (std::string&&);
		void set_downloaded_items    (std::string&&);
		void set_times_consumed      (std::string&&);
		void set_reconsume_value     (std::string&&);
		void set_priority            (std::string&&);
		void set_enable_discussion   (std::string&&);

        std::string get_season_began() const;
        Glib::Date get_date_began() const;
        Glib::Date get_date_ended() const;

    private:
        std::string get_season(Glib::Date&&) const;
        std::string get_season(const std::string&) const;
        Glib::Date get_date(const std::string&) const;
	};

}
