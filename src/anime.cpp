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

	std::string anime_season_from_date(const std::string& s) {
		int year = 0, month = 0;
        try {
            year = std::stoi(s.substr(0,4));
            month = std::stoi(s.substr(5,2));
        } catch (std::exception e) {
            return "Unknown";
        }

		if (year == 0)
			return "Unknown";
		else {
			switch (month) {
                case 12:
                    ++year; 
                case 1:
                case 2:
                    return "Winter " + std::to_string(year);
                case 3:
                case 4:
                case 5:
                    return "Spring " + std::to_string(year);
                case 6:
                case 7:
                case 8:
                    return "Summer " + std::to_string(year);
                case 9:
                case 10:
                case 11:
                    return "Autumn " + std::to_string(year);
                default:
                    return s.substr(0, 7);
			}
		}
	}

	void Anime::set_series_animedb_id(const std::string& id)
	{
		series_animedb_id = std::stoll(id);
	}

	void Anime::set_series_title(const std::string& title)
	{
		series_title = title;
	}

	void Anime::set_series_type(const std::string& str)
	{
		if (str.size() == 1)
			series_type = anime_series_type_from_int(std::stoi(str));
		else
			series_type = anime_series_type_from_string(str);
	}

	void Anime::set_series_episodes(const std::string& str)
	{
		series_episodes = std::stoi(str);
	}

	void Anime::set_series_status(const std::string& str)
	{
		if (str.size() == 1)
			series_status = anime_series_status_from_int(std::stoi(str));
		else
			series_status = anime_series_status_from_string(str);
	}

	void Anime::set_series_date_begin(const std::string& str)
	{
		series_date_begin = str;
	}

	void Anime::set_series_date_end(const std::string& str)
	{
		series_date_end = str;
	}

	void Anime::set_image_url(const std::string& str)
	{
		image_url = str;
	}

	void Anime::set_series_synonyms(const std::string& str)
	{
		auto iter = str.find("; ");
		decltype(iter) start = 0;
		while (iter != std::string::npos) {
			auto const substr = str.substr(start, iter - start);
			if (substr.size() > 0) {
				series_synonyms.insert(substr);
			}

			start = iter + 2;
			iter = str.find("; ", start);
		}
		auto const substr = str.substr(start, iter - start);
		if (substr.size() > 0)
			series_synonyms.insert(substr);
	}

	void Anime::set_series_synopsis(const std::string& str)
	{
		series_synopsis = str;
	}

	void Anime::set_id(const std::string& str)
	{
		id = std::stoll(str);
	}

	void Anime::set_status(const std::string& str)
	{
		status = anime_status_from_int(std::stoi(str));
	}

	void Anime::set_episodes(const std::string& str)
	{
		episodes = std::stoi(str);
	}

	void Anime::set_score(const std::string& str)
	{
		score = std::stof(str);
	}

	void Anime::set_downloaded_episodes(const std::string& str)
	{
		downloaded_episodes = std::stoi(str);
	}

	void Anime::set_storage_type(const std::string& str)
	{
		storage_type = std::stoi(str);
	}

	void Anime::set_storage_value(const std::string& str)
	{
		storage_value = std::stof(str);
	}

	void Anime::set_times_watched(const std::string& str)
	{
		times_watched = std::stoi(str);
	}

	void Anime::set_rewatch_value(const std::string& str)
	{
		rewatch_value = std::stoi(str);
	}

	void Anime::set_date_start(const std::string& str)
	{
		date_start = str;
	}

	void Anime::set_date_finish(const std::string& str)
	{
		date_finish = str;
	}

	void Anime::set_priority(const std::string& str)
	{
		priority = std::stoi(str);
	}

	void Anime::set_enable_discussion(const std::string& str)
	{
		enable_discussion = str.compare("0")==0?false:true;
	}

	void Anime::set_enable_rewatching(const std::string& str)
	{
		enable_rewatching = str.compare("0")==0?false:true;
	}

	void Anime::set_comments(const std::string& str)
	{
		comments = str;
	}

	void Anime::set_fansub_group(const std::string& str)
	{
		fansub_group = str;
	}

	void Anime::set_tags(const std::string& str)
	{
		auto iter = str.find("; ");
		decltype(iter) start = 0;
		while (iter != str.npos) {
			auto const substr = str.substr(start, iter - start);
			if (substr.size() > 0) {
				tags.insert(substr);
			}

			start = iter + 2;
			iter = str.find("; ", start);
		}
		auto const substr = str.substr(start, iter - start);
		if (substr.size() > 0)
			tags.insert(substr);
	}
		
	void Anime::set_rewatch_episode(const std::string& str)
	{
		rewatch_episode = std::stoi(str);
	}

	void Anime::set_last_updated(const std::string& str)
	{
		last_updated = std::stoull(str);
	}
}
