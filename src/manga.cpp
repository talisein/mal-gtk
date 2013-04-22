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

#include "manga.hpp"
#include <iostream>

namespace MAL {
	MangaSeriesType manga_series_type_from_int(const int i) {
		switch(i) {
		case 1:
			return MANGA;
		case 2:
			return NOVEL;
		case 3:
			return ONESHOT;
		case 4:
			return DOUJIN;
		case 5:
			return MANHWA;
		case 6:
			return MANHUA;
        case 7:
            return OEL;
		default:
			std::cerr << "Error: Unknown manga series type " << i << std::endl;
			return MANGASERIESTYPE_INVALID;
		}
	}

	MangaSeriesType manga_series_type_from_string(const Glib::ustring& s) {
		if (s.compare(to_string(MANGA)) == 0)
			return MANGA;
		else if (s.compare(to_string(NOVEL)) == 0)
			return NOVEL;
		else if (s.compare(to_string(ONESHOT)) == 0)
			return ONESHOT;
		else if (s.compare(to_string(DOUJIN)) == 0)
			return DOUJIN;
		else if (s.compare(to_string(MANHWA)) == 0)
			return MANHWA;
		else if (s.compare(to_string(MANHUA)) == 0)
			return MANHUA;
        else if (s.compare(to_string(OEL)) == 0)
            return OEL;
		else {
			std::cerr << "Error: Unknown manga series type '" << s << "'" << std::endl;
			return MANGASERIESTYPE_INVALID;
		}
	}

	MangaSeriesStatus manga_series_status_from_int(const int i) {
		switch (i) {
		case 1:
			return PUBLISHING;
		case 2:
			return MANGAFINISHED;
		case 3:
			return NOTYETPUBLISHED;
		default:
			std::cerr << "Error: Unknown manga series status " << i << std::endl;
			return MANGASERIESSTATUS_INVALID;
		}
	}

	MangaSeriesStatus manga_series_status_from_string(const Glib::ustring& s) {
		if (s.compare(to_string(PUBLISHING)) == 0 ||
		    s.compare("Currently Publishing") == 0)
			return PUBLISHING;
		else if (s.compare(to_string(MANGAFINISHED)) == 0 ||
		    s.compare("Finished Publishing") == 0)
			return MANGAFINISHED;
		else if (s.compare(to_string(MANGAFINISHED)) == 0 ||
		    s.compare("Not yet published") == 0)
			return NOTYETPUBLISHED;
		else {
			std::cerr << "Error: Unknown manga series status '" << s << "'" << std::endl;
			return MANGASERIESSTATUS_INVALID;
		}
	}

	MangaStatus manga_status_from_int(const int i) {
		switch (i) {
		case 1:
			return READING;
		case 2:
			return MANGACOMPLETED;
		case 3:
			return MANGAONHOLD;
		case 4:
			return MANGADROPPED;
		case 6:
			return PLANTOREAD;
		default:
			std::cerr << "Error: Unknown manga status " << i << std::endl;
			return MANGASTATUS_INVALID;
		}
	}

	MangaStatus manga_status_from_string(const Glib::ustring& s) {
		if (s.compare(to_string(READING)) == 0)
			return READING;
		else if (s.compare(to_string(MANGACOMPLETED)) == 0)
			return MANGACOMPLETED;
		else if (s.compare(to_string(MANGAONHOLD)) == 0)
			return MANGAONHOLD;
		else if (s.compare(to_string(MANGADROPPED)) == 0)
			return MANGADROPPED;
		else if (s.compare(to_string(PLANTOREAD)) == 0)
			return PLANTOREAD;
		else
			return MANGASTATUS_INVALID;
	}


    Glib::ustring to_string(const MangaSeriesType s) {
		switch (s) {
		case MANGA:
			return "Manga";
		case NOVEL:
			return "Novel";
		case ONESHOT:
			return "One Shot";
		case DOUJIN:
			return "Doujin";
		case MANHWA:
			return "Manhwa";
		case MANHUA:
			return "Manhua";
		case OEL:
			return "OEL";
		default:
			return "Invalid Type";
		}
	}

    Glib::ustring to_string(const MangaSeriesStatus s) {
		switch (s) {
		case PUBLISHING:
			return "Publishing";
		case MANGAFINISHED:
			return "Finished";
		case NOTYETPUBLISHED:
			return "Not Yet Published";
		default:
			return "Invalid Status";
		}
	}

    Glib::ustring to_string(const MangaStatus s) {
		switch (s) {
		case READING:
			return "Reading";
		case MANGACOMPLETED:
			return "Completed";
		case MANGAONHOLD:
			return "On Hold";
		case MANGADROPPED:
			return "Dropped";
		case PLANTOREAD:
			return "Plan To Read";
		default:
			return "Invalid Status";
		}
	}

    std::shared_ptr<MALItem> Manga::clone() const {
        return std::make_shared<Manga>(*this);
    }

	void Manga::set_series_type(std::string&& str)
	{
		if (str.size() == 1)
			series_type = manga_series_type_from_int(std::stoi(str));
		else
			series_type = manga_series_type_from_string(str);
	}

	void Manga::set_series_chapters(std::string&& str)
	{
		series_chapters = std::stoi(str);
	}

	void Manga::set_series_volumes(std::string&& str)
	{
		series_volumes = std::stoi(str);
	}

	void Manga::set_series_status(std::string&& str)
	{
		if (str.size() == 1)
			series_status = manga_series_status_from_int(std::stoi(str));
		else
			series_status = manga_series_status_from_string(str);
	}

	void Manga::set_status(std::string&& str)
	{
		status = manga_status_from_int(std::stoi(str));
	}

	void Manga::set_chapters(std::string&& str)
	{
		chapters = std::stoi(str);
	}

	void Manga::set_volumes(std::string&& str)
	{
		volumes = std::stoi(str);
	}

	void Manga::set_rereading_chapter(std::string&& str)
	{
		rereading_chapter = std::stoi(str);
	}

	void Manga::set_retail_volumes(std::string&& str)
	{
		retail_volumes = std::stoi(str);
	}
}
