#pragma once
#include <set>
#include <string>
#include <ctime>

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

	SeriesType anime_series_type_from_int(const int i);
	SeriesStatus anime_series_status_from_int(const int i);
	AnimeStatus anime_status_from_int(const int i);

	class Anime {
	public:
		Anime() = default;
		~Anime() = default;
		
		int64_t               series_animedb_id;
		std::string           series_title;
		SeriesType            series_type;
		int16_t               series_episodes;
		SeriesStatus          series_status;
		std::string           series_date_begin;
		std::string           series_date_end;
		std::string           image_url;
		std::set<std::string> series_synonyms;

		int64_t               id; //
		AnimeStatus           status; //
		int16_t               episodes; //
		int16_t               score; //
		int16_t               downloaded_episodes;
		int16_t               storage_type;
		float                 storage_value;
		int16_t               times_watched;
		int16_t               rewatch_value;
		std::string           date_start; //
		std::string           date_finish; //
		int16_t               priority;
		bool                  enable_discussion;
		bool                  enable_rewatching; //
		std::string           comments;
		std::string           fansub_group;
		std::set<std::string> tags; //

		int16_t               rewatch_episode; //
		std::time_t           last_updated; //
		
	};
}
