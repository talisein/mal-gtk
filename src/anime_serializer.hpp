#pragma once
#include <list>
#include <libxml/parser.h>
#include "anime.hpp"

namespace MAL {

	class AnimeSerializer {
	public:
		AnimeSerializer() = default;
		~AnimeSerializer() = default;
		void operator=(const AnimeSerializer&) = delete;
		AnimeSerializer(const AnimeSerializer&) = delete;

		std::list<Anime> deserialize(const std::string& xml) const;
		std::string serialize(const Anime& anime) const;

	private:
	};

}
