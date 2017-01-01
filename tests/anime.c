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

#include <glib.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include "malgtk_anime.h"


static void
test_anime_getset(void)
{
    g_autoptr(MalgtkAnime)  anime           = malgtk_anime_new();
    MalgtkAnimeSeriesType   series_type     = MALGTK_ANIME_SERIES_TYPE_ONA;
    MalgtkAnimeSeriesStatus series_status   = MALGTK_ANIME_SERIES_STATUS_FINISHED;
    gint                    series_episodes = g_test_rand_int_range(1, G_MAXINT);
    MalgtkAnimeStatus       status          = MALGTK_ANIME_STATUS_ON_HOLD;
    gint                    episodes        = g_test_rand_int_range(0, series_episodes);
    gint                    rewatch_episode = g_test_rand_int_range(0, series_episodes);
    MalgtkAnimeStorageType  storage_type    = MALGTK_ANIME_STORAGE_TYPE_DVDCD;
    gdouble                 storage_value   = g_test_rand_double_range(0.0, G_MAXDOUBLE);

    g_object_set (G_OBJECT(anime),
                  "series-type",     series_type,
                  "series-status",   series_status,
                  "series-episodes", series_episodes,
                  "status",          status,
                  "episodes",        episodes,
                  "rewatch-episode", rewatch_episode,
                  "storage-type",    storage_type,
                  "storage-value",   storage_value,
                  NULL);

    MalgtkAnimeSeriesType   _series_type;
    MalgtkAnimeSeriesStatus _series_status;
    gint                    _series_episodes;
    MalgtkAnimeStatus       _status;
    gint                    _episodes;
    gint                    _rewatch_episode;
    MalgtkAnimeStorageType  _storage_type;
    gdouble                 _storage_value;

    g_object_get (G_OBJECT(anime),
                  "series-type",     &_series_type,
                  "series-status",   &_series_status,
                  "series-episodes", &_series_episodes,
                  "status",          &_status,
                  "episodes",        &_episodes,
                  "rewatch-episode", &_rewatch_episode,
                  "storage-type",    &_storage_type,
                  "storage-value",   &_storage_value,
                  NULL);

    g_assert_cmpint (series_type,     ==, _series_type);
    g_assert_cmpint (series_status,   ==, _series_status);
    g_assert_cmpint (series_episodes, ==, _series_episodes);
    g_assert_cmpint (status,          ==, _status);
    g_assert_cmpint (episodes,        ==, _episodes);
    g_assert_cmpint (rewatch_episode, ==, _rewatch_episode);
    g_assert_cmpint (storage_type,    ==, _storage_type);
    g_assert_cmpint (storage_value,   ==, _storage_value);
}

static void
test_anime_xmlset (void)
{
    static const char xml[] = "<anime version=\"1\"><MALitem version=\"1\"><series_itemdb_id>1</series_itemdb_id><series_title>Cowboy Bebop</series_title><series_preferred_title></series_preferred_title><series_date_begin>1998-04-03</series_date_begin><series_date_end>1999-04-24</series_date_end><image_url>http://cdn.myanimelist.net/images/anime/4/19644.jpg</image_url><series_synonyms><series_synonym>Cowboy Bebop</series_synonym></series_synonyms><series_synopsis></series_synopsis><tags/><date_start>0000-00-00</date_start><date_finish>0000-00-00</date_finish><id>9755128</id><last_updated>1238650198</last_updated><score>0.000000</score><enable_reconsuming>0</enable_reconsuming><fansub_group></fansub_group><comments></comments><downloaded_items>0</downloaded_items><times_consumed>0</times_consumed><reconsume_value>Invalid Reconsume Value</reconsume_value><priority>Invalid Priority</priority><enable_discussion>0</enable_discussion><has_details>0</has_details></MALitem><series_type>TV</series_type><series_status>Finished</series_status><series_episodes>26</series_episodes><status>Completed</status><episodes>26</episodes><rewatch_episode>0</rewatch_episode><storage_type>Invalid Anime Storage</storage_type><storage_value>0.000000</storage_value></anime>";
    g_autoptr(MalgtkAnime) anime  = malgtk_anime_new();
    xmlTextReaderPtr       reader = xmlReaderForMemory(xml, G_N_ELEMENTS(xml) - 1, NULL, NULL, 0);
    xmlTextReaderRead(reader);

    gchar                  *series_title    = "Cowboy Bebop";
    MalgtkAnimeSeriesType   series_type     = MALGTK_ANIME_SERIES_TYPE_TV;
    MalgtkAnimeSeriesStatus series_status   = MALGTK_ANIME_SERIES_STATUS_FINISHED;
    gint                    series_episodes = 26;
    MalgtkAnimeStatus       status          = MALGTK_ANIME_STATUS_COMPLETED;
    gint                    episodes        = 26;
    gint                    rewatch_episode = 0;
    MalgtkAnimeStorageType  storage_type    = MALGTK_ANIME_STORAGE_TYPE_INVALID;
    gdouble                 storage_value   = 0.0;

    malgtk_anime_set_from_xml(anime, reader);

    g_autofree gchar       *_series_title = NULL;
    MalgtkAnimeSeriesType   _series_type;
    MalgtkAnimeSeriesStatus _series_status;
    gint                    _series_episodes;
    MalgtkAnimeStatus       _status;
    gint                    _episodes;
    gint                    _rewatch_episode;
    MalgtkAnimeStorageType  _storage_type;
    gdouble                 _storage_value;

    g_object_get(G_OBJECT(anime),
                 "series_title",    &_series_title,
                 "series-type",     &_series_type,
                 "series-status",   &_series_status,
                 "series-episodes", &_series_episodes,
                 "status",          &_status,
                 "episodes",        &_episodes,
                 "rewatch-episode", &_rewatch_episode,
                 "storage-type",    &_storage_type,
                 "storage-value",   &_storage_value,
                 NULL);

    g_assert_cmpstr  (series_title,    ==, _series_title);
    g_assert_cmpint  (series_type,     ==, _series_type);
    g_assert_cmpint  (series_status,   ==, _series_status);
    g_assert_cmpint  (series_episodes, ==, _series_episodes);
    g_assert_cmpint  (status,          ==, _status);
    g_assert_cmpint  (episodes,        ==, _episodes);
    g_assert_cmpint  (rewatch_episode, ==, _rewatch_episode);
    g_assert_cmpint  (storage_type,    ==, _storage_type);
    g_assert_cmpfloat(storage_value,   ==, _storage_value);

    xmlFreeTextReader(reader);
}

static void
test_anime_xmlget (void)
{
    static const char xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<anime version=\"1\"><MALitem version=\"1\"><series_itemdb_id>71</series_itemdb_id><series_title>Full Metal Panic!</series_title><series_preferred_title></series_preferred_title><series_date_begin>2002-01-08</series_date_begin><series_date_end>2002-06-18</series_date_end><image_url>https://myanimelist.cdn-dena.com/images/anime/2/75259.jpg</image_url><series_synonyms><series_synonym>FMP</series_synonym><series_synonym>Full Metal Panic!</series_synonym></series_synonyms><series_synopsis></series_synopsis><tags/><date_start>0000-00-00</date_start><date_finish>0000-00-00</date_finish><id>9755223</id><last_updated>1238650530</last_updated><score>0.000000</score><enable_reconsuming>0</enable_reconsuming><fansub_group></fansub_group><comments></comments><downloaded_items>0</downloaded_items><times_consumed>0</times_consumed><reconsume_value>Invalid Reconsume Value</reconsume_value><priority>Invalid Priority</priority><enable_discussion>0</enable_discussion><has_details>1</has_details></MALitem><series_type>TV</series_type><series_status>Finished</series_status><series_episodes>24</series_episodes><status>Completed</status><episodes>24</episodes><rewatch_episode>0</rewatch_episode><storage_type>Invalid Anime Storage</storage_type><storage_value>2.000000</storage_value></anime>\n";

    g_autoptr(MalgtkAnime) anime = malgtk_anime_new();
    xmlBufferPtr     buffer = xmlBufferCreate();;
    xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
    xmlTextReaderPtr reader = xmlReaderForMemory(xml, G_N_ELEMENTS(xml) - 1, NULL, NULL, 0);

    /* Set the state */
    xmlTextReaderRead(reader);
    malgtk_anime_set_from_xml(anime, reader);

    /* Now serialize the state */
    xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    malgtk_anime_get_xml(anime, writer);
    xmlTextWriterEndDocument(writer);

    /* Check idempotent */
    const char *content = (char*)xmlBufferContent(buffer);
    g_assert_cmpstr(xml, ==, content);

    /* Free */
    xmlFreeTextReader(reader);
    xmlFreeTextWriter(writer);
    xmlBufferFree(buffer);
}

struct notify_ctx
{
    gchar *name;
    gint64 cnt;
};

static void
notify_counter (GObject    *gobject,
                GParamSpec *pspec,
                gpointer    user_data)
{
    struct notify_ctx *ctx = user_data;
    if (strcmp(pspec->name, ctx->name) == 0) {
        ++ctx->cnt;
    }
}

static void
test_anime_notify(void)
{
    struct notify_ctx ctx;
    g_autoptr(MalgtkAnime)  anime = malgtk_anime_new();
    g_signal_connect(G_OBJECT(anime), "notify",
                     G_CALLBACK (notify_counter),
                     &ctx);

#define  TEST_NOTIFY(prop, val, val2)  do {                 \
        ctx.name = prop;                                    \
        ctx.cnt = 0;                                        \
        g_object_set(G_OBJECT(anime), prop, val, NULL);     \
        g_assert_cmpint(ctx.cnt, ==, 1);                    \
        g_object_set(G_OBJECT(anime), prop, val, NULL);     \
        g_assert_cmpint(ctx.cnt, ==, 1);                    \
        g_object_set(G_OBJECT(anime), prop, val2, NULL);    \
        g_assert_cmpint(ctx.cnt, ==, 2); } while (0);

    TEST_NOTIFY("series-type", MALGTK_ANIME_SERIES_TYPE_TV, MALGTK_ANIME_SERIES_TYPE_ONA);
    TEST_NOTIFY("series-status", MALGTK_ANIME_SERIES_STATUS_FINISHED, MALGTK_ANIME_SERIES_STATUS_INVALID);
    TEST_NOTIFY("series-episodes", 13, 26);
    TEST_NOTIFY("status", MALGTK_ANIME_STATUS_ON_HOLD, MALGTK_ANIME_STATUS_COMPLETED);
    TEST_NOTIFY("episodes", 8, 12);
    TEST_NOTIFY("rewatch-episode", 7, 9);
    TEST_NOTIFY("storage-type", MALGTK_ANIME_STORAGE_TYPE_DVDCD, MALGTK_ANIME_STORAGE_TYPE_INVALID);
    TEST_NOTIFY("storage-value", 12.2, 159.2);
}

int
main(int argc, char *argv[])
{
    setlocale (LC_ALL, "");
    g_test_init (&argc, &argv, NULL);
    g_test_add_func("/malgtk/anime/getset", test_anime_getset);
    g_test_add_func("/malgtk/anime/xmlset", test_anime_xmlset);
    g_test_add_func("/malgtk/anime/notify", test_anime_notify);
    g_test_add_func("/malgtk/anime/xmlget", test_anime_xmlget);


    int res = g_test_run ();

    return res;
}
