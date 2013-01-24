#include "anime_serializer.hpp"
#include <iostream>
#include <libxml/xmlreader.h>
#include <cstring>
#include <memory>

namespace {
	static std::map<const std::string, const MAL::FIELDS> initialize_field_map() {
		return {
			{ "my_id",                    MAL::MYID },
			{ "series_animedb_id",        MAL::ANIMEDBID },
			{ "id",                       MAL::ANIMEDBID },
			{ "series_title",             MAL::SERIESTITLE },
			{ "title",                    MAL::SERIESTITLE },
			{ "series_type",              MAL::SERIESTYPE },
			{ "type",                     MAL::SERIESTYPE },
			{ "series_episodes",          MAL::SERIESEPISODES },
			{ "episodes",                 MAL::SERIESEPISODES },
			{ "series_status",            MAL::SERIESSTATUS },
			{ "status",                   MAL::SERIESSTATUS },
			{ "series_start",             MAL::SERIESDATEBEGIN },
			{ "start_date",               MAL::SERIESDATEBEGIN },
			{ "series_end",               MAL::SERIESDATEEND },
			{ "end_date",                 MAL::SERIESDATEEND },
			{ "series_image",             MAL::SERIESIMAGEURL },
			{ "image",                    MAL::SERIESIMAGEURL },
			{ "series_synonyms",          MAL::SERIESSYNONYMS },
			{ "synonyms",                 MAL::SERIESSYNONYMS },
			{ "my_score",                 MAL::MYSCORE },
			{ "score",                    MAL::MYSCORE },
			{ "english",                  MAL::SERIESSYNONYMS },
			{ "my_watched_episodes",      MAL::MYWATCHEDEPISODES },
			{ "my_start_date",            MAL::MYSTARTDATE },
			{ "my_finish_date",           MAL::MYFINISHDATE },
			{ "my_status",                MAL::MYSTATUS },
			{ "my_rewatching",            MAL::MYREWATCHING },
			{ "my_rewatching_ep",         MAL::MYREWATCHINGEP },
			{ "my_last_updated",          MAL::MYLASTUPDATED },
			{ "my_tags",                  MAL::MYTAGS },
			{ "user_id",                  MAL::USERID },
			{ "synopsis",                 MAL::SYNOPSIS },
			{ "user_name",                MAL::FIELDNONE },
			{ "user_watching",            MAL::FIELDNONE },
			{ "user_completed",           MAL::FIELDNONE },
			{ "user_onhold",              MAL::FIELDNONE },
			{ "user_dropped",             MAL::FIELDNONE },
			{ "user_plantowatch",         MAL::FIELDNONE },
			{ "user_days_spent_watching", MAL::FIELDNONE },
			{ "myinfo",                   MAL::FIELDNONE },
			{ "myanimelist",              MAL::FIELDNONE },
			{ "entry",                    MAL::ENTRY },
			{ "anime",                    MAL::ANIME }
		};
	}

	static std::map<const MAL::FIELDS, const decltype(std::mem_fn(&MAL::Anime::set_id))> initialize_member_map() {
		return {
			{ MAL::ANIMEDBID,         std::mem_fn(&MAL::Anime::set_series_animedb_id) },
			{ MAL::SERIESTITLE,       std::mem_fn(&MAL::Anime::set_series_title) },
			{ MAL::SERIESTYPE,        std::mem_fn(&MAL::Anime::set_series_type) },
			{ MAL::SERIESEPISODES,    std::mem_fn(&MAL::Anime::set_series_episodes) },
			{ MAL::SERIESSTATUS,      std::mem_fn(&MAL::Anime::set_series_status) },
			{ MAL::SERIESDATEBEGIN,   std::mem_fn(&MAL::Anime::set_series_date_begin) },
			{ MAL::SERIESDATEEND,     std::mem_fn(&MAL::Anime::set_series_date_end) },
			{ MAL::SERIESIMAGEURL,    std::mem_fn(&MAL::Anime::set_image_url) },
			{ MAL::SERIESSYNONYMS,    std::mem_fn(&MAL::Anime::set_series_synonyms) },
			{ MAL::MYID,              std::mem_fn(&MAL::Anime::set_id) },
			{ MAL::MYWATCHEDEPISODES, std::mem_fn(&MAL::Anime::set_episodes) },
			{ MAL::MYSTARTDATE,       std::mem_fn(&MAL::Anime::set_date_start) },
			{ MAL::MYFINISHDATE,      std::mem_fn(&MAL::Anime::set_date_finish) },
			{ MAL::MYSCORE,           std::mem_fn(&MAL::Anime::set_score) },
			{ MAL::MYSTATUS,          std::mem_fn(&MAL::Anime::set_status) },
			{ MAL::MYREWATCHING,      std::mem_fn(&MAL::Anime::set_enable_rewatching) },
			{ MAL::MYREWATCHINGEP,    std::mem_fn(&MAL::Anime::set_rewatch_episode) },
			{ MAL::MYLASTUPDATED,     std::mem_fn(&MAL::Anime::set_last_updated) },
			{ MAL::MYTAGS,            std::mem_fn(&MAL::Anime::set_tags) },
			{ MAL::SYNOPSIS,          std::mem_fn(&MAL::Anime::set_series_synopsis) },
		};
	}
}

namespace MAL {

	struct xmlTextReaderDeleter {
		void operator()(xmlTextReaderPtr reader) const {
			xmlFreeTextReader(reader);
		}
	};

	AnimeSerializer::AnimeSerializer() :
		field_map(initialize_field_map()),
		member_map(initialize_member_map())
	{
	}

	std::list<Anime> AnimeSerializer::deserialize(const std::string& xml) const {
		std::list<Anime> res;
		std::unique_ptr<char[]> cstr(new char[xml.size()]);
		std::memcpy(cstr.get(), xml.c_str(), xml.size());
		std::unique_ptr<xmlTextReader, xmlTextReaderDeleter> reader(xmlReaderForMemory(
			         cstr.get(), xml.size(), "", "UTF-8", XML_PARSE_RECOVER | XML_PARSE_NOENT ));

		if (!reader) {
			std::cerr << "Error: Couldn't create XML reader" << std::endl;
			return res;
		}

		int ret = 1;
		FIELDS field = FIELDNONE;
		Anime anime;
		bool entry_after_anime = false;
		bool seen_anime = false;
		bool seen_entry = false;
		for( ret = xmlTextReaderRead(reader.get()); ret == 1;
		     ret = xmlTextReaderRead(reader.get()) ) {
			const xmlChar *name  = xmlTextReaderConstName(reader.get());
			const xmlChar *value = xmlTextReaderConstValue(reader.get());
			const int node_type  = xmlTextReaderNodeType(reader.get());

			if (name) {
				const std::string name_str(reinterpret_cast<const char*>(name));
				auto iter = field_map.find(name_str);
				if (iter != field_map.end()) {
					field = iter->second;
				} else {
					if (name_str.compare("#text") != 0)
						std::cerr << "Unexpected field " << name_str << std::endl;
				}

				if (node_type == 1) {         // Element start
					if (field == ENTRY) {
						if (seen_anime && !seen_entry) 
							entry_after_anime = true;
						seen_entry = true;
					} else if (field == ANIME) {
						seen_anime = true;
					}

				} else if (node_type == 15) { // End Element
					if (entry_after_anime) {
						if (field == ENTRY) {
							res.push_back(anime);
							anime = Anime();
						}
					} else {
						if (field == ANIME) {
							res.push_back(anime);
							anime = Anime();
						}
					}
					field = FIELDNONE;

				} else if (value) {           // Text
					const std::string value_str(reinterpret_cast<const char*>(value));
					if (name_str.compare("#text") == 0) {
						auto iter = member_map.find(field);
						if ( iter != member_map.end() ) {
							iter->second(anime, value_str);
						}
					} else {
						std::cerr << "Error: Unexpected " << name_str << " = " 
						          << value_str << std::endl;
					}
				}
			}
		}

		if ( ret != 0 ) {
			std::cerr << "Error: Failed to parse! ret = " << ret << std::endl;
		}

		return res;
	}

	std::string AnimeSerializer::serialize(const Anime& anime) const {
		std::string out;
		
		out += "<?xml version=\"1.0\" encoding=\"UTF-8\"?><entry>";
		out += "<episode>";
		out += std::to_string(anime.episodes);
		out += "</episode>";
		out += "<status>";
		out += std::to_string(anime.status);
		out += "</status>";
		out += "<score>";
		out += std::to_string(anime.score);
		out += "</score>";
		out += "<downloaded_episodes>";
		out += std::to_string(anime.downloaded_episodes);
		out += "</downloaded_episodes>";
		out += "<storage_type>";
		//out += std::to_string(anime.storage_type);
		out += "</storage_type>";
		out += "<storage_value>";
		//out += std::to_string(anime.storage_value);
		out += "</storage_value>";
		out += "<times_rewatched>";
		//out += std::to_string(anime.times_watched);
		out += "</times_rewatched>";
		out += "<rewatch_value>";
		//out += std::to_string(anime.rewatch_value);
		out += "</rewatch_value>";
		out += "<date_start>";
		out += anime.date_start;
		out += "</date_start>";
		out += "<date_finish>";
		out += anime.date_finish;
		out += "</date_finish>";
		out += "<priority>";
		//out += std::to_string(anime.priority);
		out += "</priority>";
		out += "<enable_discussion>";
		//out += anime.enable_discussion?"1":"0";
		out += "</enable_discussion>";
		out += "<enable_rewatching>";
		out += (anime.enable_rewatching)?"1":"0";
		out += "</enable_rewatching>";
		out += "<comments>";
		//out += anime.comments;
		out += "</comments>";
		out += "<fansub_group>";
		//out += std::to_string(anime.fansub_group);
		out += "</fansub_group>";
		out += "<tags>";
		auto iter = anime.tags.begin();
		bool was_first = true;
		while (iter != anime.tags.end()) {
			if (!was_first)
				out += "; ";
			out += *iter;
			was_first = false;
			++iter;
		}
		out += "</tags>";
		out += "<rewatch_episode>";
		out += std::to_string(anime.rewatch_episode);
		out += "</rewatch_episode>";
		out += "</entry>";
		return out;
	}
	
}
