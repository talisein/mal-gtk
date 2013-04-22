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
			std::cerr << "Error: Unknown Anime Series Type (" << i << ")" << std::endl;
			return SERIESTYPE_INVALID;
		}
	}

	SeriesType anime_series_type_from_string(const Glib::ustring& s) {
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
			std::cerr << "Error: Unknown Anime Series Type (" << s << ")" << std::endl;
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
			std::cerr << "Error: Unknown Anime Series Status (" << i << ")" << std::endl;
			return SERIESSTATUS_INVALID;
		}
	}

	SeriesStatus anime_series_status_from_string(const Glib::ustring& s) {
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
			std::cerr << "Error: Unknown Anime Series Status (" << s << ")" << std::endl;
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
			std::cerr << "Error: Unknown Anime Status (" << i << ")" << std::endl;
			return ANIMESTATUS_INVALID;
		}
	}

	AnimeStatus anime_status_from_string(const Glib::ustring& s) {
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


    Glib::ustring to_string(const SeriesType s) {
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

	Glib::ustring to_string(const SeriesStatus s) {
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

	Glib::ustring to_string(const AnimeStatus s) {
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

    std::shared_ptr<MALItem> Anime::clone() const {
        return std::make_shared<Anime>(*this);
    }

	void Anime::set_series_type(std::string&& str)
	{
		if (str.size() == 1)
			series_type = anime_series_type_from_int(std::stoi(str));
		else
			series_type = anime_series_type_from_string(str);
	}

	void Anime::set_series_status(std::string&& str)
	{
		if (str.size() == 1)
			series_status = anime_series_status_from_int(std::stoi(str));
		else
			series_status = anime_series_status_from_string(str);
	}

	void Anime::set_series_episodes(std::string&& str)
	{
		series_episodes = std::stoi(str);
	}

	void Anime::set_status(std::string&& str)
	{
		status = anime_status_from_int(std::stoi(str));
	}

	void Anime::set_episodes(std::string&& str)
	{
		episodes = std::stoi(str);
	}

	void Anime::set_rewatch_episode(std::string&& str)
	{
		rewatch_episode = std::stoi(str);
	}

	void Anime::set_storage_type(std::string&& str)
	{
		storage_type = std::stoi(str);
	}

	void Anime::set_storage_value(std::string&& str)
	{
		storage_value = std::stof(str);
	}
}
