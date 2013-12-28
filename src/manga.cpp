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

    MangaStorageType manga_storage_type_from_int(const int i) {
        switch (i) {
            case -1:
                return MANGA_STORAGE_INVALID;
            case 0:
                return MANGA_STORAGE_NONE;
            case 1:
                return MANGA_STORAGE_HARDDRIVE;
            case 2:
                return MANGA_STORAGE_EXTERNALHD;
            case 3:
                return MANGA_STORAGE_DVDCD;
            case 4:
                return MANGA_STORAGE_RETAILMANGA;
            case 5:
                return MANGA_STORAGE_MAGAZINE;
            case 6:
                return MANGA_STORAGE_NAS;
            default:
                std::cerr << "Error: Unknown manga storage type '" << i << "'" << std::endl;
                return MANGA_STORAGE_INVALID;
        }
    }

    MangaStorageType manga_storage_type_from_string(const std::string& s) {
        if (s == to_string(MANGA_STORAGE_INVALID))
            return MANGA_STORAGE_INVALID;
        else if (s == to_string(MANGA_STORAGE_NONE))
            return MANGA_STORAGE_NONE;
        else if (s == to_string(MANGA_STORAGE_HARDDRIVE))
            return MANGA_STORAGE_HARDDRIVE;
        else if (s == to_string(MANGA_STORAGE_EXTERNALHD))
            return MANGA_STORAGE_EXTERNALHD;
        else if (s == to_string(MANGA_STORAGE_DVDCD))
            return MANGA_STORAGE_DVDCD;
        else if (s == to_string(MANGA_STORAGE_RETAILMANGA))
            return MANGA_STORAGE_RETAILMANGA;
        else if (s == to_string(MANGA_STORAGE_MAGAZINE))
            return MANGA_STORAGE_MAGAZINE;
        else if (s == to_string(MANGA_STORAGE_NAS))
            return MANGA_STORAGE_NAS;
        else {
            std::cerr << "Error: Unknown manga storage type '" << s << "s" << std::endl;
            return MANGA_STORAGE_INVALID;
        }
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

    Glib::ustring to_string(const MangaStorageType s) {
        switch (s) {
            case MANGA_STORAGE_INVALID:
                return "Invalid Storage Type";
            case MANGA_STORAGE_NONE:
                return "None";
            case MANGA_STORAGE_HARDDRIVE:
                return "Harddrive";
            case MANGA_STORAGE_EXTERNALHD:
                return "External HD";
            case MANGA_STORAGE_DVDCD:
                return "DVD/CD";
            case MANGA_STORAGE_RETAILMANGA:
                return "Retail Manga";
            case MANGA_STORAGE_MAGAZINE:
                return "Magazine";
            case MANGA_STORAGE_NAS:
                return "NAS";
            default:
                return "Invalid Storage Type";
        }
    }

    std::shared_ptr<MALItem> Manga::clone() const {
        return std::make_shared<Manga>(*this);
    }

    void Manga::serialize(XmlWriter& writer) const
    {
        writer.startElement("manga");
        writer.writeAttribute("version", "1");
        MALItem::serialize(writer);
        using std::to_string;
        writer.writeElement("series_type",       to_string(series_type));
        writer.writeElement("series_status",     to_string(series_status));
        writer.writeElement("series_chapters",   to_string(static_cast<int>(series_chapters)));
        writer.writeElement("series_volumes",    to_string(static_cast<int>(series_volumes)));
        writer.writeElement("status",            to_string(status));
        writer.writeElement("chapters",          to_string(static_cast<int>(chapters)));
        writer.writeElement("volumes",           to_string(static_cast<int>(volumes)));
        writer.writeElement("rereading_chapter", to_string(static_cast<int>(rereading_chapter)));
        writer.writeElement("retail_volumes",    to_string(static_cast<int>(retail_volumes)));
        writer.writeElement("storage_type",      to_string(storage_type));
        writer.endElement();
    }

    namespace {
        enum FIELDS { FIELD_INVALID, SERIES_TYPE, SERIES_STATUS,
                      SERIES_CHAPTERS, SERIES_VOLUMES, STATUS, CHAPTERS,
                      VOLUMES, REREADING_CHAPTER, RETAIL_VOLUMES, STORAGE_TYPE };

        const std::map<std::string, FIELDS> field_map = {
            {"series_type",       SERIES_TYPE},
            {"series_status",     SERIES_STATUS},
            {"series_chapters",   SERIES_CHAPTERS},
            {"series_volumes",    SERIES_VOLUMES},
            {"status",            STATUS},
            {"chapters",          CHAPTERS},
            {"volumes",           VOLUMES},
            {"rereading_chapter", REREADING_CHAPTER},
            {"retail_volumes",    RETAIL_VOLUMES},
            {"storage_type",      STORAGE_TYPE}
        };

        const std::map<FIELDS, decltype(std::mem_fn(&Manga::set_series_type))> method_map = {
            { SERIES_TYPE,       std::mem_fn(&Manga::set_series_type)},
            { SERIES_STATUS,     std::mem_fn(&Manga::set_series_status)},
            { SERIES_CHAPTERS,   std::mem_fn(&Manga::set_series_chapters)},
            { SERIES_VOLUMES,    std::mem_fn(&Manga::set_series_volumes)},
            { STATUS,            std::mem_fn(&Manga::set_status)},
            { CHAPTERS,          std::mem_fn(&Manga::set_chapters)},
            { VOLUMES,           std::mem_fn(&Manga::set_volumes)},
            { REREADING_CHAPTER, std::mem_fn(&Manga::set_rereading_chapter)},
            { RETAIL_VOLUMES,    std::mem_fn(&Manga::set_retail_volumes)},
            { STORAGE_TYPE,      std::mem_fn(&Manga::set_storage_type)}
        };
    }

    Manga::Manga() :
        MALItem(),
        series_type       {MANGASERIESTYPE_INVALID},
        series_status     {MANGASERIESSTATUS_INVALID},
        series_chapters   {0},
        series_volumes    {0},
        status            {MANGASTATUS_INVALID},
        chapters          {0},
        volumes           {0},
        rereading_chapter {0},
        retail_volumes    {0},
        storage_type      {MANGA_STORAGE_INVALID}
    {
    }

    Manga::Manga(XmlReader& reader) :
        MALItem(reader),
        series_type(MANGASERIESTYPE_INVALID),
        series_status(MANGASERIESSTATUS_INVALID),
        status(MANGASTATUS_INVALID),
        storage_type(MANGA_STORAGE_INVALID)
    {
        reader.read();
        if (reader.get_name() != "series_type") {
            std::cerr << "Error: Deserializing manga started on unexpected field: " << reader.get_name() << std::endl;
            return;
        }

        FIELDS field = FIELD_INVALID;
        while ( !(reader.get_name() == "manga" && reader.get_type() == XML_READER_TYPE_END_ELEMENT) ) {
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
        reader.read();

    }

    void Manga::update_from_details(const std::shared_ptr<MALItem>& details)
    {
        MALItem::update_from_details(details);
        auto manga_details = std::static_pointer_cast<Manga>(details);
        retail_volumes     = manga_details->retail_volumes;
        storage_type       = manga_details->storage_type;
    }

    void Manga::update_from_list(const std::shared_ptr<MALItem>& item)
    {
        MALItem::update_from_list(item);
        auto manga = std::static_pointer_cast<Manga>(item);
        if (series_itemdb_id == manga->series_itemdb_id) {
            series_type       = manga->series_type;
            series_status     = manga->series_status;
            series_chapters   = manga->series_chapters;
            series_volumes    = manga->series_volumes;
            
            // TODO: Handle conflicts
            status            = manga->status;
            chapters          = manga->chapters;
            volumes           = manga->volumes;
            rereading_chapter = manga->rereading_chapter;
        }
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
        if (str.size() == 1)
            status = manga_status_from_int(std::stoi(str));
        else
            status = manga_status_from_string(str);
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

    void Manga::set_storage_type(std::string&& str)
    {
        if (str.size() == 1)
            storage_type = manga_storage_type_from_int(std::stoi(str));
        else
            storage_type = manga_storage_type_from_string(str);
    }
}
