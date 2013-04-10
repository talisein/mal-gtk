#pragma once
#include <functional>
#include <list>
#include <map>
#include "manga.hpp"

namespace MAL {

	enum MANGA_FIELDS : int_fast8_t;

	class MangaSerializer {
	public:
		MangaSerializer();
		~MangaSerializer() = default;
		void operator=(const MangaSerializer&) = delete;
		MangaSerializer(const MangaSerializer&) = delete;

		std::list<Manga> deserialize(const std::string& xml) const;
		std::string serialize(const Manga& anime) const;

	private:
		const std::map<const std::string, const MANGA_FIELDS> field_map;
		const std::map<const MANGA_FIELDS, std::function<void (MAL::Manga&, const std::string&)> > member_map;
	};

}
