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
#include <cstdint>
#include "malitem.hpp"

namespace MAL {
	enum class AnimeStatus : std::int_fast8_t {
            INVALID     = -1,
            NONE        =  0,
            WATCHING    =  1,
            COMPLETED   =  2,
            ONHOLD      =  3,
            DROPPED     =  4,
            PLANTOWATCH =  6,
	};

	enum class SeriesStatus : std::int_fast8_t {
            INVALID     = -1,
            AIRING      =  1,
            FINISHED    =  2,
            NOTYETAIRED =  3,
	};

	enum class SeriesType : std::int_fast8_t {
            INVALID     = -1,
	        TV          =  1,
	        OVA         =  2,
	        MOVIE       =  3,
	        SPECIAL     =  4,
	        ONA         =  5,
            MUSIC       =  6,
	};
	                  
	enum class AnimeStorageType : std::int_fast8_t {
            INVALID     =  0,
            HARDDRIVE   =  1,
            DVDCD       =  2,
            NONE        =  3,
            RETAILDVD   =  4,
            VHS         =  5,
            EXTERNALHD  =  6,
            NAS         =  7,
    };

    Glib::ustring to_string(const SeriesType s);
    Glib::ustring to_string(const SeriesStatus s);
    Glib::ustring to_string(const AnimeStatus s);
    Glib::ustring to_string(const AnimeStorageType s);

	SeriesType anime_series_type(const int i);
	SeriesType anime_series_type(const Glib::ustring& s);

	SeriesStatus anime_series_status(const int i);
	SeriesStatus anime_series_status(const Glib::ustring& s);

	AnimeStatus anime_status(const int i);
	AnimeStatus anime_status(const Glib::ustring& s);

    AnimeStorageType anime_storage_type(const int i);
    AnimeStorageType anime_storage_type(const Glib::ustring& s);

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

		void set_series_type         (const std::string&&);
		void set_series_status       (const std::string&&);
		void set_series_episodes     (const std::string&&);

		void set_status              (const std::string&&);
		void set_episodes            (const std::string&&);
        void set_rewatch_episode     (const std::string&&);

		void set_storage_type        (const std::string&&);
		void set_storage_value       (const std::string&&);
	};
}
