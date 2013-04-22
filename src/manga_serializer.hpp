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
#include <functional>
#include <list>
#include <map>
#include "manga.hpp"
#include "text_util.hpp"

namespace MAL {

	enum MANGA_FIELDS : int_fast8_t;

	class MangaSerializer {
	public:
		MangaSerializer(const std::shared_ptr<TextUtility>&);
		~MangaSerializer() = default;
		void operator=(const MangaSerializer&) = delete;
		MangaSerializer(const MangaSerializer&) = delete;

		std::list<std::shared_ptr<Manga> > deserialize(const std::string& xml) const;
		std::string serialize(const Manga& anime) const;

	private:
		const std::map<const std::string, const MANGA_FIELDS> field_map;
		const std::map<const MANGA_FIELDS, std::function<void (MAL::Manga&, std::string&&)> > member_map;

        std::shared_ptr<TextUtility> m_text_util;
	};

}
