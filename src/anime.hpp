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

#pragma once
#include <string>
#include <ctime>
#include <set>
#include <glibmm/ustring.h>
#include "malitem.hpp"

namespace MAL {
	enum AnimeStatus { ANIMESTATUS_INVALID = -1,
	                   WATCHING            = 1,
	                   COMPLETED           = 2,
	                   ONHOLD              = 3,
	                   DROPPED             = 4,
	                   PLANTOWATCH         = 6
	};

	enum SeriesStatus { SERIESSTATUS_INVALID = -1,
	                    AIRING               = 1,
	                    FINISHED             = 2,
	                    NOTYETAIRED          = 3,
	};

	enum SeriesType { SERIESTYPE_INVALID = -1,
	                  TV                 = 1,
	                  OVA                = 2,
	                  MOVIE              = 3,
	                  SPECIAL            = 4,
	                  ONA                = 5,
	                  MUSIC              = 6
	};
	                  
	enum AnimeStorageType {
        ANIME_STORAGE_INVALID    = 0,
        ANIME_STORAGE_HARDDRIVE  = 1,
        ANIME_STORAGE_DVDCD      = 2,
        ANIME_STORAGE_NONE       = 3,
        ANIME_STORAGE_RETAILDVD  = 4,
        ANIME_STORAGE_VHS        = 5,
        ANIME_STORAGE_EXTERNALHD = 6,
        ANIME_STORAGE_NAS        = 7
    };

    Glib::ustring to_string(const SeriesType s);
    Glib::ustring to_string(const SeriesStatus s);
    Glib::ustring to_string(const AnimeStatus s);
    Glib::ustring to_string(const AnimeStorageType s);

	SeriesType anime_series_type_from_int(const int i);
	SeriesType anime_series_type_from_string(const Glib::ustring& s);

	SeriesStatus anime_series_status_from_int(const int i);
	SeriesStatus anime_series_status_from_string(const Glib::ustring& s);

	AnimeStatus anime_status_from_int(const int i);
	AnimeStatus anime_status_from_string(const Glib::ustring& s);

    AnimeStorageType anime_storage_type_from_string(const Glib::ustring& s);

	class Anime final : public MALItem {
	public:
		Anime();
        Anime(XmlReader& reader);
        virtual std::shared_ptr<MALItem> clone() const override;
        virtual void serialize(XmlWriter&) const override;
		
		SeriesType            series_type;
		SeriesStatus          series_status;
		int_fast16_t          series_episodes;

		AnimeStatus           status;          // We know how to serialize /
		int_fast16_t          episodes;        // deserialize these fields
		int_fast16_t          rewatch_episode; //

		AnimeStorageType      storage_type;    // We know how to serialize
		float                 storage_value;   // these fields, but not
                                               // deserialize

        virtual void update_from_details (const std::shared_ptr<MALItem>& details) override;
        virtual void update_from_list (const std::shared_ptr<MALItem>& item) override;

		void set_series_type         (std::string&&);
		void set_series_status       (std::string&&);
		void set_series_episodes     (std::string&&);

		void set_status              (std::string&&);
		void set_episodes            (std::string&&);
        void set_rewatch_episode     (std::string&&);

		void set_storage_type        (std::string&&);
		void set_storage_value       (std::string&&);
	};
}
