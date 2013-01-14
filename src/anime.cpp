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
}
