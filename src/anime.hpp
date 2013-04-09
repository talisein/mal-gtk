#pragma once
#include <string>
#include <ctime>
#include <set>

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

	std::string anime_season_from_date(const std::string& s);

	SeriesType anime_series_type_from_int(const int i);
	SeriesType anime_series_type_from_string(const std::string& s);

	SeriesStatus anime_series_status_from_int(const int i);
	SeriesStatus anime_series_status_from_string(const std::string& s);

	AnimeStatus anime_status_from_int(const int i);
	AnimeStatus anime_status_from_string(const std::string& s);


	class Anime {
	public:
		Anime() = default;
		~Anime() = default;
		
		SeriesType            series_type;
		SeriesStatus          series_status;
		std::set<std::string> series_synonyms;
		std::string           series_title;
		std::string           series_date_begin;
		std::string           series_date_end;
		std::string           series_synopsis;
		std::string           image_url;
		int_fast64_t          series_animedb_id;
		int_fast16_t          series_episodes;

		std::set<std::string> tags;              // We know how to serialize
		std::string           date_start;        // these fields
		std::string           date_finish;       //
		AnimeStatus           status;            //
		int_fast64_t          id;                //
		int_fast16_t          episodes;          //
		int_fast16_t          rewatch_episode;   //
		std::time_t           last_updated;      //
		float                 score;             //
		bool                  enable_rewatching; //

		std::string           comments;
		std::string           fansub_group;
		int_fast16_t          downloaded_episodes;
		int_fast16_t          storage_type;
		int_fast16_t          times_watched;
		int_fast16_t          rewatch_value;
		int_fast16_t          priority;
		float                 storage_value;
		bool                  enable_discussion;


		void set_series_animedb_id   (const std::string&);
		void set_series_title        (const std::string&);
		void set_series_type         (const std::string&);
		void set_series_episodes     (const std::string&);
		void set_series_status       (const std::string&);
		void set_series_date_begin   (const std::string&);
		void set_series_date_end     (const std::string&);
		void set_image_url           (const std::string&);
		void set_series_synonyms     (const std::string&);
		void set_series_synopsis     (const std::string&);
		void set_id                  (const std::string&);
		void set_status              (const std::string&);
		void set_episodes            (const std::string&);
		void set_score               (const std::string&);
		void set_downloaded_episodes (const std::string&);
		void set_storage_type        (const std::string&);
		void set_storage_value       (const std::string&);
		void set_times_watched       (const std::string&);
		void set_rewatch_value       (const std::string&);
		void set_date_start          (const std::string&);
		void set_date_finish         (const std::string&);
		void set_priority            (const std::string&);
		void set_enable_discussion   (const std::string&);
		void set_enable_rewatching   (const std::string&);
		void set_comments            (const std::string&);
		void set_fansub_group        (const std::string&);
		void set_tags                (const std::string&);
		void set_rewatch_episode     (const std::string&);
		void set_last_updated        (const std::string&);
	};
}
