#include "anime.hpp"
#include <iostream>

namespace MAL {
	SeriesType anime_series_type_from_int(const int i) {
		switch(i) {
		case 1:
			return TV;
		case 2:
			return OVA;
		case 3:
			return MOVIE;
		case 4:
			return SPECIAL;
		case 5:
			return ONA;
		case 6:
			return MUSIC;
		default:
			std::cerr << "Error: Unknown anime series type " << i << std::endl;
			return SERIESTYPE_INVALID;
		}
	}

	SeriesType anime_series_type_from_string(const std::string& s) {
		if (s.compare(to_string(TV)) == 0)
			return TV;
		else if (s.compare(to_string(OVA)) == 0)
			return OVA;
		else if (s.compare(to_string(MOVIE)) == 0)
			return MOVIE;
		else if (s.compare(to_string(SPECIAL)) == 0)
			return SPECIAL;
		else if (s.compare(to_string(ONA)) == 0)
			return ONA;
		else if (s.compare(to_string(MUSIC)) == 0)
			return MUSIC;
		else {
			std::cerr << "Error: Unknown series type '" << s << "'" << std::endl;
			return SERIESTYPE_INVALID;
		}
	}

	SeriesStatus anime_series_status_from_int(const int i) {
		switch (i) {
		case 1:
			return AIRING;
		case 2:
			return FINISHED;
		case 3:
			return NOTYETAIRED;
		default:
			std::cerr << "Error: Unknown series status " << i << std::endl;
			return SERIESSTATUS_INVALID;
		}
	}

	SeriesStatus anime_series_status_from_string(const std::string& s) {
		if (s.compare(to_string(AIRING)) == 0 ||
		    s.compare("Currently Airing") == 0)
			return AIRING;
		else if (s.compare(to_string(FINISHED)) == 0 ||
		    s.compare("Finished Airing") == 0)
			return FINISHED;
		else if (s.compare(to_string(FINISHED)) == 0 ||
		    s.compare("Not yet aired") == 0)
			return FINISHED;
		else {
			std::cerr << "Error: Unknown series status '" << s << "'" << std::endl;
			return SERIESSTATUS_INVALID;
		}
	}

	AnimeStatus anime_status_from_int(const int i) {
		switch (i) {
		case 1:
			return WATCHING;
		case 2:
			return COMPLETED;
		case 3:
			return ONHOLD;
		case 4:
			return DROPPED;
		case 6:
			return PLANTOWATCH;
		default:
			std::cerr << "Error: Unknown anime status " << i << std::endl;
			return ANIMESTATUS_INVALID;
		}
	}

	AnimeStatus anime_status_from_string(const std::string& s) {
		if (s.compare(to_string(WATCHING)) == 0)
			return WATCHING;
		else if (s.compare(to_string(COMPLETED)) == 0)
			return COMPLETED;
		else if (s.compare(to_string(ONHOLD)) == 0)
			return ONHOLD;
		else if (s.compare(to_string(DROPPED)) == 0)
			return DROPPED;
		else if (s.compare(to_string(PLANTOWATCH)) == 0)
			return PLANTOWATCH;
		else
			return ANIMESTATUS_INVALID;
	}


	std::string to_string(const SeriesType s) {
		switch (s) {
		case TV:
			return "TV";
		case OVA:
			return "OVA";
		case MOVIE:
			return "Movie";
		case SPECIAL:
			return "Special";
		case ONA:
			return "ONA";
		case MUSIC:
			return "Music";
		default:
			return "Invalid Type";
		}
	}

	std::string to_string(const SeriesStatus s) {
		switch (s) {
		case AIRING:
			return "Airing";
		case FINISHED:
			return "Finished";
		case NOTYETAIRED:
			return "Not Yet Aired";
		default:
			return "Invalid Status";
		}
	}

	std::string to_string(const AnimeStatus s) {
		switch (s) {
		case WATCHING:
			return "Watching";
		case COMPLETED:
			return "Completed";
		case ONHOLD:
			return "On Hold";
		case DROPPED:
			return "Dropped";
		case PLANTOWATCH:
			return "Plan To Watch";
		default:
			return "Invalid Status";
		}
	}

}
