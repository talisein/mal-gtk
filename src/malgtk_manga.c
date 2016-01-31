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

#include "malgtk_manga.h"
#include "malgtk_enumtypes.h"
#include "malgtk_deserialize_v1_tools.h"

typedef struct _MalgtkManga MalgtkManga;

typedef struct _MalgtkManga
{
    MalgtkMalitem parent_instance;
} MalgtkManga;

typedef struct _MalgtkMangaPrivate
{
    MalgtkMangaSeriesType series_type;
    MalgtkMangaSeriesStatus series_status;
    gint series_chapters;
    gint series_volumes;

    MalgtkMangaStatus status;
    gint chapters;
    gint volumes;
    gint rereading_chapter;

    gint retail_volumes;

    MalgtkMangaStorageType storage_type;
} MalgtkMangaPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MalgtkManga, malgtk_manga, MALGTK_TYPE_MALITEM);

enum
{
    PROP_0,
    PROP_SERIES_TYPE,
    PROP_SERIES_STATUS,
    PROP_SERIES_CHAPTERS,
    PROP_SERIES_VOLUMES,
    PROP_STATUS,
    PROP_CHAPTERS,
    PROP_VOLUMES,
    PROP_REREADING_CHAPTER,
    PROP_RETAIL_VOLUMES,
    PROP_STORAGE_TYPE,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
malgtk_manga_set_property (GObject      *object,
                           guint         property_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
    MalgtkManga *self = MALGTK_MANGA (object);
    MalgtkMangaPrivate *priv = malgtk_manga_get_instance_private (self);
    gint i;

    switch (property_id)
    {
        case PROP_SERIES_TYPE:
            i = g_value_get_enum (value);
            if (i != priv->series_type) {
                priv->series_type = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_TYPE]);
            }
            break;
        case PROP_SERIES_STATUS:
            i = g_value_get_enum (value);
            if (i != priv->series_status) {
                priv->series_status =  i;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_STATUS]);
            }
            break;
        case PROP_SERIES_CHAPTERS:
            i = g_value_get_int (value);
            if (i != priv->series_chapters) {
                priv->series_chapters = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_CHAPTERS]);
            }
            break;
        case PROP_SERIES_VOLUMES:
            i = g_value_get_int (value);
            if (i != priv->series_volumes) {
                priv->series_volumes = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_VOLUMES]);
            }
            break;
        case PROP_STATUS:
            i = g_value_get_enum (value);
            if (i != priv->status) {
                priv->status = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_STATUS]);
            }
            break;
        case PROP_CHAPTERS:
            i = g_value_get_int (value);
            if (i != priv->chapters) {
                priv->chapters = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_CHAPTERS]);
            }
            break;
        case PROP_VOLUMES:
            i = g_value_get_int (value);
            if (i != priv->volumes) {
                priv->volumes = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_VOLUMES]);
            }
            break;
        case PROP_REREADING_CHAPTER:
            i = g_value_get_int (value);
            if (i != priv->rereading_chapter) {
                priv->rereading_chapter = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_REREADING_CHAPTER]);
            }
            break;
        case PROP_RETAIL_VOLUMES:
            i = g_value_get_int (value);
            if (i != priv->retail_volumes) {
                priv->retail_volumes = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_RETAIL_VOLUMES]);
            }
            break;
        case PROP_STORAGE_TYPE:
            i = g_value_get_enum (value);
            if (i != priv->storage_type) {
                priv->storage_type = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_STORAGE_TYPE]);
            }
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
malgtk_manga_get_property (GObject    *object,
                           guint       property_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
    MalgtkManga *self = MALGTK_MANGA (object);
    MalgtkMangaPrivate *priv = malgtk_manga_get_instance_private (self);

    switch (property_id)
    {
        case PROP_SERIES_TYPE:
            g_value_set_enum (value, priv->series_type);
            break;
        case PROP_SERIES_STATUS:
            g_value_set_enum (value, priv->series_status);
            break;
        case PROP_SERIES_CHAPTERS:
            g_value_set_int (value, priv->series_chapters);
            break;
        case PROP_SERIES_VOLUMES:
            g_value_set_int (value, priv->series_volumes);
            break;
        case PROP_STATUS:
            g_value_set_enum (value, priv->status);
            break;
        case PROP_CHAPTERS:
            g_value_set_int (value, priv->chapters);
            break;
        case PROP_VOLUMES:
            g_value_set_int (value, priv->volumes);
            break;
        case PROP_REREADING_CHAPTER:
            g_value_set_int (value, priv->rereading_chapter);
            break;
        case PROP_RETAIL_VOLUMES:
            g_value_set_int (value, priv->retail_volumes);
            break;
        case PROP_STORAGE_TYPE:
            g_value_set_enum (value, priv->storage_type);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}


static void
malgtk_manga_class_init (MalgtkMangaClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->set_property = malgtk_manga_set_property;
    gobject_class->get_property = malgtk_manga_get_property;

    obj_properties[PROP_SERIES_TYPE] =
        g_param_spec_enum ("series-type",
                           "Series Type",
                           "Type of media this series is",
                           MALGTK_TYPE_MANGA_SERIES_TYPE,
                           MALGTK_MANGA_SERIES_TYPE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_STATUS] =
        g_param_spec_enum ("series-status",
                           "Series Status",
                           "Publishing status of the manga series",
                           MALGTK_TYPE_MANGA_SERIES_STATUS,
                           MALGTK_MANGA_SERIES_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_CHAPTERS] =
        g_param_spec_int ("series-chapters",
                          "Series Chapters",
                          "Number of chapters in the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_VOLUMES] =
        g_param_spec_int ("series-volumes",
                          "Series Volumes",
                          "Number of volumes in the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_STATUS] =
        g_param_spec_enum ("status",
                           "Status",
                           "Your status for the series",
                           MALGTK_TYPE_MANGA_STATUS,
                           MALGTK_MANGA_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_CHAPTERS] =
        g_param_spec_int ("chapters",
                          "Chapters",
                          "Number of chapters you've read",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_VOLUMES] =
        g_param_spec_int ("volumes",
                          "Volumes",
                          "Number of volumes you've read",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_REREADING_CHAPTER] =
        g_param_spec_int ("rereading-chapter",
                          "Rereading Chapter",
                          "Which chapter you are rereading",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_RETAIL_VOLUMES] =
        g_param_spec_int ("retail-volumes",
                          "Retail Volumes",
                          "Number of retail volumes you own",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_STORAGE_TYPE] =
        g_param_spec_enum ("storage-type",
                           "Storage Type",
                           "Medium you are storing the series with",
                           MALGTK_TYPE_MANGA_STORAGE_TYPE,
                           MALGTK_MANGA_STORAGE_TYPE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    g_object_class_install_properties (gobject_class,
                                       N_PROPERTIES,
                                       obj_properties);
}
static void
malgtk_manga_init (MalgtkManga *self)
{
    (void)self;
    /* MalgtkMangaPrivate *priv = malgtk_manga_get_instance_private (self); */
}

MalgtkManga *
malgtk_manga_new(void)
{
    return g_object_new(MALGTK_TYPE_MANGA, NULL);
}

static gboolean
xform_series_type_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_MANGA_SERIES_TYPE, in);
}

static gboolean
xform_series_status_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_MANGA_SERIES_STATUS, in);
}

static gboolean
xform_status_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_MANGA_STATUS, in);
}

static gboolean
xform_storage_type_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_MANGA_STORAGE_TYPE, in);
}

static const struct {
    const xmlChar *xml_name;
    gint prop_id;
    gboolean (*xform)(GValue *out, const xmlChar* in);
} field_map[] = {
    {BAD_CAST"MALitem",           -1,                     NULL},
    {BAD_CAST"manga",             -1,                     NULL},
    {BAD_CAST"series_type",       PROP_SERIES_TYPE,       xform_series_type_enum},
    {BAD_CAST"series_status",     PROP_SERIES_STATUS,     xform_series_status_enum},
    {BAD_CAST"series_chapters",   PROP_SERIES_CHAPTERS,   xform_gint64},
    {BAD_CAST"series_volumes",    PROP_SERIES_VOLUMES,    xform_gint64},
    {BAD_CAST"status",            PROP_STATUS,            xform_status_enum},
    {BAD_CAST"chapters",          PROP_CHAPTERS,          xform_gint64},
    {BAD_CAST"volumes",           PROP_VOLUMES,           xform_gint64},
    {BAD_CAST"rereading_chapter", PROP_REREADING_CHAPTER, xform_gint64},
    {BAD_CAST"retail_volumes",    PROP_RETAIL_VOLUMES,    xform_gint64},
    {BAD_CAST"storage_type",      PROP_STORAGE_TYPE,      xform_storage_type_enum},
};

static gint
find_field_for_name(const xmlChar *name)
{
    for (size_t i = 0; i < G_N_ELEMENTS(field_map); ++i)
    {
        if (xmlStrEqual(field_map[i].xml_name, name)) {
            return i;
        }
    }

    return -1;
}

void
malgtk_manga_set_from_xml(MalgtkManga *manga, xmlTextReaderPtr reader)
{
    gint offset = -1;
    GValue value = G_VALUE_INIT;
    gboolean is_default;
    g_return_if_fail(MALGTK_IS_MANGA(manga));

    for (; !(xmlStrEqual(BAD_CAST"manga", xmlTextReaderConstName(reader)) &&
             xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT);
         xmlTextReaderRead(reader))
    {
        switch (xmlTextReaderNodeType(reader))
        {
            case XML_READER_TYPE_ELEMENT:
                offset = find_field_for_name(xmlTextReaderConstName(reader));
                if (-1 == offset) {
                    g_warning("Unexpected field: %s", (const char*)xmlTextReaderConstName(reader));
                }
                if (0 == offset) {
                    malgtk_malitem_set_from_xml (MALGTK_MALITEM(manga), reader);
                    break;
                }
                break;
            case XML_READER_TYPE_TEXT:
                switch (field_map[offset].prop_id) {
                    case -1:
                        break;
                    default:
                        is_default = field_map[offset].xform(&value,
                                                             xmlTextReaderConstValue(reader));
                        if (!is_default) {
                            g_object_set_property(G_OBJECT(manga),
                                                  obj_properties[field_map[offset].prop_id]->name, &value);
                            g_value_unset(&value);
                        }
                        break;
                }
                break;
            default:
                break;
        }
    }

}
