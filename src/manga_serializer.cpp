#include <iostream>
#include <cstring>
#include <memory>
#include <libxml/xmlreader.h>
#include "manga_serializer.hpp"

namespace MAL {
	enum MANGA_FIELDS : int_fast8_t { FIELDNONE, FIELDTEXT,
			MANGADBID, SERIESTITLE, SERIESTYPE, SERIESCHAPTERS, SERIESVOLUMES, SERIESSTATUS,
			SERIESDATEBEGIN, SERIESDATEEND, SERIESIMAGEURL, SERIESSYNONYMS,
			FIELDMANGA, ENTRY, MYID, MYREADCHAPTERS, MYREADVOLUMES, MYSTARTDATE, MYFINISHDATE, MYSCORE,
			MYSTATUS, MYREREADING, MYREREADINGCHAP, MYLASTUPDATED, MYTAGS,
			USERID, SYNOPSIS
			};
}

namespace {
	static std::map<const std::string, const MAL::MANGA_FIELDS> initialize_field_map() {
		return {
			{ "my_id",                    MAL::MYID },
			{ "series_mangadb_id",        MAL::MANGADBID },
			{ "id",                       MAL::MANGADBID },
			{ "series_title",             MAL::SERIESTITLE },
			{ "title",                    MAL::SERIESTITLE },
			{ "series_type",              MAL::SERIESTYPE },
			{ "type",                     MAL::SERIESTYPE },
			{ "series_chapters",          MAL::SERIESCHAPTERS },
			{ "chapters",                 MAL::SERIESCHAPTERS },
			{ "series_volumes",           MAL::SERIESVOLUMES },
			{ "volumes",                  MAL::SERIESVOLUMES },
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
			{ "my_read_chapters",         MAL::MYREADCHAPTERS },
			{ "my_read_volumes",          MAL::MYREADVOLUMES },
			{ "my_start_date",            MAL::MYSTARTDATE },
			{ "my_finish_date",           MAL::MYFINISHDATE },
			{ "my_status",                MAL::MYSTATUS },
			{ "my_rereading",             MAL::MYREREADING },
			{ "my_rereadingg", /*srsly?*/ MAL::MYREREADING },
			{ "my_rereading_chap",        MAL::MYREREADINGCHAP },
			{ "my_last_updated",          MAL::MYLASTUPDATED },
			{ "my_tags",                  MAL::MYTAGS },
			{ "synopsis",                 MAL::SYNOPSIS },
			{ "user_id",                  MAL::USERID },
			{ "entry",                    MAL::ENTRY },
			{ "manga",                    MAL::FIELDMANGA },
			{ "#text",                    MAL::FIELDTEXT },
			{ "user_name",                MAL::FIELDNONE },
			{ "user_reading",             MAL::FIELDNONE },
			{ "user_completed",           MAL::FIELDNONE },
			{ "user_onhold",              MAL::FIELDNONE },
			{ "user_dropped",             MAL::FIELDNONE },
			{ "user_plantoread",          MAL::FIELDNONE },
			{ "user_days_spent_watching", MAL::FIELDNONE },
			{ "myinfo",                   MAL::FIELDNONE },
			{ "myanimelist",              MAL::FIELDNONE },
                };
	}

	static std::map<const MAL::MANGA_FIELDS, std::function<void (MAL::Manga&, const std::string&)> > initialize_member_map() {
		return {
			{ MAL::MANGADBID,         std::mem_fn(&MAL::Manga::set_series_itemdb_id) },
			{ MAL::SERIESTITLE,       std::mem_fn(&MAL::Manga::set_series_title) },
			{ MAL::SERIESDATEBEGIN,   std::mem_fn(&MAL::Manga::set_series_date_begin) },
			{ MAL::SERIESDATEEND,     std::mem_fn(&MAL::Manga::set_series_date_end) },
			{ MAL::SERIESIMAGEURL,    std::mem_fn(&MAL::Manga::set_image_url) },
			{ MAL::SERIESSYNONYMS,    std::mem_fn(&MAL::Manga::set_series_synonyms) },
			{ MAL::SYNOPSIS,          std::mem_fn(&MAL::Manga::set_series_synopsis) },

			{ MAL::SERIESTYPE,        std::mem_fn(&MAL::Manga::set_series_type) },
			{ MAL::SERIESCHAPTERS,    std::mem_fn(&MAL::Manga::set_series_chapters) },
			{ MAL::SERIESVOLUMES,     std::mem_fn(&MAL::Manga::set_series_volumes) },
			{ MAL::SERIESSTATUS,      std::mem_fn(&MAL::Manga::set_series_status) },

			{ MAL::MYTAGS,            std::mem_fn(&MAL::Manga::set_tags) },
			{ MAL::MYSTARTDATE,       std::mem_fn(&MAL::Manga::set_date_start) },
			{ MAL::MYFINISHDATE,      std::mem_fn(&MAL::Manga::set_date_finish) },
			{ MAL::MYID,              std::mem_fn(&MAL::Manga::set_id) },
			{ MAL::MYLASTUPDATED,     std::mem_fn(&MAL::Manga::set_last_updated) },
			{ MAL::MYSCORE,           std::mem_fn(&MAL::Manga::set_score) },
			{ MAL::MYREREADING,       std::mem_fn(&MAL::Manga::set_enable_reconsuming) },

			{ MAL::MYREADCHAPTERS,    std::mem_fn(&MAL::Manga::set_chapters) },
			{ MAL::MYREADVOLUMES,     std::mem_fn(&MAL::Manga::set_volumes) },
			{ MAL::MYSTATUS,          std::mem_fn(&MAL::Manga::set_status) },
			{ MAL::MYREREADINGCHAP,   std::mem_fn(&MAL::Manga::set_rereading_chapter) },
                };
	}

	struct xmlTextReaderDeleter {
		void operator()(xmlTextReaderPtr reader) const {
			xmlFreeTextReader(reader);
		}
	};

	static std::string xmlchar_to_str(const xmlChar* str) {
		if (str)
			return std::string(reinterpret_cast<const char*>(str));
		else
			return std::string();
	}
}

namespace MAL {

	MangaSerializer::MangaSerializer() :
		field_map(initialize_field_map()),
		member_map(initialize_member_map())
	{
	}


	/** Handles deserialization from both myanimelist and search results.
	 * Search results are      <manga><entry></entry><entry></entry></manga>
	 * myanimelist results are <entry><manga></manga><manga></manga></entry>
	 */
	std::list<Manga> MangaSerializer::deserialize(const std::string& xml) const {
		std::list<Manga> res;
		std::unique_ptr<char[]> cstr(new char[xml.size()]);
		std::memcpy(cstr.get(), xml.c_str(), xml.size());
		std::unique_ptr<xmlTextReader, xmlTextReaderDeleter> reader(xmlReaderForMemory(
            cstr.get(), xml.size(), "", "UTF-8", XML_PARSE_RECOVER | XML_PARSE_NOENT ));

		if (!reader) {
			std::cerr << "Error: Couldn't create XML reader" << std::endl;
            std::cerr << "XML follows: " << xml << std::endl;
			return res;
		}

		Manga manga;
		int ret                 = 1;
		MANGA_FIELDS field      = FIELDNONE;
		MANGA_FIELDS prev_field = FIELDNONE;
		bool entry_after_manga  = false;
		bool seen_manga         = false;
		bool seen_entry         = false;
		for( ret = xmlTextReaderRead(reader.get()); ret == 1;
		     ret = xmlTextReaderRead(reader.get()) ) {
			const std::string name  = xmlchar_to_str(xmlTextReaderConstName (reader.get()));
			const std::string value = xmlchar_to_str(xmlTextReaderConstValue(reader.get()));

			if (name.size() > 0) {
				auto field_iter = field_map.find(name);
				if (field_iter != field_map.end()) {
					prev_field = field;
					field = field_iter->second;
				} else {
					std::cerr << "Unexpected field " << name << std::endl;
				}

				switch (xmlTextReaderNodeType(reader.get())) {
                    case XML_READER_TYPE_ELEMENT:
                        entry_after_manga |= (field == ENTRY) && seen_manga && !seen_entry;
                        seen_entry        |= field == ENTRY;
                        seen_manga        |= field == FIELDMANGA;
                        break;
                    case XML_READER_TYPE_END_ELEMENT:
                        if ( ( entry_after_manga && field == ENTRY) ||
                             (!entry_after_manga && field == FIELDMANGA)) {
                            res.push_back(manga);
                            manga = Manga();
                        }
                        field = FIELDNONE;
                        break;
                    case XML_READER_TYPE_TEXT:
                        if( field != FIELDTEXT ) {
                            std::cerr << "There's a problem!" << std::endl;
                        }
                        if (value.size() > 0) {
                            auto member_iter = member_map.find(prev_field);
                            if ( member_iter != member_map.end() ) {
                                member_iter->second(manga, value);
                            }
                        } else {
                            std::cerr << "Error: Unexpected " << name << " = " 
                                      << value << std::endl;
                        }
                        break;
                    case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
                        break;
                    default:
                        std::cerr << "Warning: Unexpected node type "
                                  << xmlTextReaderNodeType(reader.get())
                                  << " : " << name << "=" << value << std::endl;
                        break;
				}
			}
		}
		
		if ( ret != 0 ) {
			std::cerr << "Error: Failed to parse! ret = " << ret << std::endl;
		}

		return res;
	}

	std::string MangaSerializer::serialize(const Manga& manga) const {
		std::string out;
		
		out += "<?xml version=\"1.0\" encoding=\"UTF-8\"?><entry>";
		out += "<chapter>";
		out += std::to_string(manga.chapters);
		out += "</chapter>";
		out += "<volume>";
		out += std::to_string(manga.volumes);
		out += "</volume>";
		out += "<status>";
		out += std::to_string(manga.status);
		out += "</status>";
		out += "<score>";
		out += std::to_string(manga.score);
		out += "</score>";
		out += "<downloaded_chapters>";
		out += std::to_string(manga.downloaded_items);
		out += "</downloaded_chapters>";
		out += "<times_reread>";
		out += "</times_reread>";
		out += "<reread_value>";
		out += "</reread_value>";
		out += "<date_start>";
		out += manga.date_start;
		out += "</date_start>";
		out += "<date_finish>";
		out += manga.date_finish;
		out += "</date_finish>";
		out += "<priority>";
		out += "</priority>";
		out += "<enable_discussion>";
		out += "</enable_discussion>";
		out += "<enable_rereading>";
		out += (manga.enable_reconsuming)?"1":"0";
		out += "</enable_rereading>";
		out += "<comments>";
		out += "</comments>";
		out += "<scan_group>";
		out += "</scan_group>";
		out += "<tags>";
		auto iter = manga.tags.begin();
		bool was_first = true;
		while (iter != manga.tags.end()) {
			if (!was_first)
				out += "; ";
			out += *iter;
			was_first = false;
			++iter;
		}
		out += "</tags>";
		out += "<retail_volumes>";
		out += "</retail_volumes>";
		out += "</entry>";
		return out;
	}
	
}
