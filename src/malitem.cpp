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

#include "malitem.hpp"
#include <iostream>
#include <map>
#include <algorithm>


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
    
    Priority priority_from_int(const int i)
    {
        switch (i) {
            case 0: return PRIORITY_LOW;
            case 1: return PRIORITY_MEDIUM;
            case 2: return PRIORITY_HIGH;
            default:
                std::cerr << "Error: Unknown priority (" << i << ")" << std::endl;
                return PRIORITY_INVALID;
        }
    }
    
    ReconsumeValue reconsume_value_from_int(const int i)
    {
        switch (i) {
            case 1:
                return RECONSUME_VALUE_VERYLOW;
            case 2:
                return RECONSUME_VALUE_LOW;
            case 3:
                return RECONSUME_VALUE_MEDIUM;
            case 4:
                return RECONSUME_VALUE_HIGH;
            case 5:
                return RECONSUME_VALUE_VERYHIGH;
            case 0:
                return RECONSUME_VALUE_INVALID;
            default:
                std::cerr << "Error: Unknown reconsume value (" << i << ")" << std::endl;
                return RECONSUME_VALUE_INVALID;
        }
    }

	Priority priority_from_string(const Glib::ustring& s) {
		if (s.compare(to_string(PRIORITY_INVALID)) == 0)
			return PRIORITY_INVALID;
        else if (s.empty())
            return PRIORITY_INVALID;
		else if (s.compare(to_string(PRIORITY_LOW)) == 0)
			return PRIORITY_LOW;
		else if (s.compare(to_string(PRIORITY_MEDIUM)) == 0)
			return PRIORITY_MEDIUM;
		else if (s.compare(to_string(PRIORITY_HIGH)) == 0)
			return PRIORITY_HIGH;
		else {
			std::cerr << "Error: Unknown Priority (" << s << ")" << std::endl;
			return PRIORITY_INVALID;
		}
	}

	ReconsumeValue reconsume_value_from_string(const Glib::ustring& s) {
		if (s == to_string(RECONSUME_VALUE_INVALID))
			return RECONSUME_VALUE_INVALID;
		else if (s == to_string(RECONSUME_VALUE_VERYLOW))
			return RECONSUME_VALUE_VERYLOW;
		else if (s == to_string(RECONSUME_VALUE_LOW))
			return RECONSUME_VALUE_LOW;
		else if (s == to_string(RECONSUME_VALUE_MEDIUM))
			return RECONSUME_VALUE_MEDIUM;
		else if (s == to_string(RECONSUME_VALUE_HIGH))
			return RECONSUME_VALUE_HIGH;
		else if (s == to_string(RECONSUME_VALUE_VERYHIGH))
			return RECONSUME_VALUE_VERYHIGH;
		else {
			std::cerr << "Error: Unknown Reconsume Value (" << s << ")" << std::endl;
			return RECONSUME_VALUE_INVALID;
		}
	}

	Glib::ustring to_string(const Priority p) {
		switch (p) {
		case PRIORITY_INVALID:
			return "Invalid Priority";
		case PRIORITY_LOW:
			return "Low";
		case PRIORITY_MEDIUM:
			return "Medium";
		case PRIORITY_HIGH:
			return "High";
		default:
            std::cerr << "Error: Unknown priority (" << p << ")" << std::endl;
			return "Unknown";
		}
	}

	Glib::ustring to_string(const ReconsumeValue r) {
		switch (r) {
		case RECONSUME_VALUE_INVALID:
			return "Invalid Reconsume Value";
		case RECONSUME_VALUE_VERYLOW:
			return "Very Low";
		case RECONSUME_VALUE_LOW:
			return "Low";
		case RECONSUME_VALUE_MEDIUM:
			return "Medium";
		case RECONSUME_VALUE_HIGH:
			return "High";
		case RECONSUME_VALUE_VERYHIGH:
			return "Very High";
		default:
            std::cerr << "Error: Unknown reconsume value (" << r << ")" << std::endl;
			return "Unknown Reconsume Value";
		}
	}

    MALItem::MALItem() :
        series_itemdb_id   {0},
        id                 {0},
        score              {0.},
        enable_reconsuming {false},
        downloaded_items   {0},
        times_consumed     {0},
        reconsume_value    {RECONSUME_VALUE_INVALID},
        priority           {PRIORITY_INVALID},
        enable_discussion  {false},
        has_details        {false}
    {
    }

    namespace {
        enum FIELDS { 
            FIELD_INVALID, SERIES_ITEMDB_ID, SERIES_TITLE, SERIES_PREFERRED_TITLE,
            SERIES_DATE_BEGIN, SERIES_DATE_END, IMAGE_URL, SERIES_SYNONYMS,
            SERIES_SYNONYM, SERIES_SYNOPSIS, TAGS, TAG, DATE_FINISH,
            DATE_START, ID, LAST_UPDATED, SCORE, ENABLE_RECONSUMING,
            FANSUB_GROUP, COMMENTS, DOWNLOADED_ITEMS, TIMES_CONSUMED,
            RECONSUME_VALUE, PRIORITY, ENABLE_DISCUSSION, HAS_DETAILS
        };
        std::map<std::string, FIELDS> field_map = {
            {"series_itemdb_id",       SERIES_ITEMDB_ID},
            {"series_title",           SERIES_TITLE},
            {"series_preferred_title", SERIES_PREFERRED_TITLE},
            {"series_date_begin",      SERIES_DATE_BEGIN},
            {"series_date_end",        SERIES_DATE_END},
            {"image_url",              IMAGE_URL},
            {"series_synonyms",        FIELD_INVALID},
            {"series_synonym",         SERIES_SYNONYM},
            {"series_synopsis",        SERIES_SYNOPSIS},
            {"tags",                   FIELD_INVALID},
            {"tag",                    TAG},
            {"date_finish",            DATE_FINISH},
            {"date_start",             DATE_START},
            {"id",                     ID},
            {"last_updated",           LAST_UPDATED},
            {"score",                  SCORE},
            {"enable_reconsuming",     ENABLE_RECONSUMING},
            {"fansub_group",           FANSUB_GROUP},
            {"comments",               COMMENTS},
            {"downloaded_items",       DOWNLOADED_ITEMS},
            {"times_consumed",         TIMES_CONSUMED},
            {"reconsume_value",        RECONSUME_VALUE},
            {"priority",               PRIORITY},
            {"enable_discussion",      ENABLE_DISCUSSION},
            {"has_details",            HAS_DETAILS}
        };

        std::map<FIELDS, decltype(std::mem_fn(&MALItem::set_series_title))> method_map = {
            {SERIES_ITEMDB_ID,       std::mem_fn(&MALItem::set_series_itemdb_id)},
            {SERIES_TITLE,           std::mem_fn(&MALItem::set_series_title)},
            {SERIES_PREFERRED_TITLE, std::mem_fn(&MALItem::set_series_preferred_title)},
            {SERIES_DATE_BEGIN,      std::mem_fn(&MALItem::set_series_date_begin)},
            {SERIES_DATE_END,        std::mem_fn(&MALItem::set_series_date_end)},
            {IMAGE_URL,              std::mem_fn(&MALItem::set_image_url)},
            {SERIES_SYNONYMS,        std::mem_fn(&MALItem::set_series_synonyms)},
            {SERIES_SYNONYM,         std::mem_fn(&MALItem::add_series_synonym)},
            {SERIES_SYNOPSIS,        std::mem_fn(&MALItem::set_series_synopsis)},
            {TAGS,                   std::mem_fn(&MALItem::set_tags)},
            {TAG,                    std::mem_fn(&MALItem::add_tag)},
            {DATE_FINISH,            std::mem_fn(&MALItem::set_date_finish)},
            {DATE_START,             std::mem_fn(&MALItem::set_date_start)},
            {ID,                     std::mem_fn(&MALItem::set_id)},
            {LAST_UPDATED,           std::mem_fn(&MALItem::set_last_updated)},
            {SCORE,                  std::mem_fn(&MALItem::set_score)},
            {ENABLE_RECONSUMING,     std::mem_fn(&MALItem::set_enable_reconsuming)},
            {FANSUB_GROUP,           std::mem_fn(&MALItem::set_fansub_group)},
            {COMMENTS,               std::mem_fn(&MALItem::set_comments)},
            {DOWNLOADED_ITEMS,       std::mem_fn(&MALItem::set_downloaded_items)},
            {TIMES_CONSUMED,         std::mem_fn(&MALItem::set_times_consumed)},
            {RECONSUME_VALUE,        std::mem_fn(&MALItem::set_reconsume_value)},
            {PRIORITY,               std::mem_fn(&MALItem::set_priority)},
            {ENABLE_DISCUSSION,      std::mem_fn(&MALItem::set_enable_discussion)},
            {HAS_DETAILS,            std::mem_fn(&MALItem::set_has_details)}
            
        };
    }

    MALItem::MALItem(XmlReader& reader) :
        reconsume_value(RECONSUME_VALUE_INVALID),
        priority(PRIORITY_INVALID)
    {
        reader.read();
        if (reader.get_name() != "MALitem") {
            std::cerr << "MALItem constructor called with reader in incorrect state: " << reader.get_name() << std::endl;
            return;
        }

        reader.read();
        FIELDS field = FIELD_INVALID;
        while ( !(reader.get_name() == "MALitem" && reader.get_type() == XML_READER_TYPE_END_ELEMENT) ) {
            switch (reader.get_type())
            {
                case XML_READER_TYPE_ELEMENT: {
                    auto iter = field_map.find(reader.get_name());
                    if (iter != field_map.end()) {
                        field = iter->second;
                    } else {
                        std::cerr << "Warning: Unexpected field: " << reader.get_name() << std::endl;
                        field = FIELD_INVALID;
                    }
                }
                    break;
                case XML_READER_TYPE_TEXT:
                    if (field != FIELD_INVALID) {
                        auto method_iter = method_map.find(field);
                        if (method_iter != method_map.end()) {
                            method_iter->second(this, reader.get_value());
                        }
                    }
                    break;
                default:
                    break;
            }
            reader.read();
        }
    }

    std::shared_ptr<MALItem> MALItem::clone() const
    {
        return std::make_shared<MALItem>(*this);
    }

    void MALItem::serialize(XmlWriter& writer) const
    {
        writer.startElement("MALitem");
        writer.writeAttribute("version", "1");
        using std::to_string;

        writer.writeElement("series_itemdb_id",   to_string(series_itemdb_id));
        writer.writeElement("series_title",                 series_title);
        writer.writeElement("series_preferred_title",       series_preferred_title);
        writer.writeElement("series_date_begin",            series_date_begin);
        writer.writeElement("series_date_end",              series_date_end);
        writer.writeElement("image_url",                    image_url);

        writer.startElement("series_synonyms");
        std::for_each(std::begin(series_synonyms), std::end(series_synonyms),
                      std::bind(&XmlWriter::writeElement,
                                std::ref(writer),
                                "series_synonym",
                                std::placeholders::_1));
        writer.endElement();

        writer.writeElement("series_synopsis",              series_synopsis );

        writer.startElement("tags");
        std::for_each(std::begin(tags), std::end(tags),
                      std::bind(&XmlWriter::writeElement,
                                std::ref(writer),
                                "tag",
                                std::placeholders::_1));
        writer.endElement();

        writer.writeElement("date_start",                   date_start);
        writer.writeElement("date_finish",                  date_finish);
        writer.writeElement("id",                 to_string(id));
        writer.writeElement("last_updated",       to_string(last_updated));
        writer.writeElement("score",              to_string(score));
        writer.writeElement("enable_reconsuming", to_string(enable_reconsuming));
        writer.writeElement("fansub_group",                 fansub_group);
        writer.writeElement("comments",                     comments);
        writer.writeElement("downloaded_items",   to_string(downloaded_items));
        writer.writeElement("times_consumed",     to_string(times_consumed));
        writer.writeElement("reconsume_value",    to_string(reconsume_value));
        writer.writeElement("priority",           to_string(priority));
        writer.writeElement("enable_discussion",  to_string(enable_discussion));
        writer.writeElement("has_details",        to_string(has_details));
        writer.endElement();
    }
    
    void MALItem::update_from_details(const std::shared_ptr<MALItem>& details)
    {
        fansub_group      = details->fansub_group;
        comments          = details->comments;
        downloaded_items  = details->downloaded_items;
        times_consumed    = details->times_consumed;
        reconsume_value   = details->reconsume_value;
        priority          = details->priority;
        enable_discussion = details->enable_discussion;
        has_details       = true;
    }

    namespace {
        template<typename T>
        static inline auto absdiff(const T l, const T r)->decltype(l-r)
        {
            if (l > r)
                return l - r;
            else
                return r - l;
        }
    }

    void MALItem::update_from_list(const std::shared_ptr<MALItem>& item)
    {
        if (series_itemdb_id == item->series_itemdb_id) {
            if (absdiff (last_updated, item->last_updated) > 3)
                has_details = false;

            series_title       = item->series_title;
            series_date_begin  = item->series_date_begin;
            series_date_end    = item->series_date_end;
            image_url          = item->image_url;
            id                 = item->id;
            last_updated       = item->last_updated;
            series_synonyms.insert(item->series_synonyms.begin(), item->series_synonyms.end());
            tags           .insert(item->tags.begin(),            item->tags.end());
            
            // TODO: handle conflicts for these:
            date_start         = item->date_start;
            date_finish        = item->date_finish;
            score              = item->score;
            enable_reconsuming = item->enable_reconsuming;
        } else {
            std::cerr << "Error: Attempted to update " << series_title << " from " << item->series_title << std::endl;
        }
    }

	void MALItem::set_series_itemdb_id(std::string&& itemdb_id)
	{
		series_itemdb_id = std::stoll(itemdb_id);
	}

	void MALItem::set_series_title(std::string&& title)
	{
		series_title = std::move(title);
	}

	void MALItem::set_series_preferred_title(std::string&& title)
	{
		series_title = std::move(title);
	}

	void MALItem::set_series_date_begin(std::string&& str)
	{
		series_date_begin = std::move(str);
	}

	void MALItem::set_series_date_end(std::string&& str)
	{
		series_date_end = std::move(str);
	}

	void MALItem::set_image_url(std::string&& str)
	{
		image_url = std::move(str);
	}

    void MALItem::add_series_synonym(std::string&& str)
    {
        series_synonyms.insert(std::move(str));
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

    void MALItem::add_tag(std::string&& tag)
    {
        tags.insert(std::move(tag));
    }

	void MALItem::set_tags(std::string&& str)
	{
		auto iter = str.find(", ");
		decltype(iter) start = 0;
		while (iter != str.npos) {
			auto const substr = str.substr(start, iter - start);
			if (substr.size() > 0) {
				tags.insert(substr);
			}

			start = iter + 2;
			iter = str.find(", ", start);
		}
		auto const substr = str.substr(start, iter - start);
		if (substr.size() > 0)
			tags.insert(substr);
	}
		
	void MALItem::set_date_start(std::string&& str)
	{
		date_start = std::move(str);
	}

	void MALItem::set_date_finish(std::string&& str)
	{
		date_finish = std::move(str);
	}

	void MALItem::set_id(std::string&& str)
	{
		id = std::stoll(str);
	}

	void MALItem::set_last_updated(std::string&& str)
	{
		last_updated = static_cast<std::time_t>(std::stoull(str));
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
		fansub_group = std::move(str);
	}

	void MALItem::set_comments(std::string&& str)
	{
		comments = std::move(str);
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
        if (str.size() == 1)
            reconsume_value = reconsume_value_from_int(std::stoi(str));
        else
            reconsume_value = reconsume_value_from_string(str);
	}

	void MALItem::set_priority(std::string&& str)
	{
        if (str.size() == 1)
            priority = priority_from_int(std::stoi(str));
        else
            priority = priority_from_string(str);
	}

	void MALItem::set_enable_discussion(std::string&& str)
	{
		enable_discussion = (str.size()==0||str.compare("0")==0)?false:true;
	}

	void MALItem::set_has_details(std::string&& str)
	{
		has_details = (str.size()==0||str.compare("0")==0)?false:true;
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
        if (Glib::Date::valid_month(month))
            out.set_month(month);
        if (Glib::Date::valid_day(day))
            out.set_day(day);

        return out;
    }

}
