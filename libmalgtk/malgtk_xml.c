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

#include "malgtk_xml.h"
#include "malgtk_gtree.h"
#include "malgtk_date.h"

static void
malgtk_xml_serialize_int64(xmlTextWriterPtr writer,
                           const char *element,
                           gint64 i)
{
    xmlTextWriterWriteFormatElement(writer, BAD_CAST element,
                                    "%" G_GINT64_FORMAT, i);
}

static void
malgtk_xml_serialize_int(xmlTextWriterPtr writer,
                         const char *element,
                         gint i)
{
    xmlTextWriterWriteFormatElement(writer, BAD_CAST element,
                                    "%d", i);
}

static void
malgtk_xml_serialize_bool(xmlTextWriterPtr writer,
                          const char *element,
                          gboolean b)
{
    xmlTextWriterWriteFormatElement(writer, BAD_CAST element, "%u", b);
}

static void
malgtk_xml_serialize_double(xmlTextWriterPtr writer,
                            const char *element,
                            gdouble d)
{
#if 0
    gchar buf[G_ASCII_DTOSTR_BUF_SIZE];
    g_ascii_dtostr(buf, G_ASCII_DTOSTR_BUF_SIZE, d);
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST buf);
#else
    xmlTextWriterWriteFormatElement(writer, BAD_CAST element, "%f", d);
#endif    
}

static void
malgtk_xml_serialize_enum(xmlTextWriterPtr writer,
                          GType enum_type,
                          const char *element,
                          gint value)
{
    GEnumClass *enum_class = g_type_class_ref(enum_type);
    GEnumValue *enum_value = g_enum_get_value(enum_class, value);
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST enum_value->value_nick);
    g_type_class_unref(enum_class);
}

static void
malgtk_xml_serialize_gstring(xmlTextWriterPtr writer, 
                             const char *element,
                             GString *str)
{
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST str->str);
}

static void
malgtk_xml_serialize_maldate(xmlTextWriterPtr writer,
                             const char *element,
                             const MalgtkDate *date)
{
    g_autofree char *str = malgtk_date_get_string(date);
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST str);
}

static void
malgtk_xml_serialize_gdate(xmlTextWriterPtr writer,
                           const char *element,
                           const GDate *date)
{
    if (!g_date_valid(date)) {
        xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST "0000-00-00");
        return;
    }

    xmlTextWriterWriteFormatElement(writer, BAD_CAST element, "%d-%02d-%02d",
                                    g_date_get_year(date),
                                    g_date_get_month(date),
                                    g_date_get_day(date));
}

static void
malgtk_xml_serialize_gdatetime(xmlTextWriterPtr writer,
                               const char *element,
                               GDateTime *datetime)
{
    malgtk_xml_serialize_int64(writer, element, g_date_time_to_unix(datetime));
}

struct key_writer_pair
{
    const char *element;
    xmlTextWriterPtr writer;
};


static gboolean
_gtree_to_writer_cb(gpointer key,
                    gpointer value __attribute__((unused)),
                    gpointer data)
{
    struct key_writer_pair *pair = (struct key_writer_pair*)data;
    xmlTextWriterWriteElement(pair->writer, BAD_CAST pair->element, BAD_CAST key);
    return FALSE;
}

static void malgtk_xml_serialize_tree(xmlTextWriterPtr writer,
                                      const char *element,
                                      const char *subelement,
                                      GTree *tree)
{
    struct key_writer_pair pair = { subelement, writer };

    xmlTextWriterStartElement(writer, BAD_CAST element);
    g_tree_foreach (tree, _gtree_to_writer_cb, &pair);
    xmlTextWriterEndElement(writer);
}


void malgtk_xml_serialize(xmlTextWriterPtr writer, struct malgtk_xml_serialization_defs *defs, gconstpointer priv)
{
    for (; 0 != defs->prop_id; ++defs) {
        if (G_TYPE_GSTRING == defs->type) {
            malgtk_xml_serialize_gstring(writer, defs->xml_name, G_STRUCT_MEMBER(GString*, priv, defs->ofs));
        } else if (MALGTK_TYPE_DATE == defs->type) {
            malgtk_xml_serialize_maldate(writer, defs->xml_name, (MalgtkDate*)G_STRUCT_MEMBER_P(priv, defs->ofs));
        } else if (G_TYPE_TREE == defs->type) {
            malgtk_xml_serialize_tree(writer, defs->xml_name, defs->xml_subname, G_STRUCT_MEMBER(GTree*, priv, defs->ofs));
        } else if (G_TYPE_DATE == defs->type) {
            malgtk_xml_serialize_gdate(writer, defs->xml_name, (GDate*)G_STRUCT_MEMBER_P(priv, defs->ofs));
        } else if (G_TYPE_INT64 == defs->type) {
            malgtk_xml_serialize_int64(writer, defs->xml_name, G_STRUCT_MEMBER(gint64, priv, defs->ofs));
        } else if (G_TYPE_INT == defs->type) {
            malgtk_xml_serialize_int(writer, defs->xml_name, G_STRUCT_MEMBER(gint64, priv, defs->ofs));
        } else if (G_TYPE_DATE_TIME == defs->type) {
            malgtk_xml_serialize_gdatetime(writer, defs->xml_name, G_STRUCT_MEMBER(GDateTime*, priv, defs->ofs));
        } else if (G_TYPE_DOUBLE == defs->type) {
            malgtk_xml_serialize_double(writer, defs->xml_name, G_STRUCT_MEMBER(gdouble, priv, defs->ofs));
        } else if (G_TYPE_BOOLEAN == defs->type) {
            malgtk_xml_serialize_bool(writer, defs->xml_name, G_STRUCT_MEMBER(gboolean, priv, defs->ofs));
        } else if (G_TYPE_IS_ENUM(defs->type)) {
            gint val;
            switch (defs->enum_size) {
                case 1:
                    val = G_STRUCT_MEMBER(gint8, priv, defs->ofs);
                    break;
                case 2:
                    val = G_STRUCT_MEMBER(gint16, priv, defs->ofs);
                    break;
                case 4:
                    val = G_STRUCT_MEMBER(gint32, priv, defs->ofs);
                    break;
                case 8:
                    val = G_STRUCT_MEMBER(gint64, priv, defs->ofs);
                    break;
                default:
                    g_error("Invalid enum size %" G_GSIZE_FORMAT, defs->enum_size);
                    break;
            }
            malgtk_xml_serialize_enum(writer, defs->type, defs->xml_name, val);
        }
    }
}
