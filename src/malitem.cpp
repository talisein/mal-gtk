#include "malitem.hpp"
#include <iostream>

namespace MAL {

    MALItem::MALItem() :
        series_itemdb_id(0),
        id(0),
        score(0.f),
        enable_reconsuming(false),
        downloaded_items(0),
        times_consumed(0),
        reconsume_value(0),
        priority(0),
        enable_discussion(false)
    {
    }

	void MALItem::set_series_itemdb_id(const std::string& id)
	{
		series_itemdb_id = std::stoll(id);
	}

	void MALItem::set_series_title(const std::string& title)
	{
		series_title = title;
	}

	void MALItem::set_series_date_begin(const std::string& str)
	{
		series_date_begin = str;
	}

	void MALItem::set_series_date_end(const std::string& str)
	{
		series_date_end = str;
	}

	void MALItem::set_image_url(const std::string& str)
	{
		image_url = str;
	}

	void MALItem::set_series_synonyms(const std::string& str)
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

	void MALItem::set_series_synopsis(const std::string& str)
	{
		series_synopsis = str;
	}

    /* Non-Series Info */

	void MALItem::set_tags(const std::string& str)
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
		
	void MALItem::set_date_start(const std::string& str)
	{
		date_start = str;
	}

	void MALItem::set_date_finish(const std::string& str)
	{
		date_finish = str;
	}

	void MALItem::set_id(const std::string& str)
	{
		id = std::stoll(str);
	}

	void MALItem::set_last_updated(const std::string& str)
	{
		last_updated = std::stoull(str);
	}

	void MALItem::set_score(const std::string& str)
	{
		score = std::stof(str);
	}

	void MALItem::set_enable_reconsuming(const std::string& str)
	{
		enable_reconsuming = (str.size()==0||str.compare("0")==0)?false:true;
	}

	void MALItem::set_fansub_group(const std::string& str)
	{
		fansub_group = str;
	}

	void MALItem::set_comments(const std::string& str)
	{
		comments = str;
	}

	void MALItem::set_downloaded_items(const std::string& str)
	{
		downloaded_items = std::stoi(str);
	}

	void MALItem::set_times_consumed(const std::string& str)
	{
		times_consumed = std::stoi(str);
	}

	void MALItem::set_reconsume_value(const std::string& str)
	{
		reconsume_value = std::stoi(str);
	}

	void MALItem::set_priority(const std::string& str)
	{
		priority = std::stoi(str);
	}

	void MALItem::set_enable_discussion(const std::string& str)
	{
		enable_discussion = (str.size()==0||str.compare("0")==0)?false:true;
	}

    std::string MALItem::get_season_began() const {
        return get_season(series_date_begin);
    }

	std::string MALItem::get_season(Glib::Date&& date) const {
        if ( Glib::Date::valid_year(date.get_year()) ) {
            if ( Glib::Date::valid_month(date.get_month()) ) {
                switch (date.get_month()) {
                    case Glib::Date::DECEMBER:
                        date.add_years(1);
                    case Glib::Date::JANUARY:
                    case Glib::Date::FEBRUARY:
                        return "Winter " + std::to_string(date.get_year());
                    case Glib::Date::MARCH:
                    case Glib::Date::APRIL:
                    case Glib::Date::MAY:
                        return "Spring " + std::to_string(date.get_year());
                    case Glib::Date::JUNE:
                    case Glib::Date::JULY:
                    case Glib::Date::AUGUST:
                        return "Summer " + std::to_string(date.get_year());
                    case Glib::Date::SEPTEMBER:
                    case Glib::Date::OCTOBER:
                    case Glib::Date::NOVEMBER:
                        return "Autumn " + std::to_string(date.get_year());
                    case Glib::Date::BAD_MONTH:
                    default:
                        return std::to_string(date.get_year());
                }
            } else {
                return std::to_string(date.get_year());
            }
        } else {
            return "Unknown";
        }
	}

	std::string MALItem::get_season(const std::string& in) const {
        Glib::Date::Year year  = std::stoi(in.substr(0,4));
        const int        month = std::stoi(in.substr(5,2));

        if ( Glib::Date::valid_year(year) ) {
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
                case 0:
                default:
                    return std::to_string(year);
            }
        } else {
            return "Unknown";
        }
	}

    Glib::Date MALItem::get_date_began() const {
        return get_date(series_date_begin);
    }

    Glib::Date MALItem::get_date_ended() const {
        return get_date(series_date_end);
    }

    Glib::Date MALItem::get_date(const std::string& in) const {
        Glib::Date        out;
        Glib::Date::Day   day   = 0;
        Glib::Date::Month month = Glib::Date::BAD_MONTH;
        Glib::Date::Year  year  = 0;

        try {
            year = std::stoi(in.substr(0,4));
            day = std::stoi(in.substr(8,2));
            switch (std::stoi(in.substr(5,2))) {
                case 1:  month = Glib::Date::JANUARY;   break;
                case 2:  month = Glib::Date::FEBRUARY;  break;
                case 3:  month = Glib::Date::MARCH;     break;
                case 4:  month = Glib::Date::APRIL;     break;
                case 5:  month = Glib::Date::MAY;       break;
                case 6:  month = Glib::Date::JUNE;      break;
                case 7:  month = Glib::Date::JULY;      break;
                case 8:  month = Glib::Date::AUGUST;    break;
                case 9:  month = Glib::Date::SEPTEMBER; break;
                case 10: month = Glib::Date::OCTOBER;   break;
                case 11: month = Glib::Date::NOVEMBER;  break;
                case 12: month = Glib::Date::DECEMBER;  break;
                default: month = Glib::Date::BAD_MONTH; break;
            }
        } catch (std::exception e) {
        }
        if (Glib::Date::valid_year(year))
            out.set_year(year);
        else
            std::cerr << "Warning: " << series_title << " has invalid year '" << in << "'" << std::endl;
        if (Glib::Date::valid_month(month))
            out.set_month(month);
        if (Glib::Date::valid_day(day))
            out.set_day(day);

        return out;
    }

}
