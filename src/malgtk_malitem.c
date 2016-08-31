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

#include <string.h>
#include "malgtk_malitem.h"
#include "malgtk_date.h"
#include "malgtk_enumtypes.h"
#include "malgtk_deserialize_v1_tools.h"

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

struct strv_tree_data
{
    const gchar * const * strv;
    gboolean is_equal;
};

static gboolean
strv_tree_eq_fn(gpointer key,
                gpointer value,
                gpointer data)
{
    struct strv_tree_data *d = (struct strv_tree_data*)data;
    if (!g_strv_contains(d->strv, (const gchar*)key)) {
        d->is_equal = FALSE;
        return TRUE;
    }

    return FALSE;
}

static gboolean
is_strv_tree_equal(gchar **strv,
                   GTree *tree)
{
    struct strv_tree_data d;

    for (gchar **s = strv; NULL != *s; ++s) {
        if (!g_tree_lookup_extended (tree, *s, NULL, NULL)) {
            return FALSE;
        }
    }

    d.strv     = (const gchar * const *)strv;
    d.is_equal = TRUE;

    g_tree_foreach(tree, strv_tree_eq_fn, &d);

    return d.is_equal;
}

static void
malgtk_malitem_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
    MalgtkMalitem *self = MALGTK_MALITEM (object);
    MalgtkMalitemPrivate *priv = malgtk_malitem_get_instance_private (self);
    gchar **strv;
    gint64 i64;
    const gchar *str;
    MalgtkDate *mdate;
    GDate *date;
    GDateTime *dt;
    gdouble dbl;
    gboolean b;
    gint i;

    switch (property_id)
    {
        case PROP_SERIES_MALDB_ID:
            i64 = g_value_get_int64 (value);
            if (i64 != priv->mal_db_id) {
                priv->mal_db_id = i64;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_MALDB_ID]);
            }
            break;
        case PROP_SERIES_TITLE:
            str = g_value_get_string (value);
            if (0 != strcmp(priv->series_title->str, str)) {
                g_string_assign(priv->series_title, str);
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_TITLE]);
            }
            break;
        case PROP_SERIES_PREFERRED_TITLE:
            str = g_value_get_string (value);
            if (0 != strcmp(priv->preferred_title->str, str)) {
                g_string_assign(priv->preferred_title, str);
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_PREFERRED_TITLE]);
            }
            break;
        case PROP_SERIES_DATE_BEGIN:
            mdate = (MalgtkDate*)g_value_get_boxed(value);
            if (!malgtk_date_is_equal(mdate, &priv->series_begin)) {
                priv->series_begin = *mdate;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_DATE_BEGIN]);
                g_object_notify_by_pspec(object, obj_properties[PROP_SEASON_BEGIN]);
            }
            break;
        case PROP_SERIES_DATE_END:
            mdate = (MalgtkDate*)g_value_get_boxed(value);
            if (!malgtk_date_is_equal(mdate, &priv->series_end)) {
                priv->series_end = *mdate;
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_DATE_END]);
                g_object_notify_by_pspec(object, obj_properties[PROP_SEASON_END]);
            }
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
            str = g_value_get_string (value);
            if (0 != strcmp(priv->image_url->str, str)) {
                g_string_assign(priv->image_url, str);
                g_object_notify_by_pspec(object, obj_properties[PROP_IMAGE_URL]);
            }
            break;
        case PROP_SERIES_SYNONYM:
            strv = g_value_get_boxed(value);
            if (!is_strv_tree_equal(strv, priv->series_synonyms)) {
                _tree_remove_all(priv->series_synonyms);
                for (; NULL != *strv; ++strv) {
                    if (!g_tree_lookup_extended(priv->series_synonyms, *strv, NULL, NULL)) {
                        g_tree_insert(priv->series_synonyms,
                                      g_string_chunk_insert_const(priv->chunk, *strv), NULL);
                    }
                }
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_SYNONYM]);
            }
            break;
        case PROP_SERIES_SYNOPSIS:
            str = g_value_get_string (value);
            if (0 != strcmp(priv->synopsis->str, str)) {
                g_string_assign (priv->synopsis, str);
                g_object_notify_by_pspec(object, obj_properties[PROP_SERIES_SYNOPSIS]);
            }
            break;
        case PROP_TAGS:
            strv = g_value_get_boxed(value);
            if (!is_strv_tree_equal(strv, priv->tags)) {
                _tree_remove_all(priv->tags);
                for (; NULL != *strv; ++strv) {
                    if (!g_tree_lookup_extended(priv->tags, *strv, NULL, NULL)) {
                        g_tree_insert(priv->tags,
                                      g_string_chunk_insert_const(priv->chunk, *strv), NULL);
                    }
                }
                g_object_notify_by_pspec(object, obj_properties[PROP_TAGS]);
            }
            break;
        case PROP_DATE_START:
            date = (GDate*)g_value_get_boxed(value);
            if (g_date_valid(date) &&
                (!g_date_valid(&priv->date_start) ||
                 0 != g_date_compare(date, &priv->date_start)))
            {
                priv->date_start = *date;
                g_object_notify_by_pspec(object, obj_properties[PROP_DATE_START]);
            }
            break;
        case PROP_DATE_FINISH:
            date = (GDate*)g_value_get_boxed(value);
            if (g_date_valid(date) &&
                (!g_date_valid(&priv->date_finish) ||
                 0 != g_date_compare(date, &priv->date_finish)))
            {
                priv->date_finish = *date;
                g_object_notify_by_pspec(object, obj_properties[PROP_DATE_FINISH]);
            }
            break;
        case PROP_ID:
            i64 = g_value_get_int64 (value);
            if (i64 != priv->id) {
                priv->id = i64;
                g_object_notify_by_pspec(object, obj_properties[PROP_ID]);
            }
            break;
        case PROP_LAST_UPDATED:
            dt = g_value_get_boxed(value);
            if (0 != g_date_time_compare(dt, priv->last_updated)) {
                g_date_time_unref (priv->last_updated);
                priv->last_updated = g_date_time_ref(dt);
                g_object_notify_by_pspec(object, obj_properties[PROP_LAST_UPDATED]);
            }
            break;
        case PROP_SCORE:
            dbl = g_value_get_double (value);
            if (dbl != priv->score) {
                priv->score = dbl;
                g_object_notify_by_pspec(object, obj_properties[PROP_SCORE]);
            }
            break;
        case PROP_ENABLE_RECONSUMING:
            b = g_value_get_boolean (value);
            if (b != priv->enable_reconsuming) {
                priv->enable_reconsuming = b;
                g_object_notify_by_pspec(object, obj_properties[PROP_ENABLE_RECONSUMING]);
            }
            break;
        case PROP_FANSUB_GROUP:
            str = g_value_get_string (value);
            if (0 != strcmp(priv->fansub_group->str, str)) {
                g_string_assign (priv->fansub_group, str);
                g_object_notify_by_pspec(object, obj_properties[PROP_FANSUB_GROUP]);
            }
            break;
        case PROP_COMMENTS:
            str = g_value_get_string (value);
            if (0 != strcmp(priv->comments->str, str)) {
                g_string_assign (priv->comments, str);
                g_object_notify_by_pspec(object, obj_properties[PROP_COMMENTS]);
            }
            break;
        case PROP_DOWNLOADED_ITEMS:
            i = g_value_get_int (value);
            if (i != priv->downloaded_items) {
                priv->downloaded_items = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_DOWNLOADED_ITEMS]);
            }
            break;
        case PROP_TIMES_CONSUMED:
            i = g_value_get_int (value);
            if (i != priv->times_consumed) {
                priv->times_consumed = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_TIMES_CONSUMED]);
            }
            break;
        case PROP_RECONSUME_VALUE:
            i = g_value_get_enum (value);
            if (i != priv->reconsume_value) {
                priv->reconsume_value = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_RECONSUME_VALUE]);
            }
            break;
        case PROP_PRIORITY:
            i = g_value_get_enum (value);
            if (i != priv->priority) {
                priv->priority = i;
                g_object_notify_by_pspec(object, obj_properties[PROP_PRIORITY]);
            }
            break;
        case PROP_ENABLE_DISCUSSION:
            b = g_value_get_boolean (value);
            if (b != priv->enable_discussion) {
                priv->enable_discussion = b;
                g_object_notify_by_pspec(object, obj_properties[PROP_ENABLE_DISCUSSION]);
            }
            break;
        case PROP_HAS_DETAILS:
            b = g_value_get_boolean (value);
            if (b != priv->has_details) {
                priv->has_details = b;
                g_object_notify_by_pspec(object, obj_properties[PROP_HAS_DETAILS]);
            }
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
            if (g_date_valid (&priv->date_start))
                g_value_set_boxed (value, &priv->date_start);
            else
                g_value_set_boxed (value, NULL);
            break;
        case PROP_DATE_FINISH:
            if (g_date_valid (&priv->date_finish))
                g_value_set_boxed (value, &priv->date_finish);
            else
                g_value_set_boxed (value, NULL);
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
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_TITLE] =
        g_param_spec_string ("series-title",
                             "Series Title",
                             "Title of the series or franchise",
                             "no-series-title-set",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_PREFERRED_TITLE] =
        g_param_spec_string ("preferred-title",
                             "Preferred Title",
                             "Your defined override for series title",
                             "no-preferred-series-title-set",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);


    obj_properties[PROP_SERIES_DATE_BEGIN] =
        g_param_spec_boxed ("series-date-begin",
                            "Series Begin",
                            "Date the series begins",
                            MALGTK_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_DATE_END] =
        g_param_spec_boxed ("series-date-end",
                            "Series End",
                            "Date the series ends",
                            MALGTK_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SEASON_BEGIN] =
        g_param_spec_string ("season-begin",
                             "Season Start",
                             "Season for the beginning of the series",
                             "no-season-begin-set",
                             G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SEASON_END] =
        g_param_spec_string ("season-end",
                             "Season End",
                             "Season for the ending of the series",
                             "no-season-end-set",
                             G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_IMAGE_URL] =
        g_param_spec_string ("image-url",
                             "Image URL",
                             "URL for image representing the series",
                             "no-image-url-set",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_SYNONYM] =
        g_param_spec_boxed ("series-synonyms",
                            "Series Synonyms",
                            "Set of synonyms for the series title",
                            G_TYPE_STRV,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SERIES_SYNOPSIS] =
        g_param_spec_string ("series-synopsis",
                             "Synopsis",
                             "Synopsis of the series",
                             "no-series-synopsis-set",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_TAGS] =
        g_param_spec_boxed ("tags",
                            "Tags",
                            "Tags associated with the series",
                            G_TYPE_STRV,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_DATE_START] =
        g_param_spec_boxed ("date-start",
                            "Started",
                            "Date you started watching or reading the series",
                            G_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_DATE_FINISH] =
        g_param_spec_boxed ("date-finish",
                            "Date Finished",
                            "Date you finished watching or reading the series",
                            G_TYPE_DATE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_ID] =
        g_param_spec_int64 ("id",
                            "id",
                            "myanimelist.net db id for your data",
                            G_MININT64,
                            G_MAXINT64,
                            0,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_LAST_UPDATED] =
        g_param_spec_boxed ("last-updated",
                            "Last Updated",
                            "Last time data was updated on mal.net",
                            G_TYPE_DATE_TIME,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_SCORE] =
        g_param_spec_double ("score",
                             "Score",
                             "Score reflecting series quality",
                             0.0,
                             10.0,
                             0.0,
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_ENABLE_RECONSUMING] =
        g_param_spec_boolean ("enable-reconsuming",
                              "Reconsuming",
                              "Indicates you are rewatching or rereading the series",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_FANSUB_GROUP] =
        g_param_spec_string ("fansub-group",
                             "Fansub Group",
                             "Fansub Group you are using",
                             "no-fansub-group-set",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_COMMENTS] =
        g_param_spec_string ("comments",
                             "Comments",
                             "Your comments",
                             "no-comments-set",
                             G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_DOWNLOADED_ITEMS] =
        g_param_spec_int ("downloaded-items",
                          "Downloaded Items",
                          "Number of downloaded episodes or chapters",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_TIMES_CONSUMED] =
        g_param_spec_int ("times-consumed",
                          "Times Consumed",
                          "Number of times you've watched or read the series",
                          0,
                          G_MAXINT,
                          0,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_RECONSUME_VALUE] =
        g_param_spec_enum ("reconsume-value",
                           "Reconsumability",
                           "How rewatchable or rereadable the series is",
                           MALGTK_TYPE_MALITEM_RECONSUME_VALUE,
                           MALGTK_MALITEM_RECONSUME_VALUE_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_PRIORITY] =
        g_param_spec_enum ("priority",
                           "Priority",
                           "Priority for watching or reading this series",
                           MALGTK_TYPE_MALITEM_PRIORITY,
                           MALGTK_MALITEM_PRIORITY_INVALID,
                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_ENABLE_DISCUSSION] =
        g_param_spec_boolean ("enable-discussion",
                              "Enable Discussion",
                              "Whether the series can be discussed on your myanimelist.net page",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    obj_properties[PROP_HAS_DETAILS] =
        g_param_spec_boolean ("has-details",
                              "Has Details",
                              "Indicates mal-gtk has downloaded all possible details for this series",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

    g_object_class_install_properties (gobject_class,
                                       N_PROPERTIES,
                                       obj_properties);

}

static gint
_gcompare_str(gconstpointer a,
              gconstpointer b,
              gpointer user_data)
{
    (void) user_data;
    return g_utf8_collate(a, b);
}

static void
malgtk_malitem_init (MalgtkMalitem *self)
{
    MalgtkMalitemPrivate *priv = malgtk_malitem_get_instance_private (self);
    priv->series_title    = g_string_new("");
    priv->preferred_title = g_string_new("");
    malgtk_date_clear(&priv->series_begin);
    malgtk_date_clear(&priv->series_end);
    priv->image_url       = g_string_new("");
    priv->series_synonyms = g_tree_new_full(_gcompare_str, NULL, NULL, NULL);
    priv->synopsis        = g_string_new("");

    priv->tags            = g_tree_new_full(_gcompare_str, NULL, NULL, NULL);
    g_date_clear(&priv->date_start, 1);
    g_date_clear(&priv->date_finish, 1);
    priv->last_updated    = g_date_time_new_from_unix_utc (0);
    priv->fansub_group    = g_string_new("");
    priv->comments        = g_string_new("");
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
malgtk_malitem_add_synonym(MalgtkMalitem *item,
                           const gchar *synonym)
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
malgtk_malitem_add_tag(MalgtkMalitem *item,
                       const gchar *tag)
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
_foreach_cb(gpointer key,
            gpointer value,
            gpointer data)
{
    struct _foreach_data *bkt = data;
    return bkt->cb(key, bkt->user_data);
}

void
malgtk_malitem_foreach_synonym(const MalgtkMalitem *item,
                               MalgtkSetForeachFunc cb,
                               gpointer user_data)
{
    MalgtkMalitemPrivate *priv;
    struct _foreach_data data = {cb, user_data};
    g_return_if_fail(MALGTK_IS_MALITEM((MalgtkMalitem*)item));

    priv = malgtk_malitem_get_instance_private ((MalgtkMalitem*)item);
    g_tree_foreach (priv->series_synonyms, _foreach_cb, &data);
}

void
malgtk_malitem_foreach_tag(const MalgtkMalitem *item,
                           MalgtkSetForeachFunc cb,
                           gpointer user_data)
{
    MalgtkMalitemPrivate *priv;
    struct _foreach_data data = {cb, user_data};
    g_return_if_fail(MALGTK_IS_MALITEM((MalgtkMalitem*)item));
    priv = malgtk_malitem_get_instance_private ((MalgtkMalitem*)item);
    g_tree_foreach (priv->tags, _foreach_cb, &data);
}

static gboolean
_tree_to_gstrv_cb(gpointer key,
                  gpointer value,
                  gpointer data)
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

static gboolean
xform_reconsume_value_enum(GValue *value,
                           const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_MALITEM_RECONSUME_VALUE, in);
}

static gboolean
xform_priority_enum(GValue *value,
                    const xmlChar *in)
{
    return xform_enum(value, MALGTK_TYPE_MALITEM_PRIORITY, in);
}


static gboolean
xform_string(GValue *value,
             const xmlChar* in)
{
    g_value_init(value, G_TYPE_STRING);
    g_value_set_static_string(value, (const char*)in);
    return FALSE;
}

static const struct {
    const xmlChar *xml_name;
    gint prop_id;
    gboolean (*xform)(GValue *out, const xmlChar* in);
} field_map[] = {
    { BAD_CAST"MALitem",                -1,                          NULL},
    { BAD_CAST"series_itemdb_id",       PROP_SERIES_MALDB_ID,        xform_gint64},
    { BAD_CAST"series_title",           PROP_SERIES_TITLE,           xform_string},
    { BAD_CAST"series_preferred_title", PROP_SERIES_PREFERRED_TITLE, xform_string},
    { BAD_CAST"series_date_begin",      PROP_SERIES_DATE_BEGIN,      xform_string},
    { BAD_CAST"series_date_end",        PROP_SERIES_DATE_END,        xform_string},
    { BAD_CAST"image_url",              PROP_IMAGE_URL,              xform_string},
    { BAD_CAST"series_synonyms",        -1,                          xform_string},
    { BAD_CAST"series_synonym",         PROP_SERIES_SYNONYM,         xform_string},
    { BAD_CAST"series_synopsis",        PROP_SERIES_SYNOPSIS,        xform_string},
    { BAD_CAST"tags",                   -1,                          xform_string},
    { BAD_CAST"tag",                    PROP_TAGS,                   xform_string},
    { BAD_CAST"date_finish",            PROP_DATE_FINISH,            xform_gdate},
    { BAD_CAST"date_start",             PROP_DATE_START,             xform_gdate},
    { BAD_CAST"id",                     PROP_ID,                     xform_gint64},
    { BAD_CAST"last_updated",           PROP_LAST_UPDATED,           xform_datetime},
    { BAD_CAST"score",                  PROP_SCORE,                  xform_gdouble},
    { BAD_CAST"enable_reconsuming",     PROP_ENABLE_RECONSUMING,     xform_gint64},
    { BAD_CAST"fansub_group",           PROP_FANSUB_GROUP,           xform_string},
    { BAD_CAST"comments",               PROP_COMMENTS,               xform_string},
    { BAD_CAST"downloaded_items",       PROP_DOWNLOADED_ITEMS,       xform_gint64},
    { BAD_CAST"times_consumed",         PROP_TIMES_CONSUMED,         xform_gint64},
    { BAD_CAST"reconsume_value",        PROP_RECONSUME_VALUE,        xform_reconsume_value_enum},
    { BAD_CAST"priority",               PROP_PRIORITY,               xform_priority_enum},
    { BAD_CAST"enable_discussion",      PROP_ENABLE_DISCUSSION,      xform_gint64},
    { BAD_CAST"has_details",            PROP_HAS_DETAILS,            xform_gint64},
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
malgtk_malitem_set_from_xml(MalgtkMalitem *malitem,
                            xmlTextReaderPtr reader)
{
    gint offset = -1;
    GValue value = G_VALUE_INIT;
    gboolean is_default;
    g_return_if_fail(MALGTK_IS_MALITEM((MalgtkMalitem*)malitem));

    g_object_freeze_notify (G_OBJECT (malitem));

    for (; !(xmlStrEqual(BAD_CAST"MALitem", xmlTextReaderConstName(reader)) &&
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
                break;
            case XML_READER_TYPE_TEXT:
                switch (field_map[offset].prop_id) {
                    case -1:
                        break;
                    case PROP_TAGS:
                        malgtk_malitem_add_tag(malitem, (const char*)xmlTextReaderConstValue(reader));
                        break;
                    case PROP_SERIES_SYNONYM:
                        malgtk_malitem_add_synonym(malitem, (const char*)xmlTextReaderConstValue(reader));
                        break;
                    default:
                        is_default = field_map[offset].xform(&value,
                                                             xmlTextReaderConstValue(reader));
                        if (!is_default) {
                            g_object_set_property(G_OBJECT(malitem),
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

    g_object_thaw_notify (G_OBJECT (malitem));
}

struct key_writer_pair
{
    const char *element;
    xmlTextWriterPtr writer;
};

static gboolean
_gtree_to_writer_cb(gpointer key,
                    gpointer value,
                    gpointer data)
{
    struct key_writer_pair *pair = (struct key_writer_pair*)data;
    xmlTextWriterWriteElement(pair->writer, BAD_CAST pair->element, BAD_CAST key);
    return FALSE;
}

static void
_write_g_tree(xmlTextWriterPtr writer,
              const char *head_element,
              const char *child_element,
              GTree *tree)
{
    struct key_writer_pair pair = { child_element, writer };

    xmlTextWriterStartElement(writer, BAD_CAST head_element);
    g_tree_foreach (tree, _gtree_to_writer_cb, &pair);
    xmlTextWriterEndElement(writer);
}

static void
_write_g_date(xmlTextWriterPtr writer,
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
_write_maldate(xmlTextWriterPtr writer,
               const char *element,
               const MalgtkDate *date)
{
    g_autofree char *str = malgtk_date_get_string(date);
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST str);
}

static void
_write_enum_nick(xmlTextWriterPtr writer,
                 const char *element,
                 GType enum_type,
                 gint value)
{
    GEnumClass *enum_class = g_type_class_ref(enum_type);
    GEnumValue *enum_value = g_enum_get_value(enum_class, value);
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST enum_value->value_nick);
    g_type_class_unref(enum_class);
}

static void
_write_g_string(xmlTextWriterPtr writer,
                const char *element,
                GString *str)
{
    xmlTextWriterWriteElement(writer, BAD_CAST element, BAD_CAST str->str);
}

void
malgtk_malitem_get_xml(const MalgtkMalitem *malitem,
                       xmlTextWriterPtr writer)
{
    MalgtkMalitemPrivate *priv;
    g_return_if_fail (MALGTK_IS_MALITEM(malitem));
    priv = malgtk_malitem_get_instance_private (malitem);
    xmlTextWriterStartElement(writer, BAD_CAST"MALitem");
    xmlTextWriterWriteAttribute(writer, BAD_CAST"version", BAD_CAST"1");

    xmlTextWriterWriteFormatElement(writer, BAD_CAST"series_itemdb_id", "%" G_GINT64_FORMAT,
                                    priv->mal_db_id);
    _write_g_string (writer, "series_title",                       priv->series_title);
    _write_g_string (writer, "series_preferred_title",             priv->preferred_title);
    _write_maldate  (writer, "series_date_begin",                 &priv->series_begin);
    _write_maldate  (writer, "series_date_end",                   &priv->series_end);
    _write_g_string (writer, "image_url",                          priv->image_url);
    _write_g_tree   (writer, "series_synonyms", "series_synonym",  priv->series_synonyms);
    _write_g_string (writer, "series_synopsis",                    priv->synopsis);
    _write_g_tree   (writer, "tags", "tag",                        priv->tags);
    _write_g_date   (writer, "date_start",                        &priv->date_start);
    _write_g_date   (writer, "date_finish",                       &priv->date_finish);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"id",           "%" G_GINT64_FORMAT,
                                    priv->id);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"last_updated", "%" G_GINT64_FORMAT,
                                    g_date_time_to_unix(priv->last_updated));
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"score",              "%f", priv->score);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"enable_reconsuming", "%d", priv->enable_reconsuming);
    _write_g_string (writer, "fansub_group", priv->fansub_group);
    _write_g_string (writer, "comments",     priv->comments);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"downloaded_items",   "%d", priv->downloaded_items);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"times_consumed",     "%d", priv->times_consumed);
    _write_enum_nick(writer, "reconsume_value", MALGTK_TYPE_MALITEM_RECONSUME_VALUE, priv->reconsume_value);
    _write_enum_nick(writer, "priority",        MALGTK_TYPE_MALITEM_PRIORITY,        priv->priority);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"enable_discussion",  "%d", priv->enable_discussion);
    xmlTextWriterWriteFormatElement(writer, BAD_CAST"has_details",        "%d", priv->has_details);

    xmlTextWriterEndElement(writer); /* MALitem */
}
