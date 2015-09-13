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

#include <stdio.h>
#include "malgtk_manga.h"
#include "malgtk_enumtypes.h"
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

    switch (property_id)
    {
        case PROP_SERIES_TYPE:
            priv->series_type = g_value_get_enum (value);
            break;
        case PROP_SERIES_STATUS:
            priv->series_status = g_value_get_enum (value);
            break;
        case PROP_SERIES_CHAPTERS:
            priv->series_chapters = g_value_get_int (value);
            break;
        case PROP_SERIES_VOLUMES:
            priv->series_volumes = g_value_get_int (value);
            break;
        case PROP_STATUS:
            priv->status = g_value_get_enum (value);
            break;
        case PROP_CHAPTERS:
            priv->chapters = g_value_get_int (value);
            break;
        case PROP_VOLUMES:
            priv->volumes = g_value_get_int (value);
            break;
        case PROP_REREADING_CHAPTER:
            priv->rereading_chapter = g_value_get_int (value);
            break;
        case PROP_RETAIL_VOLUMES:
            priv->retail_volumes = g_value_get_int (value);
            break;
        case PROP_STORAGE_TYPE:
            priv->storage_type = g_value_get_enum (value);
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
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_STATUS] =
        g_param_spec_enum ("series-status",
                           "Series Status",
                           "Publishing status of the manga series",
                           MALGTK_TYPE_MANGA_SERIES_STATUS,
                           MALGTK_MANGA_SERIES_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_CHAPTERS] =
        g_param_spec_int ("series-chapters",
                          "Series Chapters",
                          "Number of chapters in the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_VOLUMES] =
        g_param_spec_int ("series-volumes",
                          "Series Volumes",
                          "Number of volumes in the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_STATUS] =
        g_param_spec_enum ("status",
                           "Status",
                           "Your status for the series",
                           MALGTK_TYPE_MANGA_STATUS,
                           MALGTK_MANGA_STATUS_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_CHAPTERS] =
        g_param_spec_int ("chapters",
                          "Chapters",
                          "Number of chapters you've read",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_VOLUMES] =
        g_param_spec_int ("volumes",
                          "Volumes",
                          "Number of volumes you've read",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_REREADING_CHAPTER] =
        g_param_spec_int ("rereading-chapter",
                          "Rereading Chapter",
                          "Which chapter you are rereading",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_RETAIL_VOLUMES] =
        g_param_spec_int ("retail-volumes",
                          "Retail Volumes",
                          "Number of retail volumes you own",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_STORAGE_TYPE] =
        g_param_spec_enum ("storage-type",
                           "Storage Type",
                           "Medium you are storing the series with",
                           MALGTK_TYPE_MANGA_STORAGE_TYPE,
                           MALGTK_MANGA_STORAGE_TYPE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (gobject_class,
                                       N_PROPERTIES,
                                       obj_properties);
}
static void
malgtk_manga_init (MalgtkManga *self)
{
    (void)self;
    //MalgtkMangaPrivate *priv = malgtk_manga_get_instance_private (self);
    //(void)priv;
}

MalgtkManga *
malgtk_manga_new(void)
{
    return g_object_new(MALGTK_TYPE_MANGA, NULL);
}

