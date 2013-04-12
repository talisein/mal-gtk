#pragma once
#include <functional>
#include <list>
#include <map>
#include "anime.hpp"

namespace MAL {

	enum FIELDS : int_fast8_t;

	class AnimeSerializer {
	public:
		AnimeSerializer();
		~AnimeSerializer() = default;
		void operator=(const AnimeSerializer&) = delete;
		AnimeSerializer(const AnimeSerializer&) = delete;

		std::list<std::shared_ptr<Anime> > deserialize(const std::string& xml) const;
		std::string serialize(const Anime& anime) const;

	private:
		const std::map<const std::string, const FIELDS> field_map;
		const std::map<const FIELDS, std::function<void (Anime&, const std::string&)> > member_map;
	};

}
