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
#include <memory>
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <string>
#include <stdarg.h>

namespace MAL {

    class XmlReader {
    public:
        XmlReader(const std::string& xml);
        XmlReader(std::string&& xml);
        XmlReader(XmlReader&& reader);
        XmlReader(const XmlReader&) = delete;
        void operator=(const XmlReader&) = delete;

        int read();
        std::string get_name() const;
        std::string get_value() const;
        int get_type() const;

    private:
        struct XmlTextReaderDeleter {
            void operator()(xmlTextReaderPtr ptr) const {
                xmlFreeTextReader(ptr);
            }
        };
        
        const std::string buf;
        std::unique_ptr<xmlTextReader, XmlTextReaderDeleter> reader;
    };
}
