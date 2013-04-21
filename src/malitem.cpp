#include "malitem.hpp"
#include <iostream>
#include <map>

namespace {
    std::string convert_html(std::string&& in) {
        static std::map<std::string, std::string> html_map = {
            {"br />", "\n"}, { "br/>", "\n" }
        };

        auto pos = in.find("<");
        decltype(pos) start = 0;
        while ( pos != std::string::npos ) {
            auto end_pos = in.find(">", pos);
            if (end_pos == std::string::npos)
                break;

            auto iter = html_map.find(in.substr(pos+1, end_pos - pos));
            if (iter != std::end(html_map)) {
                in.replace(pos, end_pos - pos + 1, iter->second);
            }
            start = pos + 1;
            pos = in.find("<", start);
        }

        return in;
    }

    std::string strip_excessive_newlines(std::string&& in) {
        auto pos = in.find("\n\n\n");
        while ( pos != std::string::npos ) {
            in.erase(pos, 1);
            pos = in.find("\n\n\n");
        }

        return in;
    }
}

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

    std::shared_ptr<MALItem> MALItem::clone() const {
        return std::make_shared<MALItem>(*this);
    }

	void MALItem::set_series_itemdb_id(std::string&& id)
	{
		series_itemdb_id = std::stoll(id);
	}

	void MALItem::set_series_title(std::string&& title)
	{
		series_title = title;
	}

	void MALItem::set_series_date_begin(std::string&& str)
	{
		series_date_begin = str;
	}

	void MALItem::set_series_date_end(std::string&& str)
	{
		series_date_end = str;
	}

	void MALItem::set_image_url(std::string&& str)
	{
		image_url = str;
	}

	void MALItem::set_series_synonyms(std::string&& str)
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

	void MALItem::set_series_synopsis(std::string&& str)
	{
		series_synopsis = strip_excessive_newlines(convert_html(std::move(str)));
	}

    /* Non-Series Info */

	void MALItem::set_tags(std::string&& str)
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
		
	void MALItem::set_date_start(std::string&& str)
	{
		date_start = str;
	}

	void MALItem::set_date_finish(std::string&& str)
	{
		date_finish = str;
	}

	void MALItem::set_id(std::string&& str)
	{
		id = std::stoll(str);
	}

	void MALItem::set_last_updated(std::string&& str)
	{
		last_updated = std::stoull(str);
	}

	void MALItem::set_score(std::string&& str)
	{
		score = std::stof(str);
	}

	void MALItem::set_enable_reconsuming(std::string&& str)
	{
		enable_reconsuming = (str.size()==0||str.compare("0")==0)?false:true;
	}

	void MALItem::set_fansub_group(std::string&& str)
	{
		fansub_group = str;
	}

	void MALItem::set_comments(std::string&& str)
	{
		comments = str;
	}

	void MALItem::set_downloaded_items(std::string&& str)
	{
		downloaded_items = std::stoi(str);
	}

	void MALItem::set_times_consumed(std::string&& str)
	{
		times_consumed = std::stoi(str);
	}

	void MALItem::set_reconsume_value(std::string&& str)
	{
		reconsume_value = std::stoi(str);
	}

	void MALItem::set_priority(std::string&& str)
	{
		priority = std::stoi(str);
	}

	void MALItem::set_enable_discussion(std::string&& str)
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
