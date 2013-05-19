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
#include <libxml/xmlwriter.h>
#include <string>
#include <stdarg.h>

namespace MAL {

    class XmlWriter {
    public:
        XmlWriter();
        XmlWriter(const XmlWriter&) = delete;
        void operator=(const XmlWriter&) = delete;
        XmlWriter(XmlWriter&& writer);

        void startDoc(const std::string& encoding = "UTF-8");
        void endDoc();
        void startElement(const std::string& elem);
        void endElement();
        void writeAttribute(const std::string& key, const std::string& value);
        void writeFormatElement(const std::string& name, const std::string& format, ...);
        void writeElement(const std::string& name, const std::string& value);

        std::string getString() const;
    private:
        struct XmlTextWriterDeleter {
            void operator()(xmlTextWriterPtr ptr) const {
                xmlFreeTextWriter(ptr);
            }
        };

        struct XmlBufferDeleter {
            void operator()(xmlBufferPtr ptr) const {
                xmlBufferFree(ptr);
            }
        };

        std::unique_ptr<xmlBuffer, XmlBufferDeleter> buffer;
        std::unique_ptr<xmlTextWriter, XmlTextWriterDeleter> writer;

    };

}
