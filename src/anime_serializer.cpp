#include "anime_serializer.hpp"
#include <iostream>
#include <libxml/xmlreader.h>
#include <cstring>
#include <memory>

namespace MAL {
	enum FIELDS { FIELDNONE, 
	              ANIMEDBID, SERIESTITLE, SERIESTYPE, SERIESEPISODES, SERIESSTATUS,
	              SERIESDATEBEGIN, SERIESDATEEND, SERIESIMAGEURL, SERIESSYNONYMS,
	              ANIME, MYID, MYWATCHEDEPISODES, MYSTARTDATE, MYFINISHDATE, MYSCORE,
	              MYSTATUS, MYREWATCHING, MYREWATCHINGEP, MYLASTUPDATED, MYTAGS,
	              USERID, SYNOPSIS
	};

	struct xmlTextReaderDeleter {
		void operator()(xmlTextReaderPtr reader) const {
			xmlFreeTextReader(reader);
		}
	};

	static FIELDS field_from_tag(const std::string& tag) {
		FIELDS field;
		if (tag.compare("my_id") == 0) 
			field = MYID;
		else if (tag.compare("series_animedb_id") == 0 
		         || tag.compare("id") == 0) 
			field = ANIMEDBID;
		else if (tag.compare("series_title") == 0
		         || tag.compare("title") == 0) 
			field = SERIESTITLE;
		else if (tag.compare("series_type") == 0
		         || tag.compare("type") == 0) 
			field = SERIESTYPE;
		else if (tag.compare("series_episodes") == 0
		         || tag.compare("episodes") == 0) 
			field = SERIESEPISODES;
		else if (tag.compare("series_status") == 0
		         || tag.compare("status") == 0) 
			field = SERIESSTATUS;
		else if (tag.compare("series_start") == 0
		         || tag.compare("start_date") == 0) 
			field = SERIESDATEBEGIN;
		else if (tag.compare("series_end") == 0
		         || tag.compare("end_date") == 0) 
			field = SERIESDATEEND;
		else if (tag.compare("series_image") == 0
		         || tag.compare("image") == 0) 
			field = SERIESIMAGEURL;
		else if (tag.compare("series_synonyms") == 0
		         || tag.compare("synonyms") == 0) 
			field = SERIESSYNONYMS;
		else if (tag.compare("english") == 0) 
			field = SERIESSYNONYMS;
		else if (tag.compare("my_watched_episodes") == 0)
			field = MYWATCHEDEPISODES;
		else if (tag.compare("my_start_date") == 0) 
			field = MYSTARTDATE;
		else if (tag.compare("my_finish_date") == 0) 
			field = MYFINISHDATE;
		else if (tag.compare("my_score") == 0
		         || tag.compare("score") == 0) 
			field = MYSCORE;
		else if (tag.compare("my_status") == 0) 
			field = MYSTATUS;
		else if (tag.compare("my_rewatching") == 0) 
			field = MYREWATCHING;
		else if (tag.compare("my_rewatching_ep") == 0) 
			field = MYREWATCHINGEP;
		else if (tag.compare("my_last_updated") == 0) 
			field = MYLASTUPDATED;
		else if (tag.compare("my_tags") == 0) 
			field = MYTAGS;
		else if (tag.compare("user_name") == 0)
			field = FIELDNONE;
		else if (tag.compare("user_id") == 0)
			field = USERID;
		else if (tag.compare("user_watching") == 0)
			field = FIELDNONE;
		else if (tag.compare("user_completed") == 0)
			field = FIELDNONE;
		else if (tag.compare("user_onhold") == 0)
			field = FIELDNONE;
		else if (tag.compare("user_dropped") == 0)
			field = FIELDNONE;
		else if (tag.compare("user_plantowatch") == 0)
			field = FIELDNONE;
		else if (tag.compare("user_days_spent_watching") == 0)
			field = FIELDNONE;
		else if (tag.compare("myinfo") == 0)
			field = FIELDNONE;
		else if (tag.compare("myanimelist") == 0)
			field = FIELDNONE;
		else if (tag.compare("synopsis") == 0)
			field = FIELDNONE;
		else if (tag.compare("entry") == 0)
			field = FIELDNONE;
		else {
			field = FIELDNONE;
			std::cerr << "Unknown tag: " << tag << std::endl;
		}

		return field;
	}

	static void set_anime_for_field(Anime& anime, const FIELDS field, const std::string& text) {
		switch (field) {
		case MYID:
			anime.id = std::stoll(text);
			break;
		case ANIMEDBID:
			anime.series_animedb_id = std::stoll(text);
			break;
		case SERIESTITLE:
			anime.series_title = text;
			break;
		case SERIESTYPE:
			if (text.size() == 1) 
				anime.series_type = anime_series_type_from_int(std::stoi(text));
			else
				anime.series_type = anime_series_type_from_string(text);
			break;
		case SERIESEPISODES:
			anime.series_episodes = std::stoi(text);
			break;
		case SERIESSTATUS:
			if (text.size() == 1)
				anime.series_status = anime_series_status_from_int(std::stoi(text));
			else
				anime.series_status = anime_series_status_from_string(text);
			break;
		case SERIESDATEBEGIN:
			anime.series_date_begin = text;
			break;
		case SERIESDATEEND:
			anime.series_date_end = text;
			break;
		case SERIESIMAGEURL:
			anime.image_url = text;
			break;
		case SERIESSYNONYMS:
			{
				auto iter = text.find("; ");
				decltype(iter) start = 0;
				while (iter != text.npos) {
					auto const substr = text.substr(start, iter - start);
					if (substr.size() > 0) {
						anime.series_synonyms.insert(substr);
					}

					start = iter + 2;
					iter = text.find("; ", start);
				}
				anime.series_synonyms.insert(text.substr(start, iter - start));
			}
			break;
		case MYWATCHEDEPISODES:
			anime.episodes = std::stoi(text);
			break;
		case MYSTARTDATE:
			anime.date_start = text;
			break;
		case MYFINISHDATE:
			anime.date_finish = text;
			break;
		case MYSCORE:
			anime.score = std::stof(text);
			break;
		case MYSTATUS:
			anime.status = anime_status_from_int(std::stoi(text));
			break;
		case MYREWATCHING:
			anime.enable_rewatching = text.compare("0")==0?false:true;
			break;
		case MYREWATCHINGEP:
			anime.rewatch_episode = std::stoi(text);
			break;
		case MYLASTUPDATED:
			anime.last_updated = std::stoull(text);
			break;
		case MYTAGS:
			{
				auto iter = text.find("; ");
				decltype(iter) start = 0;
				while (iter != text.npos) {
					auto const substr = text.substr(start, iter - start);
					if (substr.size() > 0) {
						anime.tags.insert(substr);
					}

					start = iter + 2;
					iter = text.find("; ", start);
				}
				anime.tags.insert(text.substr(start, iter - start));
			}
			break;
		case FIELDNONE:
			//std::cerr << " = " << text << std::endl;
			break;
		case USERID:
			break;
		default:
			std::cerr << "Invalid field. Text = " << text << std::endl;
		}
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
				if (node_type == 1) {         // Element start
					if (name_str.compare("entry") == 0) {
						if (seen_anime && !seen_entry) 
							entry_after_anime = true;
						seen_entry = true;
					} else if (name_str.compare("anime") == 0) {
						seen_anime = true;
					} else {
						field = field_from_tag(name_str);
					}
				} else if (node_type == 15) { // End Element
					if (entry_after_anime) {
						if (name_str.compare("entry") == 0) {
							res.push_back(anime);
							anime = Anime();
						}
					} else {
						if (name_str.compare("anime") == 0) {
							res.push_back(anime);
							anime = Anime();
						}
					}
					field = FIELDNONE;
				} else if (value) {           // Text
					const std::string value_str(reinterpret_cast<const char*>(value));
					if (name_str.compare("#text") == 0)
						set_anime_for_field(anime, field, value_str);
					else
						std::cerr << "Error: Unexpected " << name_str << " = " 
						          << value_str << std::endl;
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
