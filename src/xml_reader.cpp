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

#include "xml_reader.hpp"


namespace MAL {

    XmlReader::XmlReader(const std::string& xml) :
        buf(xml),
        reader(xmlReaderForMemory(buf.c_str(), buf.size(), nullptr, nullptr, 0))
    {
    }

    XmlReader::XmlReader(std::string&& xml) :
        buf(std::move(xml)),
        reader(xmlReaderForMemory(buf.c_str(), buf.size(), nullptr, nullptr, 0))
    {
    }

    XmlReader::XmlReader(XmlReader&& xml_reader) :
        buf(std::move(xml_reader.buf)),
        reader(std::move(xml_reader.reader))
    {
    }

    int
    XmlReader::read()
    {
        return xmlTextReaderRead(reader.get());
    }

    std::string
    XmlReader::get_name() const
    {
        return reinterpret_cast<const char*>(xmlTextReaderConstName(reader.get()));
    }

    std::string
    XmlReader::get_value() const
    {
        return reinterpret_cast<const char*>(xmlTextReaderConstValue(reader.get()));
    }

    int
    XmlReader::get_type() const
    {
        return xmlTextReaderNodeType(reader.get());
    }

}
