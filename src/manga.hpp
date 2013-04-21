#pragma once
#include <string>
#include <ctime>
#include <set>
#include "malitem.hpp"

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
	                  
	
    Glib::ustring to_string(const MangaSeriesType s);
    Glib::ustring to_string(const MangaSeriesStatus s);
    Glib::ustring to_string(const MangaStatus s);

	MangaSeriesType manga_series_type_from_int(const int i);
	MangaSeriesType manga_series_type_from_string(const Glib::ustring& s);

	MangaSeriesStatus manga_series_status_from_int(const int i);
	MangaSeriesStatus manga_series_status_from_string(const Glib::ustring& s);

	MangaStatus manga_status_from_int(const int i);
	MangaStatus manga_status_from_string(const Glib::ustring& s);


	class Manga final : public MALItem {
	public:
		Manga() = default;
        virtual std::shared_ptr<MALItem> clone() const override;
		
		MangaSeriesType       series_type;
		MangaSeriesStatus     series_status;
		int_fast16_t          series_chapters;
		int_fast16_t          series_volumes;

		MangaStatus           status;            // We know how to serialize /
		int_fast16_t          chapters;          // deserialize these fields
		int_fast16_t          volumes;           //
		int_fast16_t          rereading_chapter; //

		int_fast16_t          retail_volumes;    // We know how to serialize
                                                 // but not deserialize

		void set_series_type         (std::string&&);
		void set_series_chapters     (std::string&&);
		void set_series_volumes      (std::string&&);
		void set_series_status       (std::string&&);

        void set_status              (std::string&&);
		void set_chapters            (std::string&&);
		void set_volumes             (std::string&&);
		void set_rereading_chapter   (std::string&&);

		void set_retail_volumes      (std::string&&);
	};
}
