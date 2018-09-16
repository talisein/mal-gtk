/* -*- mode: c++; c-file-style: "linux"; c-basic-offset: 4; indent-tabs-mode: nil; -*-
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
#include <string>
#include <stdexcept>

namespace MAL {
    SeriesType
    anime_series_type(const int i)
    {
        switch(i) {
        case 1:
            return SeriesType::TV;
        case 2:
            return SeriesType::OVA;
        case 3:
            return SeriesType::MOVIE;
        case 4:
            return SeriesType::SPECIAL;
        case 5:
            return SeriesType::ONA;
        case 6:
            return SeriesType::MUSIC;
        case 0:
            return SeriesType::INVALID;
        default:
            std::cerr << "Error: Unknown Anime Series Type (" << i << ")" << std::endl;
            return SeriesType::INVALID;
        }
    }

    SeriesStatus
    anime_series_status(const int i)
    {
        switch (i) {
        case 1:
            return SeriesStatus::AIRING;
        case 2:
            return SeriesStatus::FINISHED;
        case 3:
            return SeriesStatus::NOTYETAIRED;
        case 0:
            return SeriesStatus::INVALID;
        default:
            std::cerr << "Error: Unknown Anime Series Status (" << i << ")" << std::endl;
            return SeriesStatus::INVALID;
        }
    }

    AnimeStatus
    anime_status(const int i)
    {
        switch (i) {
        case 1:
            return AnimeStatus::WATCHING;
        case 2:
            return AnimeStatus::COMPLETED;
        case 3:
            return AnimeStatus::ONHOLD;
        case 4:
            return AnimeStatus::DROPPED;
        case 6:
            return AnimeStatus::PLANTOWATCH;
        case 0:
            return AnimeStatus::INVALID;
        default:
            std::cerr << "Error: Unknown Anime Status (" << i << ")" << std::endl;
            return AnimeStatus::INVALID;
        }
    }


    AnimeStorageType
    anime_storage_type(const int i)
    {
        switch (i) {
        case 0:
            return AnimeStorageType::INVALID;
        case 1:
            return AnimeStorageType::HARDDRIVE;
        case 2:
            return AnimeStorageType::DVDCD;
        case 3:
            return AnimeStorageType::NONE;
        case 4:
            return AnimeStorageType::RETAILDVD;
        case 5:
            return AnimeStorageType::VHS;
        case 6:
            return AnimeStorageType::EXTERNALHD;
        case 7:
            return AnimeStorageType::NAS;
        default:
            std::cerr << "Error: Unknown Anime Storage Type (" << i << ")" << std::endl;
            return AnimeStorageType::INVALID;
        }
    }

    SeriesType
    anime_series_type(const Glib::ustring& s)
    {
        if (s == to_string(SeriesType::TV) )
            return SeriesType::TV;
        else if (s == to_string(SeriesType::OVA))
            return SeriesType::OVA;
        else if (s == to_string(SeriesType::MOVIE))
            return SeriesType::MOVIE;
        else if (s == to_string(SeriesType::SPECIAL))
            return SeriesType::SPECIAL;
        else if (s == to_string(SeriesType::ONA))
            return SeriesType::ONA;
        else if (s == to_string(SeriesType::MUSIC))
            return SeriesType::MUSIC;
        else if (s == to_string(SeriesType::INVALID))
            return SeriesType::INVALID;
        else {
            std::cerr << "Error: Unknown Anime Series Type (" << s << ")" << std::endl;
            return SeriesType::INVALID;
        }
    }

    SeriesStatus
    anime_series_status(const Glib::ustring& s)
    {
        if (s == to_string(SeriesStatus::AIRING) || s == "Currently Airing")
            return SeriesStatus::AIRING;
        else if (s == to_string(SeriesStatus::FINISHED) || s == "Finished Airing")
            return SeriesStatus::FINISHED;
        else if (s == to_string(SeriesStatus::NOTYETAIRED) || s == "Not yet aired")
            return SeriesStatus::NOTYETAIRED;
        else if (s == to_string(SeriesStatus::INVALID))
            return SeriesStatus::INVALID;
        else {
            std::cerr << "Error: Unknown Anime Series Status (" << s << ")" << std::endl;
            return SeriesStatus::INVALID;
        }
    }

    AnimeStatus
    anime_status(const Glib::ustring& s)
    {
        if (s == to_string(AnimeStatus::WATCHING))
            return AnimeStatus::WATCHING;
        else if (s == to_string(AnimeStatus::COMPLETED))
            return AnimeStatus::COMPLETED;
        else if (s == to_string(AnimeStatus::ONHOLD))
            return AnimeStatus::ONHOLD;
        else if (s == to_string(AnimeStatus::DROPPED))
            return AnimeStatus::DROPPED;
        else if (s == to_string(AnimeStatus::PLANTOWATCH))
            return AnimeStatus::PLANTOWATCH;
        else if (s == to_string(AnimeStatus::NONE))
            return AnimeStatus::INVALID;
        else if (s == to_string(AnimeStatus::INVALID))
            return AnimeStatus::INVALID;
        else {
            std::cerr << "Error: Unknown Anime Status (" << s << ")" << std::endl;
            return AnimeStatus::INVALID;
        }
    }

    AnimeStorageType
    anime_storage_type(const Glib::ustring& s)
    {
        if (s == to_string(AnimeStorageType::INVALID))
            return AnimeStorageType::INVALID;
        else if (s == to_string(AnimeStorageType::HARDDRIVE))
            return AnimeStorageType::HARDDRIVE;
        else if (s == to_string(AnimeStorageType::DVDCD))
            return AnimeStorageType::DVDCD;
        else if (s == to_string(AnimeStorageType::NONE))
            return AnimeStorageType::NONE;
        else if (s == to_string(AnimeStorageType::RETAILDVD))
            return AnimeStorageType::RETAILDVD;
        else if (s == to_string(AnimeStorageType::VHS))
            return AnimeStorageType::VHS;
        else if (s == to_string(AnimeStorageType::EXTERNALHD))
            return AnimeStorageType::EXTERNALHD;
        else if (s == to_string(AnimeStorageType::NAS))
            return AnimeStorageType::NAS;
        else {
            std::cerr << "Error: Unknown Anime Storage Type (" << s << ")" << std::endl;
            return AnimeStorageType::INVALID;
        }
    }


    Glib::ustring
    to_string(const SeriesType s)
    {
        switch (s) {
        case SeriesType::TV:
            return "TV";
        case SeriesType::OVA:
            return "OVA";
        case SeriesType::MOVIE:
            return "Movie";
        case SeriesType::SPECIAL:
            return "Special";
        case SeriesType::ONA:
            return "ONA";
        case SeriesType::MUSIC:
            return "Music";
        case SeriesType::INVALID:
        default:
            return "Unknown";
        }
    }

    Glib::ustring
    to_string(const SeriesStatus s)
    {
        switch (s) {
        case SeriesStatus::AIRING:
            return "Airing";
        case SeriesStatus::FINISHED:
            return "Finished";
        case SeriesStatus::NOTYETAIRED:
            return "Not Yet Aired";
        case SeriesStatus::INVALID:
        default:
            return "Unknown";
        }
    }

    Glib::ustring
    to_string(const AnimeStatus s)
    {
        switch (s) {
        case AnimeStatus::WATCHING:
            return "Watching";
        case AnimeStatus::COMPLETED:
            return "Completed";
        case AnimeStatus::ONHOLD:
            return "On Hold";
        case AnimeStatus::DROPPED:
            return "Dropped";
        case AnimeStatus::PLANTOWATCH:
            return "Plan To Watch";
        case AnimeStatus::INVALID:
        case AnimeStatus::NONE:
        default:
            return "Unknown";
        }
    }

    Glib::ustring
    to_string(const AnimeStorageType s)
    {
        switch (s) {
        case AnimeStorageType::INVALID:
            return "Invalid Anime Storage";
        case AnimeStorageType::HARDDRIVE:
            return "Hard Drive";
        case AnimeStorageType::DVDCD:
            return "DVD/CD";
        case AnimeStorageType::NONE:
            return "None";
        case AnimeStorageType::RETAILDVD:
            return "Retail DVD";
        case AnimeStorageType::VHS:
            return "VHS";
        case AnimeStorageType::EXTERNALHD:
            return "External HD";
        case AnimeStorageType::NAS:
            return "NAS";
        default:
            return "Invalid Anime Storage";
        }
    }

    std::shared_ptr<MALItem>
    Anime::clone() const
    {
        return std::make_shared<Anime>(*this);
    }

    void
    Anime::serialize(XmlWriter& writer) const
    {
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
        series_type     {SeriesType::INVALID},
        series_status   {SeriesStatus::INVALID},
        series_episodes {0},
        status          {AnimeStatus::INVALID},
        episodes        {0},
        rewatch_episode {0},
        storage_type    {AnimeStorageType::INVALID},
        storage_value   {0.}
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
        series_type(SeriesType::INVALID),
        series_status(SeriesStatus::INVALID),
        status(AnimeStatus::INVALID),
        storage_type(AnimeStorageType::INVALID)
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

    void
    Anime::update_from_details(const std::shared_ptr<MALItem>& details)
    {
        MALItem::update_from_details(details);
        auto anime_details = std::static_pointer_cast<Anime>(details);
        storage_type       = anime_details->storage_type;
        storage_value      = anime_details->storage_value;
    }

    void
    Anime::update_from_list(const std::shared_ptr<MALItem>& item)
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

            has_details     = false;
        }
    }

    void
    Anime::set_series_type(const std::string&& str)
    {
        if (str.size() == 1) {
            try {
                series_type = anime_series_type(std::stoi(str));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid argument converting series type \""
                          << str << "\" to an integer: "
                          << e.what() << std::endl;
                series_type = SeriesType::INVALID;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Converting series type \"" << str
                          << "\" to int falls outside the expected range: "
                          << e.what() << std::endl;
                series_type = SeriesType::INVALID;
            }
        } else {
            series_type = anime_series_type(str);
        }
    }

    void
    Anime::set_series_status(const std::string&& str)
    {
        constexpr int status_invalid = static_cast<int>(SeriesStatus::INVALID);
        if (str.size() == 1) {
            int s = status_invalid;
            try {
                s = std::stoi(str);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid argument converting series type \""
                          << str << "\" to an integer: "
                          << e.what() << std::endl;
                s = status_invalid;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Converting \"" << str
                          << "\" to int falls outside the expected range: "
                          << e.what() << std::endl;
                s = status_invalid;
            }
            series_status = anime_series_status(s);
        } else {
            series_status = anime_series_status(str);
        }
    }

    void
    Anime::set_series_episodes(const std::string&& str)
    {
        if (str.size() > 0)
            series_episodes = std::stoi(str);
        else
            series_episodes = 0;
    }

    void
    Anime::set_status(const std::string&& str)
    {
        if (str.size() == 1)
            status = anime_status(std::stoi(str));
        else
            status = anime_status(str);
    }

    void
    Anime::set_episodes(const std::string&& str)
    {
        if (str.size() > 0)
            episodes = std::stoi(str);
        else
            episodes = 0;
    }

    void
    Anime::set_rewatch_episode(const std::string&& str)
    {
        try {
            rewatch_episode = std::stoi(str);
        } catch (const std::exception& e) {
            rewatch_episode = 0;
        }
    }

    void
    Anime::set_storage_type(const std::string&& str)
    {
        if (str.size() == 1)
            storage_type = anime_storage_type(std::stoi(str));
        else
            storage_type = anime_storage_type(str);
    }

    void
    Anime::set_storage_value(const std::string&& str)
    {
        storage_value = std::stof(str);
    }
}
