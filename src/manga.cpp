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

	MangaSeriesType manga_series_type_from_string(const std::string& s) {
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

	MangaSeriesStatus manga_series_status_from_string(const std::string& s) {
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

	MangaStatus manga_status_from_string(const std::string& s) {
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


	std::string to_string(const MangaSeriesType s) {
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

	std::string to_string(const MangaSeriesStatus s) {
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

	std::string to_string(const MangaStatus s) {
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

	std::string manga_season_from_date(const std::string& s) {
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

	void Manga::set_series_mangadb_id(const std::string& id)
	{
		series_mangadb_id = std::stoll(id);
	}

	void Manga::set_series_title(const std::string& title)
	{
		series_title = title;
	}

	void Manga::set_series_type(const std::string& str)
	{
		if (str.size() == 1)
			series_type = manga_series_type_from_int(std::stoi(str));
		else
			series_type = manga_series_type_from_string(str);
	}

	void Manga::set_series_chapters(const std::string& str)
	{
		series_chapters = std::stoi(str);
	}

	void Manga::set_series_volumes(const std::string& str)
	{
		series_volumes = std::stoi(str);
	}

	void Manga::set_series_status(const std::string& str)
	{
		if (str.size() == 1)
			series_status = manga_series_status_from_int(std::stoi(str));
		else
			series_status = manga_series_status_from_string(str);
	}

	void Manga::set_series_date_begin(const std::string& str)
	{
		series_date_begin = str;
	}

	void Manga::set_series_date_end(const std::string& str)
	{
		series_date_end = str;
	}

	void Manga::set_image_url(const std::string& str)
	{
		image_url = str;
	}

	void Manga::set_series_synonyms(const std::string& str)
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

	void Manga::set_id(const std::string& str)
	{
		id = std::stoll(str);
	}

	void Manga::set_chapters(const std::string& str)
	{
		chapters = std::stoi(str);
	}

	void Manga::set_volumes(const std::string& str)
	{
		volumes = std::stoi(str);
	}

	void Manga::set_score(const std::string& str)
	{
		score = std::stof(str);
	}

	void Manga::set_date_start(const std::string& str)
	{
		date_start = str;
	}

	void Manga::set_date_finish(const std::string& str)
	{
		date_finish = str;
	}

	void Manga::set_status(const std::string& str)
	{
		status = manga_status_from_int(std::stoi(str));
	}

	void Manga::set_enable_rereading(const std::string& str)
	{
		enable_rereading = str.compare("0")==0?false:true;
	}

	void Manga::set_rereading_chapter(const std::string& str)
	{
		rereading_chapter = std::stoi(str);
	}

	void Manga::set_last_updated(const std::string& str)
	{
		last_updated = std::stoull(str);
	}

	void Manga::set_tags(const std::string& str)
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
		
	void Manga::set_series_synopsis(const std::string& str)
	{
		series_synopsis = str;
	}

	void Manga::set_scan_group(const std::string& str)
	{
		scan_group = str;
	}

	void Manga::set_comments(const std::string& str)
	{
		comments = str;
	}

	void Manga::set_retail_volumes(const std::string& str)
	{
		retail_volumes = std::stoi(str);
	}

	void Manga::set_downloaded_chapters(const std::string& str)
	{
		downloaded_chapters = std::stoi(str);
	}

	void Manga::set_times_reread(const std::string& str)
	{
		times_reread = std::stoi(str);
	}

	void Manga::set_reread_value(const std::string& str)
	{
		reread_value = std::stoi(str);
	}

	void Manga::set_priority(const std::string& str)
	{
		priority = std::stoi(str);
	}

	void Manga::set_enable_discussion(const std::string& str)
	{
		enable_discussion = str.compare("0")==0?false:true;
	}


}
