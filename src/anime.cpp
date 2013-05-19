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
        case 0:
            return SERIESTYPE_INVALID;
		default:
            std::cerr << "Error: Unknown Anime Series Type (" << i << ")" << std::endl;
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
        case 0:
            return SERIESSTATUS_INVALID;
		default:
            std::cerr << "Error: Unknown Anime Series Status (" << i << ")" << std::endl;
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
        case 0:
            return ANIMESTATUS_INVALID;
		default:
            std::cerr << "Error: Unknown Anime Status (" << i << ")" << std::endl;
			return ANIMESTATUS_INVALID;
		}
	}


    AnimeStorageType anime_storage_type_from_int(const int i) {
       switch (i) {
           case 0:
               return ANIME_STORAGE_INVALID;
           case 1:
               return ANIME_STORAGE_HARDDRIVE;
           case 2:
               return ANIME_STORAGE_DVDCD;
           case 3:
               return ANIME_STORAGE_NONE;
           case 4:
               return ANIME_STORAGE_RETAILDVD;
           case 5:
               return ANIME_STORAGE_VHS;
           case 6:
               return ANIME_STORAGE_EXTERNALHD;
           case 7:
               return ANIME_STORAGE_NAS;
           default:
               std::cerr << "Error: Unknown Anime Storage Type (" << i << ")" << std::endl;
               return ANIME_STORAGE_INVALID;
       }
    }

	SeriesType anime_series_type_from_string(const Glib::ustring& s) {
		if (s == to_string(TV) )
			return TV;
		else if (s == to_string(OVA))
			return OVA;
		else if (s == to_string(MOVIE))
			return MOVIE;
		else if (s == to_string(SPECIAL))
			return SPECIAL;
		else if (s == to_string(ONA))
			return ONA;
		else if (s == to_string(MUSIC))
			return MUSIC;
        else if (s == to_string(SERIESTYPE_INVALID))
            return SERIESTYPE_INVALID;
		else {
			std::cerr << "Error: Unknown Anime Series Type (" << s << ")" << std::endl;
			return SERIESTYPE_INVALID;
		}
	}

	SeriesStatus anime_series_status_from_string(const Glib::ustring& s) {
		if (s == to_string(AIRING) || s == "Currently Airing")
			return AIRING;
		else if (s == to_string(FINISHED) || s == "Finished Airing")
			return FINISHED;
        else if (s == to_string(NOTYETAIRED) || s == "Not yet aired")
            return NOTYETAIRED;
        else if (s == to_string(SERIESSTATUS_INVALID))
            return SERIESSTATUS_INVALID;
		else {
			std::cerr << "Error: Unknown Anime Series Status (" << s << ")" << std::endl;
			return SERIESSTATUS_INVALID;
		}
	}

	AnimeStatus anime_status_from_string(const Glib::ustring& s) {
		if (s == to_string(WATCHING))
			return WATCHING;
		else if (s == to_string(COMPLETED))
			return COMPLETED;
		else if (s == to_string(ONHOLD))
			return ONHOLD;
		else if (s == to_string(DROPPED))
			return DROPPED;
		else if (s == to_string(PLANTOWATCH))
			return PLANTOWATCH;
        else if (s == to_string(ANIMESTATUS_INVALID))
            return ANIMESTATUS_INVALID;
		else {
			std::cerr << "Error: Unknown Anime Status (" << s << ")" << std::endl;
			return ANIMESTATUS_INVALID;
        }
	}

	AnimeStorageType anime_storage_type_from_string(const Glib::ustring& s) {
		if (s == to_string(ANIME_STORAGE_INVALID))
			return ANIME_STORAGE_INVALID;
		else if (s == to_string(ANIME_STORAGE_HARDDRIVE))
			return ANIME_STORAGE_HARDDRIVE;
		else if (s == to_string(ANIME_STORAGE_DVDCD))
			return ANIME_STORAGE_DVDCD;
		else if (s == to_string(ANIME_STORAGE_NONE))
			return ANIME_STORAGE_NONE;
		else if (s == to_string(ANIME_STORAGE_RETAILDVD))
			return ANIME_STORAGE_RETAILDVD;
		else if (s == to_string(ANIME_STORAGE_VHS))
			return ANIME_STORAGE_VHS;
		else if (s == to_string(ANIME_STORAGE_EXTERNALHD))
			return ANIME_STORAGE_EXTERNALHD;
		else if (s == to_string(ANIME_STORAGE_NAS))
			return ANIME_STORAGE_NAS;
		else {
			std::cerr << "Error: Unknown Anime Storage Type (" << s << ")" << std::endl;
			return ANIME_STORAGE_INVALID;
        }
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
			return "Unknown";
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
			return "Unknown";
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
			return "Unknown";
		}
	}

	Glib::ustring to_string(const AnimeStorageType s) {
		switch (s) {
		case ANIME_STORAGE_INVALID:
			return "Invalid Anime Storage";
		case ANIME_STORAGE_HARDDRIVE:
			return "Hard Drive";
		case ANIME_STORAGE_DVDCD:
			return "DVD/CD";
		case ANIME_STORAGE_NONE:
			return "None";
		case ANIME_STORAGE_RETAILDVD:
			return "Retail DVD";
		case ANIME_STORAGE_VHS:
			return "VHS";
		case ANIME_STORAGE_EXTERNALHD:
			return "External HD";
		case ANIME_STORAGE_NAS:
			return "NAS";
		default:
			return "Invalid Anime Storage";
		}
	}

    std::shared_ptr<MALItem> Anime::clone() const {
        return std::make_shared<Anime>(*this);
    }

    void Anime::serialize(XmlWriter& writer) const {
        writer.startElement("anime");
        writer.writeAttribute("version", "1");
        MALItem::serialize(writer);
        using std::to_string;
        writer.writeElement("series_type",     to_string(series_type));
        writer.writeElement("series_status",   to_string(series_status));
        writer.writeElement("series_episodes", to_string(series_episodes));
        writer.writeElement("status",          to_string(status));
        writer.writeElement("episodes",        to_string(episodes));
        writer.writeElement("rewatch_episode", to_string(rewatch_episode));
        writer.writeElement("storage_type",    to_string(storage_type));
        writer.writeElement("storage_value",   to_string(storage_value));
        writer.endElement();
    }

    Anime::Anime() :
        MALItem(),
        series_type(SERIESTYPE_INVALID),
        series_status(SERIESSTATUS_INVALID),
        status(ANIMESTATUS_INVALID),
        storage_type(ANIME_STORAGE_INVALID)
    {
    }

    namespace {
        enum FIELDS { FIELD_INVALID, SERIES_TYPE, SERIES_STATUS,
                      SERIES_EPISODES, STATUS, EPISODES, REWATCH_EPISODE,
                      STORAGE_TYPE, STORAGE_VALUE };

        const std::map<std::string, FIELDS> field_map = {
            {"series_type",     SERIES_TYPE},
            {"series_status",   SERIES_STATUS},
            {"series_episodes", SERIES_EPISODES},
            {"status",          STATUS},
            {"episodes",        EPISODES},
            {"rewatch_episode", REWATCH_EPISODE},
            {"storage_type",    STORAGE_TYPE},
            {"storage_value",   STORAGE_VALUE},
        };

        const std::map<FIELDS, decltype(std::mem_fn(&Anime::set_series_type))> method_map = {
            { SERIES_TYPE,     std::mem_fn(&Anime::set_series_type)},
            { SERIES_STATUS,   std::mem_fn(&Anime::set_series_status)},
            { SERIES_EPISODES, std::mem_fn(&Anime::set_series_episodes)},
            { STATUS,          std::mem_fn(&Anime::set_status)},
            { EPISODES,        std::mem_fn(&Anime::set_episodes)},
            { REWATCH_EPISODE, std::mem_fn(&Anime::set_rewatch_episode)},
            { STORAGE_TYPE,    std::mem_fn(&Anime::set_storage_type)},
            { STORAGE_VALUE,   std::mem_fn(&Anime::set_storage_value)}
        };
    }

    Anime::Anime(XmlReader& reader) :
        MALItem(reader),
        series_type(SERIESTYPE_INVALID),
        series_status(SERIESSTATUS_INVALID),
        status(ANIMESTATUS_INVALID),
        storage_type(ANIME_STORAGE_INVALID)
    {
        reader.read();
        if (reader.get_name() != "series_type") {
            std::cerr << "Error: Deserializing anime started on unexpected field: " << reader.get_name() << std::endl;
            return;
        }

        FIELDS field = FIELD_INVALID;
        while ( !(reader.get_name() == "anime" && reader.get_type() == XML_READER_TYPE_END_ELEMENT) ) {
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

    void Anime::update_from_details(const std::shared_ptr<MALItem>& details)
    {
        MALItem::update_from_details(details);
        auto anime_details = std::static_pointer_cast<Anime>(details);
        storage_type       = anime_details->storage_type;
        storage_value      = anime_details->storage_value;
    }

    void Anime::update_from_list(const std::shared_ptr<MALItem>& item)
    {
        MALItem::update_from_list(item);
        auto anime = std::static_pointer_cast<Anime>(item);
        if (series_itemdb_id == anime->series_itemdb_id) {
            series_type     = anime->series_type;
            series_status   = anime->series_status;
            series_episodes = anime->series_episodes;
            
            // TODO: Handle conflicts
            status          = anime->status;
            episodes        = anime->episodes;
            rewatch_episode = anime->rewatch_episode;
        }
    }

	void Anime::set_series_type(std::string&& str)
	{
		if (str.size() == 1)
			try {
                series_type = anime_series_type_from_int(std::stoi(str));
            } catch (std::exception e) {
                std::cerr << "Error converting from int: " << e.what() << std::endl;
            }
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
        if (str.size() > 0)
            series_episodes = std::stoi(str);
        else
            series_episodes = 0;
	}

	void Anime::set_status(std::string&& str)
	{
        if (str.size() == 1)
            status = anime_status_from_int(std::stoi(str));
        else
            status = anime_status_from_string(str);
	}

	void Anime::set_episodes(std::string&& str)
	{
        if (str.size() > 0)
            episodes = std::stoi(str);
        else
            episodes = 0;
	}

	void Anime::set_rewatch_episode(std::string&& str)
	{
		rewatch_episode = std::stoi(str);
	}

	void Anime::set_storage_type(std::string&& str)
	{
        if (str.size() == 1)
            storage_type = anime_storage_type_from_int(std::stoi(str));
        else
            storage_type = anime_storage_type_from_string(str);
	}

	void Anime::set_storage_value(std::string&& str)
	{
		storage_value = std::stof(str);
	}
}
