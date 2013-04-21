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
