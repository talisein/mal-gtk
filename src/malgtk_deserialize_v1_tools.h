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

#include "malgtk_date.h"


static gboolean
xform_gint64(GValue *value, const xmlChar* in)
{
    gint64 i;
    g_value_init(value, G_TYPE_INT64);
    i = g_ascii_strtoll((const gchar*)in, NULL, 0);
    g_value_set_int64(value, i);
    return FALSE;
}

static inline gboolean
xform_gdate(GValue *value, const xmlChar* in)
{
    MalgtkDate d;
    malgtk_date_clear(&d);
    malgtk_date_set_from_string(&d, (const gchar*)in);
    if (malgtk_date_is_complete(&d)) {
        g_value_init(value, G_TYPE_DATE);
        g_value_take_boxed(value, malgtk_date_get_g_date(&d));
        return FALSE;
    } else {
        return TRUE;
    }
}

static inline gboolean
xform_datetime(GValue *value, const xmlChar* in)
{
    gint64 t;
    GDateTime *dt;

    t  = g_ascii_strtoll((const gchar*)in, NULL, 0);
    dt = g_date_time_new_from_unix_utc (t);
    g_value_init(value, G_TYPE_DATE_TIME);
    g_value_take_boxed(value, dt);
    return FALSE;
}

static gboolean
xform_gdouble(GValue *value, const xmlChar* in)
{
    gdouble i;
    g_value_init(value, G_TYPE_DOUBLE);
    i = g_ascii_strtod((const gchar*)in, NULL);
    g_value_set_double(value, i);
    return FALSE;
}

static gboolean
xform_enum(GValue *value, GType type, const xmlChar* in)
{
    GEnumClass* enum_class;
    GEnumValue* enum_value;

    enum_class = g_type_class_ref(type);
    enum_value = g_enum_get_value_by_nick(enum_class, (const char*)in);
    if (G_UNLIKELY(NULL == enum_value)) {
        g_warning("Unrecognized %s nick: %s", G_ENUM_CLASS_TYPE_NAME(enum_class), (const char*)in);
        return TRUE;
    }
    g_value_init(value, type);
    g_value_set_enum(value, enum_value->value);
    g_type_class_unref(enum_class);
    return FALSE;
}

