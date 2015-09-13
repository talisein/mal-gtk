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
#include <string.h>
#include "malgtk_malitem.h"
#include "malgtk_date.h"
#include "malgtk_enumtypes.h"

typedef struct _MalgtkMalitemPrivate
{
    gint64 mal_db_id;
    GString *series_title;
    GString *preferred_title;
    MalgtkDate series_begin;
    MalgtkDate series_end;
    GString *image_url;
    GTree *series_synonyms;
    GString *synopsis;

    GTree *tags;
    GDate date_start;
    GDate date_finish;
    gint64 id;
    GDateTime* last_updated;
    double score;
    gboolean enable_reconsuming;

    GString *fansub_group;
    GString *comments;
    gint downloaded_items;
    gint times_consumed;
    MalgtkMalitemReconsumeValue reconsume_value;
    MalgtkMalitemPriority priority;
    gboolean enable_discussion;

    gboolean has_details;

    GStringChunk *chunk;
    
} MalgtkMalitemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (MalgtkMalitem, malgtk_malitem, G_TYPE_OBJECT)

static gchar ** _tree_to_gstrv(GTree *tree);
static void     _tree_remove_all(GTree *tree);

enum
{
    PROP_0,
    PROP_SERIES_MALDB_ID,
    PROP_SERIES_TITLE,
    PROP_SERIES_PREFERRED_TITLE,
    PROP_SERIES_DATE_BEGIN,
    PROP_SERIES_DATE_END,
    PROP_SEASON_BEGIN,
    PROP_SEASON_END,
    PROP_IMAGE_URL,
    PROP_SERIES_SYNONYM,
    PROP_SERIES_SYNOPSIS,
    PROP_TAGS,
    PROP_DATE_START,
    PROP_DATE_FINISH,
    PROP_ID,
    PROP_LAST_UPDATED,
    PROP_SCORE,
    PROP_ENABLE_RECONSUMING,
    PROP_FANSUB_GROUP,
    PROP_COMMENTS,
    PROP_DOWNLOADED_ITEMS,
    PROP_TIMES_CONSUMED,
    PROP_RECONSUME_VALUE,
    PROP_PRIORITY,
    PROP_ENABLE_DISCUSSION,
    PROP_HAS_DETAILS,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

static void
malgtk_malitem_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    MalgtkMalitem *self = MALGTK_MALITEM (object);
    MalgtkMalitemPrivate *priv = malgtk_malitem_get_instance_private (self);
    gchar **strv;

    switch (property_id)
    {
        case PROP_SERIES_MALDB_ID:
            priv->mal_db_id = g_value_get_int64 (value);
            break;
        case PROP_SERIES_TITLE:
            g_string_assign(priv->series_title, g_value_get_string (value));
            break;
        case PROP_SERIES_PREFERRED_TITLE:
            g_string_assign(priv->preferred_title, g_value_get_string (value));
            break;
        case PROP_SERIES_DATE_BEGIN:
            priv->series_begin = *(MalgtkDate*)g_value_get_boxed(value);
            g_object_notify_by_pspec(object, obj_properties[PROP_SEASON_BEGIN]);
            break;
        case PROP_SERIES_DATE_END:
            priv->series_end = *(MalgtkDate*)g_value_get_boxed(value);
            g_object_notify_by_pspec(object, obj_properties[PROP_SEASON_END]);
            break;
        case PROP_SEASON_BEGIN:
            /* read-only */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        case PROP_SEASON_END:
            /* read-only */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
        case PROP_IMAGE_URL:
            g_string_assign(priv->image_url, g_value_get_string (value));
            break;
        case PROP_SERIES_SYNONYM:
            _tree_remove_all(priv->series_synonyms);
            strv = g_value_get_boxed(value);
            for (; NULL != *strv; ++strv) {
                if (!g_tree_lookup(priv->series_synonyms, *strv))
                    g_tree_insert(priv->series_synonyms,
                                  g_string_chunk_insert_const(priv->chunk, *strv), NULL);
            }
            break;
        case PROP_SERIES_SYNOPSIS:
            g_string_assign (priv->synopsis, g_value_get_string (value));
            break;
        case PROP_TAGS:
            _tree_remove_all(priv->tags);
            strv = g_value_get_boxed(value);
            for (; NULL != *strv; ++strv) {
                if (!g_tree_lookup(priv->tags, *strv))
                    g_tree_insert(priv->tags,
                                  g_string_chunk_insert_const(priv->chunk, *strv), NULL);
            }
            break;
        case PROP_DATE_START:
            priv->date_start = *(GDate*)g_value_get_boxed(value);
            break;
        case PROP_DATE_FINISH:
            priv->date_finish = *(GDate*)g_value_get_boxed(value);
            break;
        case PROP_ID:
            priv->id = g_value_get_int64 (value);
            break;
        case PROP_LAST_UPDATED:
            g_date_time_unref (priv->last_updated);
            priv->last_updated = g_value_dup_boxed(value);
            break;
        case PROP_SCORE:
            priv->score = g_value_get_double (value);
            break;
        case PROP_ENABLE_RECONSUMING:
            priv->enable_reconsuming = g_value_get_boolean (value);
            break;
        case PROP_FANSUB_GROUP:
            g_string_assign (priv->fansub_group, g_value_get_string (value));
            break;
        case PROP_COMMENTS:
            g_string_assign (priv->comments, g_value_get_string (value));
            break;
        case PROP_DOWNLOADED_ITEMS:
            priv->downloaded_items = g_value_get_int (value);
            break;
        case PROP_TIMES_CONSUMED:
            priv->times_consumed = g_value_get_int (value);
            break;
        case PROP_RECONSUME_VALUE:
            priv->reconsume_value = g_value_get_enum (value);
            break;
        case PROP_PRIORITY:
            priv->priority = g_value_get_enum (value);
            break;
        case PROP_ENABLE_DISCUSSION:
            priv->enable_discussion = g_value_get_boolean (value);
            break;
        case PROP_HAS_DETAILS:
            priv->has_details = g_value_get_boolean (value);
            break;
        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
 
}

static void
malgtk_malitem_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
    MalgtkMalitem *self = MALGTK_MALITEM (object);
    MalgtkMalitemPrivate *priv = malgtk_malitem_get_instance_private (self);
    
    switch (property_id)
    {
        case PROP_SERIES_MALDB_ID:
            g_value_set_int64 (value, priv->mal_db_id);
            break;
        case PROP_SERIES_TITLE:
            g_value_set_string (value, priv->series_title->str);
            break;
        case PROP_SERIES_PREFERRED_TITLE:
            g_value_set_string (value, priv->preferred_title->str);
            break;
        case PROP_SERIES_DATE_BEGIN:
            g_value_set_boxed(value, &priv->series_begin);
            break;
        case PROP_SERIES_DATE_END:
            g_value_set_boxed(value, &priv->series_end);
            break;
        case PROP_SEASON_BEGIN:
            g_value_take_string(value, malgtk_date_get_season(&priv->series_begin));
            break;
        case PROP_SEASON_END:
            g_value_take_string(value, malgtk_date_get_season(&priv->series_end));
            break;
        case PROP_IMAGE_URL:
            g_value_set_string (value, priv->image_url->str);
            break;
        case PROP_SERIES_SYNONYM:
            g_value_take_boxed (value, _tree_to_gstrv(priv->series_synonyms));
            break;
        case PROP_SERIES_SYNOPSIS:
            g_value_set_string (value, priv->synopsis->str);
            break;
        case PROP_TAGS:
            g_value_take_boxed (value, _tree_to_gstrv(priv->tags));
            break;
        case PROP_DATE_START:
            g_value_set_boxed (value, &priv->date_start);
            break;
        case PROP_DATE_FINISH:
            g_value_set_boxed (value, &priv->date_finish);
            break;
        case PROP_ID:
            g_value_set_int64 (value, priv->id);
            break;
        case PROP_LAST_UPDATED:
            g_value_set_boxed (value, priv->last_updated);
            break;
        case PROP_SCORE:
            g_value_set_double (value, priv->score);
            break;
        case PROP_ENABLE_RECONSUMING:
            g_value_set_boolean (value, priv->enable_reconsuming);
            break;
        case PROP_FANSUB_GROUP:
            g_value_set_string (value, priv->fansub_group->str);
            break;
        case PROP_COMMENTS:
            g_value_set_string (value, priv->comments->str);
            break;
        case PROP_DOWNLOADED_ITEMS:
            g_value_set_int (value, priv->downloaded_items);
            break;
        case PROP_TIMES_CONSUMED:
            g_value_set_int (value, priv->times_consumed);
            break;
        case PROP_RECONSUME_VALUE:
            g_value_set_enum (value, priv->reconsume_value);
            break;
        case PROP_PRIORITY:
            g_value_set_enum (value, priv->priority);
            break;
        case PROP_ENABLE_DISCUSSION:
            g_value_set_boolean (value, priv->enable_discussion);
            break;
        case PROP_HAS_DETAILS:
            g_value_set_boolean (value, priv->has_details);
            break;

        default:
            /* We don't have any other property... */
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

static void
malgtk_malitem_dispose (GObject *obj)
{
    G_OBJECT_CLASS (malgtk_malitem_parent_class)->dispose (obj);
}

static void
malgtk_malitem_finalize (GObject *obj)
{
    MalgtkMalitemPrivate *priv = malgtk_malitem_get_instance_private (MALGTK_MALITEM(obj));
    g_string_free (priv->series_title, TRUE);
    g_string_free (priv->preferred_title, TRUE);
    g_string_free (priv->image_url, TRUE);
    g_tree_unref (priv->series_synonyms);
    g_string_free (priv->synopsis, TRUE);

    g_tree_unref (priv->tags);
    g_date_time_unref(priv->last_updated);

    g_string_free (priv->fansub_group, TRUE);
    g_string_free (priv->comments, TRUE);

    g_string_chunk_free (priv->chunk);

    G_OBJECT_CLASS (malgtk_malitem_parent_class)->finalize (obj);
}


static void
malgtk_malitem_class_init (MalgtkMalitemClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->set_property = malgtk_malitem_set_property;
    gobject_class->get_property = malgtk_malitem_get_property;
    gobject_class->dispose      = malgtk_malitem_dispose;
    gobject_class->finalize     = malgtk_malitem_finalize;
        
    obj_properties[PROP_SERIES_MALDB_ID] =
        g_param_spec_int64 ("mal-db-id",
                            "MAL.net id",
                            "myanimelist.net database id",
                            G_MININT64,
                            G_MAXINT64,
                            0,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_TITLE] =
        g_param_spec_string ("series-title",
                             "Series Title",
                             "Title of the series or franchise",
                             "no-series-title-set",
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_PREFERRED_TITLE] =
        g_param_spec_string ("preferred-title",
                             "Preferred Title",
                             "Your defined override for series title",
                             "no-preferred-series-title-set",
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    
    obj_properties[PROP_SERIES_DATE_BEGIN] = 
        g_param_spec_boxed ("series-date-begin",
                            "Series Begin",
                            "Date the series begins",
                            MALGTK_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_DATE_END] = 
        g_param_spec_boxed ("series-date-end",
                            "Series End",
                            "Date the series ends",
                            MALGTK_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SEASON_BEGIN] = 
        g_param_spec_string ("season-begin",
                             "Season Start",
                             "Season for the beginning of the series",
                             "no-season-begin-set",
                             G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SEASON_END] = 
        g_param_spec_string ("season-end",
                             "Season End",
                             "Season for the ending of the series",
                             "no-season-end-set",
                             G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_IMAGE_URL] =
        g_param_spec_string ("image-url",
                             "Image URL",
                             "URL for image representing the series",
                             "no-image-url-set",
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_SERIES_SYNONYM] =
        g_param_spec_boxed ("series-synonyms",
                            "Series Synonyms",
                            "Set of synonyms for the series title",
                            G_TYPE_STRV,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    
    obj_properties[PROP_SERIES_SYNOPSIS] =
        g_param_spec_string ("series-synopsis",
                             "Synopsis",
                             "Synopsis of the series",
                             "no-series-synopsis-set",
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_TAGS] =
        g_param_spec_boxed ("tags",
                            "Tags",
                            "Tags associated with the series",
                            G_TYPE_STRV,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    
    obj_properties[PROP_DATE_START] = 
        g_param_spec_boxed ("date-start",
                            "Started",
                            "Date you started watching or reading the series",
                            G_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    
    obj_properties[PROP_DATE_FINISH] = 
        g_param_spec_boxed ("date-finish",
                            "Date Finished",
                            "Date you finished watching or reading the series",
                            G_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    
    obj_properties[PROP_ID] =
        g_param_spec_int64 ("id",
                            "id",
                            "myanimelist.net db id for your data",
                            G_MININT64,
                            G_MAXINT64,
                            0,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_LAST_UPDATED] = 
        g_param_spec_boxed ("last-updated",
                            "Last Updated",
                            "Last time data was updated on mal.net",
                            G_TYPE_DATE_TIME,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    
    obj_properties[PROP_SCORE] =
        g_param_spec_double ("score",
                             "Score",
                             "Score reflecting series quality",
                             0.0,
                             10.0,
                             0.0,
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_ENABLE_RECONSUMING] = 
        g_param_spec_boolean ("enable-reconsuming",
                              "Reconsuming",
                              "Indicates you are rewatching or rereading the series",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_FANSUB_GROUP] =
        g_param_spec_string ("fansub-group",
                             "Fansub Group",
                             "Fansub Group you are using",
                             "no-fansub-group-set",
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_COMMENTS] =
        g_param_spec_string ("comments",
                             "Comments",
                             "Your comments",
                             "no-comments-set",
                             G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_DOWNLOADED_ITEMS] =
        g_param_spec_int ("downloaded-items",
                          "Downloaded Items",
                          "Number of downloaded episodes or chapters",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_TIMES_CONSUMED] =
        g_param_spec_int ("times-consumed",
                          "Times Consumed",
                          "Number of times you've watched or read the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_RECONSUME_VALUE] =
        g_param_spec_enum ("reconsume-value",
                           "Reconsumability",
                           "How rewatchable or rereadable the series is",
                           MALGTK_TYPE_MALITEM_RECONSUME_VALUE,
                           MALGTK_MALITEM_RECONSUME_VALUE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
        
    obj_properties[PROP_PRIORITY] =
        g_param_spec_enum ("priority",
                           "Priority",
                           "Priority for watching or reading this series",
                           MALGTK_TYPE_MALITEM_PRIORITY,
                           MALGTK_MALITEM_PRIORITY_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
        
    obj_properties[PROP_ENABLE_DISCUSSION] = 
        g_param_spec_boolean ("enable-discussion",
                              "Enable Discussion",
                              "Whether the series can be discussed on your myanimelist.net page",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    obj_properties[PROP_HAS_DETAILS] = 
        g_param_spec_boolean ("has-details",
                              "Has Details",
                              "Indicates mal-gtk has downloaded all possible details for this series",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (gobject_class,
                                       N_PROPERTIES,
                                       obj_properties);

}

static gint
_gcompare_str(gconstpointer a, gconstpointer b, gpointer user_data)
{
    (void) user_data;
    return strcoll(a, b);
}

static void
malgtk_malitem_init (MalgtkMalitem *self)
{
    MalgtkMalitemPrivate *priv = malgtk_malitem_get_instance_private (self);
    priv->series_title    = g_string_new(NULL);
    priv->preferred_title = g_string_new(NULL);
    malgtk_date_clear(&priv->series_begin);
    malgtk_date_clear(&priv->series_end);
    priv->image_url       = g_string_new(NULL);
    priv->series_synonyms = g_tree_new_full(_gcompare_str, NULL, NULL, NULL);
    priv->synopsis        = g_string_new(NULL);

    priv->tags            = g_tree_new_full(_gcompare_str, NULL, NULL, NULL);
    g_date_clear(&priv->date_start, 1);
    g_date_clear(&priv->date_finish, 1);
    priv->last_updated    = g_date_time_new_from_unix_utc (0);
    priv->fansub_group    = g_string_new(NULL);
    priv->comments        = g_string_new(NULL);
    priv->reconsume_value = MALGTK_MALITEM_RECONSUME_VALUE_INVALID;
    priv->priority        = MALGTK_MALITEM_PRIORITY_INVALID;

    priv->chunk           = g_string_chunk_new(128);
}



MalgtkMalitem *
malgtk_malitem_new(void)
{
    return g_object_new(MALGTK_TYPE_MALITEM, NULL);
}

void
malgtk_malitem_add_synonym(MalgtkMalitem *item, const gchar *synonym)
{
    MalgtkMalitemPrivate *priv;
    gpointer p;

    g_return_if_fail(MALGTK_IS_MALITEM(item));
    g_return_if_fail(NULL != synonym && 0 != *synonym);

    priv = malgtk_malitem_get_instance_private (item);


    p = g_tree_lookup(priv->series_synonyms, synonym);
    if (p)
        return;

    g_tree_insert(priv->series_synonyms,
                  g_string_chunk_insert_const(priv->chunk, synonym), NULL);

    g_object_notify_by_pspec(G_OBJECT(item), obj_properties[PROP_SERIES_SYNONYM]);
}

void
malgtk_malitem_add_tag(MalgtkMalitem *item, const gchar *tag)
{
    MalgtkMalitemPrivate *priv;
    gpointer p;

    g_return_if_fail(MALGTK_IS_MALITEM(item));
    g_return_if_fail(NULL != tag && 0 != *tag);

    priv = malgtk_malitem_get_instance_private (item);

    p = g_tree_lookup(priv->tags, tag);
    if (p)
        return;

    g_tree_insert(priv->tags,
                  g_string_chunk_insert_const(priv->chunk, tag), NULL);

    g_object_notify_by_pspec(G_OBJECT(item), obj_properties[PROP_TAGS]);
}

struct _foreach_data {
    MalgtkSetForeachFunc cb;
    gpointer user_data;
};

static gboolean
_foreach_cb(gpointer key, gpointer value, gpointer data)
{
    struct _foreach_data *bkt = data;
    return bkt->cb(key, bkt->user_data);
}

void
malgtk_malitem_foreach_synonym(const MalgtkMalitem *item, MalgtkSetForeachFunc cb, gpointer user_data)
{
    MalgtkMalitemPrivate *priv;
    struct _foreach_data data = {cb, user_data};
    g_return_if_fail(MALGTK_IS_MALITEM(item));

    priv = malgtk_malitem_get_instance_private (item);
    g_tree_foreach (priv->series_synonyms, _foreach_cb, &data);
}

void
malgtk_malitem_foreach_tag(const MalgtkMalitem *item, MalgtkSetForeachFunc cb, gpointer user_data)
{
    MalgtkMalitemPrivate *priv;
    struct _foreach_data data = {cb, user_data};
    g_return_if_fail(MALGTK_IS_MALITEM(item));
    priv = malgtk_malitem_get_instance_private (item);
    g_tree_foreach (priv->tags, _foreach_cb, &data);
}

static gboolean 
_tree_to_gstrv_cb(gpointer key, gpointer value, gpointer data)
{
    gchar ***strvp = data; 
    **strvp = g_strdup(key);
    ++*strvp;
    return FALSE;
}

static gchar **
_tree_to_gstrv(GTree *tree)
{
    gchar **strv = g_malloc(sizeof(gchar*) * (g_tree_nnodes(tree)+1));
    gchar **res = strv;
    g_tree_foreach (tree, _tree_to_gstrv_cb, &strv);
    *strv = NULL;
    return res;
}

static void _tree_remove_all(GTree *tree)
{
    g_tree_ref (tree);
    g_tree_destroy (tree);
}
