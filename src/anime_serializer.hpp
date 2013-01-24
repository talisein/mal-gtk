#pragma once
#include <list>
#include <map>
#include <libxml/parser.h>
#include <functional>
#include "anime.hpp"

namespace MAL {
	enum FIELDS { FIELDNONE,
	              ANIMEDBID, SERIESTITLE, SERIESTYPE, SERIESEPISODES, SERIESSTATUS,
	              SERIESDATEBEGIN, SERIESDATEEND, SERIESIMAGEURL, SERIESSYNONYMS,
	              ANIME, ENTRY, MYID, MYWATCHEDEPISODES, MYSTARTDATE, MYFINISHDATE, MYSCORE,
	              MYSTATUS, MYREWATCHING, MYREWATCHINGEP, MYLASTUPDATED, MYTAGS,
	              USERID, SYNOPSIS
	};

	class AnimeSerializer {
	public:
		AnimeSerializer();
		~AnimeSerializer() = default;
		void operator=(const AnimeSerializer&) = delete;
		AnimeSerializer(const AnimeSerializer&) = delete;

		std::list<Anime> deserialize(const std::string& xml) const;
		std::string serialize(const Anime& anime) const;
		const std::map<const std::string, const FIELDS> field_map;
		const std::map<const FIELDS, const decltype(std::mem_fn(&Anime::set_id))> member_map;
	private:
	};

}
