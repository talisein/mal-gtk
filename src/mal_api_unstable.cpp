/* -*- mode: c++; c-file-style: "linux"; c-basic-offset: 4; indent-tabs-mode: nil; -*-
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

#include <type_traits>
#include <algorithm>
#include <cctype>
#include "mal_api_unstable.hpp"
#include <libxml++/libxml++.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlreader.h>
#include <libxml/HTMLparser.h>
#include <iostream>
#include "secret_allocator.h"

namespace
{
    constexpr char mal_login_url[] = "https://myanimelist.net/login.php";
    constexpr char mal_details_base_url[] = "https://myanimelist.net/ownlist";

    struct XmlDocDeleter {
        void operator()(xmlDocPtr doc) const {
            if (doc) xmlFreeDoc(doc);
        }
    };
    using docp = std::unique_ptr<xmlDoc, XmlDocDeleter>;

	struct xmlTextReaderDeleter {
		void operator()(xmlTextReaderPtr reader) const {
			xmlFreeTextReader(reader);
		}
	};
    using readerp = std::unique_ptr<xmlTextReader, xmlTextReaderDeleter>;

    struct XmlStrDeleter {
		void operator()(xmlChar *str) const {
			xmlFree(str);
		}
	};
    using xmlstrp = std::unique_ptr<xmlChar, XmlStrDeleter>;

    constexpr const xmlChar* operator"" _xml(const char* str, size_t) {
        return reinterpret_cast<const xmlChar*>(str);
    }

    extern "C"
    {
        size_t
        curl_write_function(void *buf, size_t size, size_t nmemb, void *userp)
        {
            g_return_val_if_fail(nullptr != userp, 0);
            auto ss = static_cast<std::stringstream*>(userp);

            std::streamsize count ( size * nmemb );
            ss->write(static_cast<char*>(buf), count);
            return count;
        }
    }

    std::string xml_to_string(xmlChar const * value) {
        if (!value)
            return std::string();

        return reinterpret_cast<char const *>(value);
    }

    std::string read_value(readerp& reader) {
        return xml_to_string(xmlTextReaderConstValue(reader.get()));
    }

    int
    fetch_csrf(MAL::curlp& curl, std::string& csrf)
    {
        std::stringstream buf;
        if (G_UNLIKELY(0 > curl_setopt(curl, CURLOPT_URL, mal_login_url) ||
                       0 > curl_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function) ||
                       0 > curl_setopt(curl, CURLOPT_WRITEDATA, &buf) ||
                       0 > curl_setopt(curl, CURLOPT_FAILONERROR, 1L) ||
                       0 > curl_setopt(curl, CURLOPT_NOSIGNAL, 1L)))
        {
            return -1;
        }

        if (CURLcode code = curl_perform(curl); G_UNLIKELY(CURLE_OK != code)) {
            return -1;
        }

        auto page = buf.str();
        docp doc { htmlReadMemory(page.data(), page.size(), mal_login_url, nullptr,
                                  HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET) };
        if (!doc)
            return -1;

        readerp reader {xmlReaderWalker(doc.get())};
        if (!reader)
            return -1;

        while (xmlTextReaderRead(reader.get())) {
            if (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader.get()) ||
                !xmlStrEqual(xmlTextReaderConstName(reader.get()), "meta"_xml))
            {
                continue;
            }

            const int num = xmlTextReaderAttributeCount(reader.get());
            if (2 != num) {
                continue;
            }
            xmlTextReaderMoveToFirstAttribute(reader.get());
            if (!xmlStrEqual(xmlTextReaderConstValue(reader.get()), "csrf_token"_xml))
                continue;
            xmlTextReaderMoveToNextAttribute(reader.get());
            csrf = read_value(reader);
            break;
        }

        return 0;
    }

    int
    parse_for_bad_username_or_password(const std::string& page, Glib::ustring& error_msg)
    {
        docp doc { htmlReadMemory(page.data(), page.size(), mal_login_url, nullptr,
                                  HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET) };
        if (!doc)
            return -1;

        readerp reader {xmlReaderWalker(doc.get())};
        if (!reader)
            return -1;

        /* Looking for something like:
           <div class="badresult">    Incorrect username or password   </div> */
        while (xmlTextReaderRead(reader.get())) {
            if (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader.get()) ||
                !xmlStrEqual(xmlTextReaderConstName(reader.get()), "div"_xml))
            {
                continue;
            }

            const int num = xmlTextReaderAttributeCount(reader.get());
            if (1 != num) {
                continue;
            }
            xmlTextReaderMoveToFirstAttribute(reader.get());
            if (!xmlStrEqual(xmlTextReaderConstValue(reader.get()), "badresult"_xml))
                continue;
            xmlTextReaderRead(reader.get());
            if (XML_READER_TYPE_TEXT != xmlTextReaderNodeType(reader.get()))
                continue;

            xmlstrp msg {xmlTextReaderReadString(reader.get())};
            if (!msg)
                return -1;
            Glib::ustring str {reinterpret_cast<char*>(msg.get())};

            /* Trim the whitespace */
            auto first = std::find_if_not(std::begin(str), std::end(str), [](unsigned char c){ return std::isspace(c); });
            if (std::end(str) == first)
                return -1;
            auto last = std::find_if_not(std::rbegin(str), std::rend(str), [](unsigned char c){ return std::isspace(c); });
            if (std::rend(str) == last)
                return -1;
            error_msg.append(first, std::prev(last).base());
            return 0;
        }
        return -1;
    }

    int
    parse_selector_to_int(std::optional<int>& out, int& seen_options, readerp& reader)
    {
        while (1 == xmlTextReaderRead(reader.get())) {
            if (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType(reader.get()) &&
                xmlStrEqual(xmlTextReaderConstName(reader.get()), "select"_xml))
                break;
            if (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader.get()) ||
                !xmlStrEqual(xmlTextReaderConstName(reader.get()), "option"_xml) ||
                2 != xmlTextReaderAttributeCount(reader.get()))
                continue;
            ++seen_options;
            xmlTextReaderMoveToFirstAttribute(reader.get());
            if (!xmlStrEqual(xmlTextReaderConstName(reader.get()), "value"_xml)) {
                g_warning("Unexpected selector option attribute, expected value "
                          "but got %s", (const char*)xmlTextReaderConstName(reader.get()));
                return -1;
            }
            auto val = read_value(reader);
            xmlTextReaderMoveToNextAttribute(reader.get());
            if (!xmlStrEqual(xmlTextReaderConstName(reader.get()), "selected"_xml) ||
                !xmlStrEqual(xmlTextReaderConstValue(reader.get()), "selected"_xml)) {
                g_warning("Unexpected selector option attribute, expected selected "
                          "but got %s=%s",
                          (const char*)xmlTextReaderConstName(reader.get()),
                          (const char*)xmlTextReaderConstValue(reader.get()));
                return -1;
            }
            try {
                out = std::stoi(val);
                return 0;
            } catch (const std::exception& e) {
                g_warning("Couldn't parse selector value from '%s': %s", val.c_str(), e.what());
            }
            return -1;
        }
        return -1;
    }
}

namespace MAL
{
    bool
    mal_unstable_is_logged_in(curlp& curl)
    {
        if (curlslistp cookies { curl_getcookies(curl) }; G_LIKELY(cookies)) {
            for (struct curl_slist *iter = cookies.get(); iter; iter = iter->next) {
                if (nullptr != strstr(iter->data, "is_logged_in\t1")) {
                    return true;
                }
            }
        }

        return false;
    }

    int
    mal_unstable_html_login(const std::unique_ptr<UserInfo>& info,
                            curlp& curl,
                            Glib::ustring &error_msg)
    {
        std::string csrf;
        if (G_UNLIKELY(0 > fetch_csrf(curl, csrf))) {
            g_warning("Failed to fetch csrf token. This likely means network issues.");
            error_msg = "Failed to login. No csrf token. Check network.";
            return -1;
        }

        secret_string fields;
        fields.reserve(200);
        fields += "user_name=";
        fields += curl_escape(curl, info->get_username_str());
        fields += "&password=";
        fields += curl_escape(curl, info->get_password_str());
        fields += "&cookie=1&sublogin=Login&submit=1&csrf_token=";
        fields += csrf;

        std::stringstream page;
        if (G_UNLIKELY(0 > curl_setopt(curl, CURLOPT_POST, 1L) ||
                       0 > curl_setopt(curl, CURLOPT_POSTFIELDS, fields.c_str()) ||
                       0 > curl_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(fields.size())) ||
                       0 > curl_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function) ||
                       0 > curl_setopt(curl, CURLOPT_WRITEDATA, &page)))
        {
            error_msg = "Failed to login. Check log and file a bug.";
            return -1;
        }

        if (CURLcode code = curl_perform(curl); G_UNLIKELY(CURLE_OK != code)) {
            error_msg = "Failed to login: ";
            error_msg.append(curl_easy_strerror(code));
            return -1;
        }

        curl_setopt(curl, CURLOPT_WRITEDATA, nullptr);
        curl_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        curl_setopt(curl, CURLOPT_POSTFIELDS, nullptr);
        curl_setopt(curl, CURLOPT_HTTPGET, 1L);

        /* 302 means success, 200 means failure. Jan 2018. */
        long response_code = 0;
        if (G_UNLIKELY(0 > curl_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code))) {
            error_msg = "Failed to login. Check log and file a bug.";
            return -1;
        }

        /* Check for is_logged_in cookie */
        bool cookie_logged_in = mal_unstable_is_logged_in(curl);

        /* Ok, most reliable check: We got expected 302 and we see the cookie */
        if (G_LIKELY(302 == response_code && cookie_logged_in))
            return 0;

        /* If we have the cookie, let's message about the unexpected response code */
        if (cookie_logged_in) {
            g_message("After trying to log in, we have an is_logged_in cookie, "
                      "but the response code was %ld instead of the expected 302.",
                      response_code);
            return 0;
        }

        /* Ok, it doesn't seem we have a cookie. Lets check the
         * response for an error message. */
        if (0 == parse_for_bad_username_or_password(page.str(), error_msg)) {
            /* If there is an error message and 200, we have
             * definitely failed to login. */
            if (200 == response_code) {
                g_message("Failed to login: %s", error_msg.c_str());
                return -1;
            }

            g_warning("After trying to log in, we got response code %ld, "
                      "but we don't have the expected cookie. We see this "
                      "error message: %s",
                      response_code, error_msg.c_str());
            return -1;
        }

        /* We couldn't parse an error message out...*/
        if (302 == response_code) {
            g_warning("After trying to log in, we don't have a cookie, but we "
                      "don't see an error message. We'll try to proceed. This "
                      "likely means mal-gtk needs to be updated to understand "
                      "the latest myanimelist.net. Please file a bug report.");
            return 0;
        }

        if (200 == response_code) {
            error_msg = "Failed to login. Bad password? Check log.";
            g_warning("After trying to log in, we don't have a cookie. We "
                      "couldn't parse an error message but we've likely "
                      "failed; perhaps due to a wrong password. mal-gtk "
                      "needs to be updated to understand this behavior. "
                      "Please file a bug report.");
            return -1;
        }

        error_msg = "Failed to login. Check log and file a bug report.";
        g_warning("Failed to log in, and we don't understand why. Received "
                  "response code %ld. Please file a bug report.", response_code);
        return -1;
    }

    int
    mal_unstable_anime_details(int64_t series_itemdb_id,
                               const std::unique_ptr<UserInfo>& info,
                               curlp& curl,
                               anime_details_unstable& details,
                               Glib::ustring& error_msg)
    {
        std::stringstream ss;
        ss << mal_details_base_url << "/anime/" << series_itemdb_id << "/edit";
        auto url = ss.str();

        bool did_login = false;
        if (!mal_unstable_is_logged_in(curl)) {
            if (0 > mal_unstable_html_login(info, curl, error_msg))
                return -1;
            did_login = true;
        }

        std::stringstream buf;
        do {
            std::stringstream newbuf;
            buf.swap(newbuf);

            if (G_UNLIKELY(0 > curl_setopt(curl, CURLOPT_URL, url.c_str()) ||
                           0 > curl_setopt(curl, CURLOPT_HTTPGET, 1L) ||
                           0 > curl_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function) ||
                           0 > curl_setopt(curl, CURLOPT_WRITEDATA, &buf) ||
                           0 > curl_setopt(curl, CURLOPT_FAILONERROR, 1L) ||
                           0 > curl_setopt(curl, CURLOPT_NOSIGNAL, 1L)))
            {
                return -1;
            }

            CURLcode code = curl_perform(curl);
            if (CURLE_OK != code) {
                if (did_login) {
                    error_msg = "Failed to fetch details: ";
                    error_msg.append(curl_easy_strerror(code));
                    return -1;
                }

                if (0 > mal_unstable_html_login(info, curl, error_msg))
                    return -1;
                did_login = true;
                continue;
            }

            long response_code = 0;
            if (G_UNLIKELY(0 > curl_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code))) {
                error_msg = "Failed to login. Check log and file a bug.";
                return -1;
            }

            /* TODO: Check we have a good page, or if we need to log in */
            if (200 == response_code)
                break;

            if (did_login) {
                g_warning("We logged in but couldn't fetch %s. Got response code %ld",
                          url.c_str(), response_code);
                error_msg = "Failed to fetch details. Check log and file a bug.";
                return -1;
            }

            if (0 > mal_unstable_html_login(info, curl, error_msg))
                return -1;
            did_login = true;
        } while (true);


        auto page = buf.str();
        docp doc { htmlReadMemory(page.data(), page.size(), mal_login_url, nullptr,
                                  HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET) };
        if (!doc)
            return -1;

        readerp reader {xmlReaderWalker(doc.get())};
        if (!reader)
            return -1;

        while (1 == xmlTextReaderRead(reader.get())) {
            if (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader.get()))
                continue;

            if (xmlStrEqual(xmlTextReaderConstName(reader.get()), "input"_xml)) {
                /* storage value or num watched. */
                xmlTextReaderMoveToFirstAttribute(reader.get());
                if (!xmlStrEqual(xmlTextReaderConstName(reader.get()), "type"_xml) ||
                    !xmlStrEqual(xmlTextReaderConstValue(reader.get()), "text"_xml))
                    continue;
                xmlTextReaderMoveToNextAttribute(reader.get());
                if (!xmlStrEqual(xmlTextReaderConstName(reader.get()), "id"_xml))
                    continue;
                if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_storage_value"_xml)) {
                    if (1 != xmlTextReaderMoveToNextAttribute(reader.get()) || /* name="add_anime[storage_value]" */
                        1 != xmlTextReaderMoveToNextAttribute(reader.get()) || /* class="inputtext" */
                        1 != xmlTextReaderMoveToNextAttribute(reader.get()) || /* size="4" */
                        1 != xmlTextReaderMoveToNextAttribute(reader.get())) /* value="2.2" */
                    {
                        g_warning("While parsing anime details, got unexpected "
                                  "input for storage_value. Please file a bug.");
                        continue;
                    }
                    auto val = read_value(reader);
                    if (val.empty()) {
                        g_debug("Got empty value for anime_storage_value");
                    } else {
                        try {
                            details.storage_value = std::stod(val);
                            g_debug("Parsed storage_value: %f", details.storage_value.value());
                        } catch (const std::exception& e) {
                            g_warning("Couldn't parse storage value from '%s': %s", val.c_str(), e.what());
                        }
                    }
                } else if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_num_watched_times"_xml)) {
                    if (1 != xmlTextReaderMoveToNextAttribute(reader.get()) || /* name="add_anime[num_watched_times]" */
                        1 != xmlTextReaderMoveToNextAttribute(reader.get()) || /* class="inputtext" */
                        1 != xmlTextReaderMoveToNextAttribute(reader.get()) || /* size="4" */
                        1 != xmlTextReaderMoveToNextAttribute(reader.get())) /* value="5" */
                    {
                        g_warning("While parsing anime details, got unexpected "
                                  "input for num_watched_times. Please file a bug.");
                        continue;
                    }
                    auto val = read_value(reader);
                    if (val.empty()) {
                        g_debug("Got empty value for num_watched_times.");
                        details.times_watched = 0;
                    } else {
                        try {
                            details.times_watched = std::stoi(val);
                            g_debug("Parsed times_watched: %d", details.times_watched.value());
                        } catch (const std::exception& e) {
                            g_warning("Couldn't parse storage value from '%s': %s", val.c_str(), e.what());
                        }
                    }
                }
            } else if (xmlStrEqual(xmlTextReaderConstName(reader.get()), "textarea"_xml)) {
                /* comments */
                xmlTextReaderMoveToFirstAttribute(reader.get());
                if (!xmlStrEqual(xmlTextReaderConstName(reader.get()), "id"_xml) ||
                    !xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_comments"_xml))
                    continue;
                while (1 == xmlTextReaderRead(reader.get())) {
                    if (XML_READER_TYPE_TEXT == xmlTextReaderNodeType(reader.get()) ||
                        XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType(reader.get()))
                        break;
                }
                if (XML_READER_TYPE_TEXT != xmlTextReaderNodeType(reader.get()))  {
                    g_debug("Got to end element before seeing comments");
                    details.comments = "";
                    continue;
                }

                xmlstrp msg {xmlTextReaderReadString(reader.get())};
                if (!msg) {
                    g_debug("Got an empty comment");
                    details.comments = "";
                    continue;
                }
                details.comments = {reinterpret_cast<char*>(msg.get())};
            } else if (xmlStrEqual(xmlTextReaderConstName(reader.get()), "select"_xml)) {
                /* priority, storage type, rewatch value, discuss, sns */
                xmlTextReaderMoveToFirstAttribute(reader.get());
                if (!xmlStrEqual(xmlTextReaderConstName(reader.get()), "id"_xml))
                    continue;
                int seen_options = 0;
                if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_priority"_xml)) {
                    if (0 > parse_selector_to_int(details.priority, seen_options, reader)) {
                        g_debug("failed to parse priority");
                    } else {
                        g_debug("Parsed priority %d", details.priority.value());
                    }
                } else if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_storage_type"_xml)) {
                    if (0 > parse_selector_to_int(details.storage_type, seen_options, reader)) {
                        g_debug("failed to parse storage_type");
                    } else {
                        g_debug("Parsed storage_type %d", details.storage_type.value());
                    }
                } else if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_rewatch_value"_xml)) {
                    if (0 > parse_selector_to_int(details.rewatch_value, seen_options, reader)) {
                        g_debug("failed to parse rewatch_value");
                    } else {
                        g_debug("Parsed rewatch_value %d", details.rewatch_value.value());
                    }
                } else if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_is_asked_to_discuss"_xml)) {
                    if (0 > parse_selector_to_int(details.discuss, seen_options, reader)) {
                        g_debug("failed to parse discuss");
                    } else {
                        g_debug("Parsed discuss %d", details.discuss.value());
                    }
                } else if (xmlStrEqual(xmlTextReaderConstValue(reader.get()), "add_anime_sns_post_type"_xml)) {
                    if (0 > parse_selector_to_int(details.sns, seen_options, reader)) {
                        g_debug("failed to parse sns");
                    } else {
                        g_debug("Parsed sns %d", details.sns.value());
                    }
                }
            }
        }

        if (details.storage_value && details.times_watched && details.comments
            /*found_priority && found_storage_type && found_rewatch_value &&
            found_discuss && found_sns*/)
            return 0;
        return -1;
    }
}
