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

#include "malgtk_anime.h"
#include "malgtk_enumtypes.h"
#include "malgtk_deserialize_v1_tools.h"
#include "malgtk_xml.h"

typedef struct _MalgtkAnime
{
    MalgtkMalitem parent_instance;
} MalgtkAnime;

typedef struct _MalgtkAnimePrivate
{
    MalgtkAnimeSeriesType series_type;
    MalgtkAnimeSeriesStatus series_status;
    gint series_episodes;

    MalgtkAnimeStatus status;
    gint episodes;
    gint rewatch_episode;

    MalgtkAnimeStorageType storage_type;
    gdouble storage_value;
} MalgtkAnimePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MalgtkAnime, malgtk_anime, MALGTK_TYPE_MALITEM)

static inline gconstpointer
malgtk_anime_get_instance_private_const (const MalgtkAnime *self)
{
  return (G_STRUCT_MEMBER_P (self, MalgtkAnime_private_offset));
}

enum
{
    PROP_0,
    PROP_SERIES_TYPE,
    PROP_SERIES_STATUS,
    PROP_SERIES_EPISODES,
    PROP_STATUS,
    PROP_EPISODES,
    PROP_REWATCH_EPISODE,
    PROP_STORAGE_TYPE,
    PROP_STORAGE_VALUE,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
malgtk_anime_set_property (GObject      *object,
                           guint         property_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
    MalgtkAnime *self = MALGTK_ANIME (object);
    MalgtkAnimePrivate *priv = malgtk_anime_get_instance_private (self);
    gint i;
    gdouble d;

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
                priv->series_status = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_STATUS]);
            }
            break;
        case PROP_SERIES_EPISODES:
            i = g_value_get_int (value);
            if (i != priv->series_episodes) {
                priv->series_episodes = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_EPISODES]);
            }
            break;
        case PROP_STATUS:
            i = g_value_get_enum (value);
            if (i != priv->status) {
                priv->status = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_STATUS]);
            }
            break;
        case PROP_EPISODES:
            i = g_value_get_int (value);
            if (i != priv->episodes) {
                priv->episodes = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_EPISODES]);
            }
            break;
        case PROP_REWATCH_EPISODE:
            i = g_value_get_int (value);
            if (i != priv->rewatch_episode) {
                priv->rewatch_episode = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_REWATCH_EPISODE]);
            }
            break;
        case PROP_STORAGE_TYPE:
            i = g_value_get_enum (value);
            if ((MalgtkAnimeStorageType)i != priv->storage_type) {
                priv->storage_type = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_STORAGE_TYPE]);
            }
            break;
        case PROP_STORAGE_VALUE:
            d = g_value_get_double (value);
            if (d != priv->storage_value) {
                priv->storage_value = d;
                g_object_notify_by_pspec(object, obj_properties[PROP_STORAGE_VALUE]);
            }
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

static void
malgtk_anime_get_property (GObject    *object,
                           guint       property_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
    MalgtkAnime *self = MALGTK_ANIME (object);
    MalgtkAnimePrivate *priv = malgtk_anime_get_instance_private (self);

    switch (property_id)
    {
        case PROP_SERIES_TYPE:
            g_value_set_enum (value, priv->series_type);
            break;
        case PROP_SERIES_STATUS:
            g_value_set_enum (value, priv->series_status);
            break;
        case PROP_SERIES_EPISODES:
            g_value_set_int (value, priv->series_episodes);
            break;
        case PROP_STATUS:
            g_value_set_enum (value, priv->status);
            break;
        case PROP_EPISODES:
            g_value_set_int (value, priv->episodes);
            break;
        case PROP_REWATCH_EPISODE:
            g_value_set_int (value, priv->rewatch_episode);
            break;
        case PROP_STORAGE_TYPE:
            g_value_set_enum (value, priv->storage_type);
            break;
        case PROP_STORAGE_VALUE:
            g_value_set_double (value, priv->storage_value);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

static void
malgtk_anime_dispose (GObject *obj)
{
    G_OBJECT_CLASS (malgtk_anime_parent_class)->dispose (obj);
}

static void
malgtk_anime_finalize (GObject *obj)
{
    //MalgtkAnimePrivate *priv = malgtk_anime_get_instance_private (MALGTK_ANIME(obj));
//    (void)priv;
    G_OBJECT_CLASS (malgtk_anime_parent_class)->finalize (obj);
}

static void
malgtk_anime_class_init (MalgtkAnimeClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->set_property = malgtk_anime_set_property;
    gobject_class->get_property = malgtk_anime_get_property;
    gobject_class->dispose      = malgtk_anime_dispose;
    gobject_class->finalize     = malgtk_anime_finalize;

    obj_properties[PROP_SERIES_TYPE] =
        g_param_spec_enum ("series-type",
                           "Series Type",
                           "Type of media this series is",
                           MALGTK_TYPE_ANIME_SERIES_TYPE,
                           MALGTK_ANIME_SERIES_TYPE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_STATUS] =
        g_param_spec_enum ("series-status",
                           "Series Status",
                           "Airing status of the anime",
                           MALGTK_TYPE_ANIME_SERIES_STATUS,
                           MALGTK_ANIME_SERIES_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_EPISODES] =
        g_param_spec_int ("series-episodes",
                          "Series Episodes",
                          "Number of episodes in the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_STATUS] =
        g_param_spec_enum ("status",
                           "Status",
                           "Your status for the series",
                           MALGTK_TYPE_ANIME_STATUS,
                           MALGTK_ANIME_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_EPISODES] =
        g_param_spec_int ("episodes",
                          "Episodes",
                          "Number of episodes you've watched",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_REWATCH_EPISODE] =
        g_param_spec_int ("rewatch-episode",
                          "Rewatch Episode",
                          "Which episode you are rewatching",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_STORAGE_TYPE] =
        g_param_spec_enum ("storage-type",
                           "Storage Type",
                           "Medium you are storing the series with",
                           MALGTK_TYPE_ANIME_STORAGE_TYPE,
                           MALGTK_ANIME_STORAGE_TYPE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_STORAGE_VALUE] =
        g_param_spec_double ("storage-value",
                             "Storage Value",
                             "How many bytes of storage are used",
                             0.0,
                             G_MAXDOUBLE,
                             0.0,
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    g_object_class_install_properties (gobject_class,
                                       N_PROPERTIES,
                                       obj_properties);
}

static void
malgtk_anime_init (MalgtkAnime *self)
{
    MalgtkAnimePrivate *priv = malgtk_anime_get_instance_private (self);
    (void)priv;
}

MalgtkAnime *
malgtk_anime_new(void)
{
    return g_object_new(MALGTK_TYPE_ANIME, NULL);
}

static gboolean
xform_series_type_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_ANIME_SERIES_TYPE, in);
}

static gboolean
xform_series_status_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_ANIME_SERIES_STATUS, in);
}

static gboolean
xform_status_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_ANIME_STATUS, in);
}

static gboolean
xform_storage_type_enum(GValue *value, const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_ANIME_STORAGE_TYPE, in);
}

static const struct {
    const xmlChar *xml_name;
    gint prop_id;
    gboolean (*xform)(GValue *out, const xmlChar* in);
} field_map[] = {
    {BAD_CAST"MALitem",         -1,                   NULL},
    {BAD_CAST"anime",           -1,                   NULL},
    {BAD_CAST"series_type",     PROP_SERIES_TYPE,     xform_series_type_enum},
    {BAD_CAST"series_status",   PROP_SERIES_STATUS,   xform_series_status_enum},
    {BAD_CAST"series_episodes", PROP_SERIES_EPISODES, xform_gint64},
    {BAD_CAST"status",          PROP_STATUS,          xform_status_enum},
    {BAD_CAST"episodes",        PROP_EPISODES,        xform_gint64},
    {BAD_CAST"rewatch_episode", PROP_REWATCH_EPISODE, xform_gint64},
    {BAD_CAST"storage_type",    PROP_STORAGE_TYPE,    xform_storage_type_enum},
    {BAD_CAST"storage_value",   PROP_STORAGE_VALUE,   xform_gdouble},
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
malgtk_anime_set_from_xml(MalgtkAnime *anime,
                          xmlTextReaderPtr reader)
{
    gint offset = -1;
    GValue value = G_VALUE_INIT;
    gboolean is_default;
    g_return_if_fail(MALGTK_IS_ANIME(anime));

    for (; !(xmlStrEqual(BAD_CAST"anime", xmlTextReaderConstName(reader)) &&
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
                    malgtk_malitem_set_from_xml (MALGTK_MALITEM(anime), reader);
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
                            g_object_set_property(G_OBJECT(anime),
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

static GOnce s_defs_once = G_ONCE_INIT;
static struct malgtk_xml_serialization_defs s_defs[N_PROPERTIES] = { 0 };
static void*
_init_s_defs(void* v)
{
    (void)v;
    s_defs[0] = (struct malgtk_xml_serialization_defs){
        MALGTK_TYPE_ANIME_SERIES_TYPE, PROP_SERIES_TYPE, "series-type", "series_type",
        NULL, offsetof(MalgtkAnimePrivate, series_type), sizeof(MalgtkAnimeSeriesType) };

    MALGTK_ENUM_IS_SERIALIZABLE(MalgtkAnimeSeriesType);

    s_defs[1] = (struct malgtk_xml_serialization_defs){
        MALGTK_TYPE_ANIME_SERIES_STATUS, PROP_SERIES_STATUS, "series-status", "series_status",
        NULL, offsetof(MalgtkAnimePrivate, series_status), sizeof(MalgtkAnimeSeriesStatus) };

    MALGTK_ENUM_IS_SERIALIZABLE(MalgtkAnimeSeriesStatus);

    s_defs[2] = (struct malgtk_xml_serialization_defs){
        G_TYPE_INT, PROP_SERIES_EPISODES, "series-episodes", "series_episodes",
        NULL, offsetof(MalgtkAnimePrivate, series_episodes), 0 };

    s_defs[3] = (struct malgtk_xml_serialization_defs){
        MALGTK_TYPE_ANIME_STATUS, PROP_STATUS, "status", "status",
        NULL, offsetof(MalgtkAnimePrivate, series_status), sizeof(MalgtkAnimeStatus) };

    MALGTK_ENUM_IS_SERIALIZABLE(MalgtkAnimeStatus);

    s_defs[4] = (struct malgtk_xml_serialization_defs){
        G_TYPE_INT, PROP_EPISODES, "episodes", "episodes",
        NULL, offsetof(MalgtkAnimePrivate, episodes), 0 };

    s_defs[5] = (struct malgtk_xml_serialization_defs){
        G_TYPE_INT, PROP_REWATCH_EPISODE, "rewatch-episode", "rewatch_episode",
        NULL, offsetof(MalgtkAnimePrivate, rewatch_episode), 0 };

    s_defs[6] = (struct malgtk_xml_serialization_defs){
        MALGTK_TYPE_ANIME_STORAGE_TYPE, PROP_STORAGE_TYPE, "storage-type", "storage_type",
        NULL, offsetof(MalgtkAnimePrivate, storage_type), sizeof(MalgtkAnimeStorageType) };

    MALGTK_ENUM_IS_SERIALIZABLE(MalgtkAnimeStorageType);

    s_defs[7] = (struct malgtk_xml_serialization_defs){
        G_TYPE_DOUBLE, PROP_STORAGE_VALUE, "storage-value", "storage_value",
        NULL, offsetof(MalgtkAnimePrivate, storage_value), 0 };

    return NULL;
}

void
malgtk_anime_get_xml(const MalgtkAnime *anime,
                     xmlTextWriterPtr writer)
{
    const MalgtkAnimePrivate *priv;
    g_return_if_fail (MALGTK_IS_ANIME((MalgtkAnime*)anime));
    g_once (&s_defs_once, _init_s_defs, NULL);
    priv = malgtk_anime_get_instance_private_const (anime);
    xmlTextWriterStartElement(writer, BAD_CAST"anime");
    xmlTextWriterWriteAttribute(writer, BAD_CAST"version", BAD_CAST"1");
    malgtk_malitem_get_xml(MALGTK_MALITEM((MalgtkAnime*)anime), writer);

    malgtk_xml_serialize(writer, s_defs, priv);

    xmlTextWriterEndElement(writer); /* anime */

}
