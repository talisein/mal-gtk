#pragma once
#include <string>
#include <ctime>
#include <set>
#include "malitem.hpp"

namespace MAL {
	enum AnimeStatus { ANIMESTATUS_INVALID = -1,
	                   WATCHING = 1,
	                   COMPLETED = 2,
	                   ONHOLD = 3,
	                   DROPPED = 4,
	                   PLANTOWATCH = 6
	};

	enum SeriesStatus { SERIESSTATUS_INVALID = -1,
	                    AIRING = 1,
	                    FINISHED = 2,
	                    NOTYETAIRED = 3,
	};

	enum SeriesType { SERIESTYPE_INVALID = -1,
	                  TV = 1,
	                  OVA = 2,
	                  MOVIE = 3,
	                  SPECIAL = 4,
	                  ONA = 5,
	                  MUSIC = 6
	};
	                  
	
	std::string to_string(const SeriesType s);
	std::string to_string(const SeriesStatus s);
	std::string to_string(const AnimeStatus s);

	SeriesType anime_series_type_from_int(const int i);
	SeriesType anime_series_type_from_string(const std::string& s);

	SeriesStatus anime_series_status_from_int(const int i);
	SeriesStatus anime_series_status_from_string(const std::string& s);

	AnimeStatus anime_status_from_int(const int i);
	AnimeStatus anime_status_from_string(const std::string& s);


	class Anime final : public MALItem {
	public:
		Anime() = default;
        virtual std::shared_ptr<MALItem> clone() const override;
		
		SeriesType            series_type;
		SeriesStatus          series_status;
		int_fast16_t          series_episodes;

		AnimeStatus           status;          // We know how to serialize /
		int_fast16_t          episodes;        // deserialize these fields
		int_fast16_t          rewatch_episode; //

		int_fast16_t          storage_type;    // We know how to serialize
		float                 storage_value;   // these fields, but not
                                               // deserialize

		void set_series_type         (const std::string&);
		void set_series_status       (const std::string&);
		void set_series_episodes     (const std::string&);

		void set_status              (const std::string&);
		void set_episodes            (const std::string&);
        void set_rewatch_episode     (const std::string&);

		void set_storage_type        (const std::string&);
		void set_storage_value       (const std::string&);
	};
}
