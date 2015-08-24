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

#include "xml_writer.hpp"

namespace MAL {

    XmlWriter::XmlWriter() :
        buffer(xmlBufferCreate()),
        writer(xmlNewTextWriterMemory(buffer.get(), 0))
    {
    }

    XmlWriter::XmlWriter(XmlWriter&& w) :
        buffer(std::move(w.buffer)),
        writer(std::move(w.writer))
    {
    }

    void XmlWriter::startDoc(const std::string& encoding)
    {
        xmlTextWriterStartDocument(writer.get(), nullptr, encoding.c_str(), nullptr);
    }

    void XmlWriter::endDoc()
    {
        xmlTextWriterEndDocument(writer.get());
    }

    void XmlWriter::startElement(const std::string& elem)
    {
        xmlTextWriterStartElement(writer.get(), reinterpret_cast<const xmlChar*>(elem.c_str()));
    }

    void XmlWriter::endElement()
    {
        xmlTextWriterEndElement(writer.get());
    }

    void XmlWriter::writeAttribute(const std::string& key, const std::string& value)
    {
        xmlTextWriterWriteAttribute(writer.get(),
                                    reinterpret_cast<const xmlChar*>(key.c_str()),
                                    reinterpret_cast<const xmlChar*>(value.c_str()));
    }

    void XmlWriter::writeFormatElement(const std::string& name, const std::string& format, ...)
    {
        va_list ap;
        va_start (ap, format);
        xmlTextWriterWriteVFormatElement(writer.get(),
                                        reinterpret_cast<const xmlChar*>(name.c_str()),
                                        format.c_str(),
                                        ap);
        va_end (ap);
    }

    void XmlWriter::writeElement(const std::string& name, const std::string& value)
    {
        xmlTextWriterWriteElement(writer.get(),
                                  reinterpret_cast<const xmlChar*>(name.c_str()),
                                  reinterpret_cast<const xmlChar*>(value.c_str()));
    }

    std::string XmlWriter::getString() const
    {
        return std::string(reinterpret_cast<const char*>(xmlBufferContent(buffer.get())));
    }
}
