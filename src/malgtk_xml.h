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

#include <assert.h>
#include <glib-object.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

struct malgtk_xml_serialization_defs
{
    GType type;
    gint prop_id;
    const gchar *prop_name;
    const gchar *xml_name;
    const gchar *xml_subname;
    goffset ofs;
    gsize enum_size;
};

#define MALGTK_ENUM_IS_SERIALIZABLE(e) static_assert(sizeof(e) == 1 || sizeof(e) == 2 || sizeof(e) == 4 || sizeof(e) == 8, "Unexpected enum width")

void malgtk_xml_serialize(xmlTextWriterPtr writer, struct malgtk_xml_serialization_defs *defs, gconstpointer priv);



