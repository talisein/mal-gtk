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

typedef struct _MalgtkAnime
{
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
    double storage_value;
} MalgtkAnimePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MalgtkAnime, malgtk_anime, MALGTK_TYPE_MALITEM)

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

    switch (property_id)
    {
        case PROP_SERIES_TYPE:
            priv->series_type = g_value_get_enum (value);
            break;
        case PROP_SERIES_STATUS:
            priv->series_status = g_value_get_enum (value);
            break;
        case PROP_SERIES_EPISODES:
            priv->series_episodes = g_value_get_int (value);
            break;
        case PROP_STATUS:
            priv->status = g_value_get_enum (value);
            break;
        case PROP_EPISODES:
            priv->episodes = g_value_get_int (value);
            break;
        case PROP_REWATCH_EPISODE:
            priv->rewatch_episode = g_value_get_int (value);
            break;
        case PROP_STORAGE_TYPE:
            priv->storage_type = g_value_get_enum (value);
            break;
        case PROP_STORAGE_VALUE:
            priv->storage_value = g_value_get_double (value);
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
};

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
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_STATUS] =
        g_param_spec_enum ("series-status",
                           "Series Status",
                           "Airing status of the anime",
                           MALGTK_TYPE_ANIME_SERIES_STATUS,
                           MALGTK_ANIME_SERIES_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_EPISODES] =
        g_param_spec_int ("series-episodes",
                          "Series Episodes",
                          "Number of episodes in the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_STATUS] =
        g_param_spec_enum ("status",
                           "Status",
                           "Your status for the series",
                           MALGTK_TYPE_ANIME_STATUS,
                           MALGTK_ANIME_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_EPISODES] =
        g_param_spec_int ("episodes",
                          "Episodes",
                          "Number of episodes you've watched",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_REWATCH_EPISODE] =
        g_param_spec_int ("rewatch-episode",
                          "Rewatch Episode",
                          "Which episode you are rewatching",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_STORAGE_TYPE] =
        g_param_spec_enum ("storage-type",
                           "Storage Type",
                           "Medium you are storing the series with",
                           MALGTK_TYPE_ANIME_STORAGE_TYPE,
                           MALGTK_ANIME_STORAGE_TYPE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_REWATCH_EPISODE] =
        g_param_spec_double ("storage_value",
                             "Storage Value",
                             "How many bytes of storage are used",
                             0.0,
                             G_MAXDOUBLE,
                             0.0,
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

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

