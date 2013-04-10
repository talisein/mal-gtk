#pragma once
#include <string>
#include <ctime>
#include <set>

namespace MAL {
	enum MangaStatus { MANGASTATUS_INVALID = -1,
	                   READING = 1,
	                   MANGACOMPLETED = 2,
	                   MANGAONHOLD = 3,
	                   MANGADROPPED = 4,
	                   PLANTOREAD = 6
	};

	enum MangaSeriesStatus { MANGASERIESSTATUS_INVALID = -1,
                             PUBLISHING = 1,
                             MANGAFINISHED = 2,
                             NOTYETPUBLISHED = 3,
	};

	enum MangaSeriesType { MANGASERIESTYPE_INVALID = -1,
                           MANGA   = 1,
                           NOVEL   = 2,
                           ONESHOT = 3,
                           DOUJIN  = 4,
                           MANHWA  = 5,
                           MANHUA  = 6,
                           OEL     = 7
	};
	                  
	
	std::string to_string(const MangaSeriesType s);
	std::string to_string(const MangaSeriesStatus s);
	std::string to_string(const MangaStatus s);

	std::string manga_season_from_date(const std::string& s);

	MangaSeriesType manga_series_type_from_int(const int i);
	MangaSeriesType manga_series_type_from_string(const std::string& s);

	MangaSeriesStatus manga_series_status_from_int(const int i);
	MangaSeriesStatus manga_series_status_from_string(const std::string& s);

	MangaStatus manga_status_from_int(const int i);
	MangaStatus manga_status_from_string(const std::string& s);


	class Manga {
	public:
		Manga() = default;
		~Manga() = default;
		
		MangaSeriesType       series_type;
		MangaSeriesStatus     series_status;
		std::set<std::string> series_synonyms;
		std::string           series_title;
		std::string           series_date_begin;
		std::string           series_date_end;
		std::string           series_synopsis;
		std::string           image_url;
		int_fast64_t          series_mangadb_id;
		int_fast16_t          series_chapters;
		int_fast16_t          series_volumes;

		std::set<std::string> tags;                // We know how to deserialize
		std::string           date_start;          // these fields, but not
		std::string           date_finish;         // serialize.
		MangaStatus           status;              //
		int_fast64_t          id;                  //
		int_fast16_t          chapters;            //
		int_fast16_t          volumes;             //
		int_fast16_t          rereading_chapter;   //
		std::time_t           last_updated;        //
		float                 score;               //
		bool                  enable_rereading;    //

		std::string           scan_group;          // We know how to serialize
		std::string           comments;            // these fields, but not
		int_fast16_t          retail_volumes;      // deserialize.
        int_fast16_t          downloaded_chapters; //
        int_fast16_t          times_reread;        //
        int_fast16_t          reread_value;        //
        int_fast16_t          priority;            //
        bool                  enable_discussion;   //

		void set_series_mangadb_id   (const std::string&);
		void set_series_title        (const std::string&);
		void set_series_type         (const std::string&);
		void set_series_chapters     (const std::string&);
		void set_series_volumes      (const std::string&);
		void set_series_status       (const std::string&);
		void set_series_date_begin   (const std::string&);
		void set_series_date_end     (const std::string&);
		void set_image_url           (const std::string&);
		void set_series_synonyms     (const std::string&);
		void set_id                  (const std::string&);
		void set_score               (const std::string&);
		void set_chapters            (const std::string&);
		void set_volumes             (const std::string&);
		void set_date_start          (const std::string&);
		void set_date_finish         (const std::string&);
		void set_status              (const std::string&);
		void set_enable_rereading    (const std::string&);
		void set_rereading_chapter   (const std::string&);
		void set_last_updated        (const std::string&);
		void set_tags                (const std::string&);
		void set_series_synopsis     (const std::string&);

		void set_scan_group          (const std::string&);
		void set_comments            (const std::string&);
		void set_retail_volumes      (const std::string&);
		void set_downloaded_chapters (const std::string&);
		void set_times_reread        (const std::string&);
		void set_reread_value        (const std::string&);
		void set_priority            (const std::string&);
		void set_enable_discussion   (const std::string&);
	};
}
