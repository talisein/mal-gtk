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
#include "malgtk_manga.h"

static void
test_manga_getset(void)
{
    g_autoptr(MalgtkManga)  manga             = malgtk_manga_new();
    MalgtkMangaSeriesType   series_type       = MALGTK_MANGA_SERIES_TYPE_DOUJIN;
    MalgtkMangaSeriesStatus series_status     = MALGTK_MANGA_SERIES_STATUS_NOT_YET_PUBLISHED;
    MalgtkMangaStatus       status            = MALGTK_MANGA_STATUS_ON_HOLD;
    gint                    series_chapters   = 13;
    gint                    series_volumes    = 5;
    gint                    rereading_chapter = 11;
    gint                    chapters          = 7;
    gint                    volumes           = 3;
    gint                    retail_volumes    = 5;
    MalgtkMangaStorageType  storage_type      = MALGTK_MANGA_STORAGE_TYPE_HARD_DRIVE;
    gchar                  *series_title      = "derp";

    g_object_set (G_OBJECT(manga),
                  "series-type",       series_type,
                  "series-status",     series_status,
                  "series-chapters",   series_chapters,
                  "series-volumes",    series_volumes,
                  "status",            status,
                  "chapters",          chapters,
                  "volumes",           volumes,
                  "rereading-chapter", rereading_chapter,
                  "retail-volumes",    retail_volumes,
                  "storage-type",      storage_type,
                  "series-title",      series_title,
                  NULL);

    MalgtkMangaSeriesType   _series_type;
    MalgtkMangaSeriesStatus _series_status;
    MalgtkMangaStatus       _status;
    gint                    _series_chapters;
    gint                    _series_volumes;
    gint                    _rereading_chapter;
    gint                    _chapters;
    gint                    _volumes;
    gint                    _retail_volumes;
    MalgtkMangaStorageType  _storage_type;
    g_autofree gchar       *_series_title = NULL;

    g_object_get (G_OBJECT(manga),
                  "series-type",       &_series_type,
                  "series-status",     &_series_status,
                  "series-chapters",   &_series_chapters,
                  "series-volumes",    &_series_volumes,
                  "status",            &_status,
                  "chapters",          &_chapters,
                  "volumes",           &_volumes,
                  "rereading-chapter", &_rereading_chapter,
                  "retail-volumes",    &_retail_volumes,
                  "storage-type",      &_storage_type,
                  "series-title",      &_series_title,
                  NULL);

    g_assert_cmpint (series_type,       ==, _series_type);
    g_assert_cmpint (series_status,     ==, _series_status);
    g_assert_cmpint (series_chapters,   ==, _series_chapters);
    g_assert_cmpint (series_volumes,    ==, _series_volumes);
    g_assert_cmpint (status,            ==, _status);
    g_assert_cmpint (chapters,          ==, _chapters);
    g_assert_cmpint (volumes,           ==, _volumes);
    g_assert_cmpint (rereading_chapter, ==, _rereading_chapter);
    g_assert_cmpint (retail_volumes,    ==, _retail_volumes);
    g_assert_cmpint (storage_type,      ==, _storage_type);
    g_assert_cmpstr (series_title,      ==, _series_title);
}

static void
test_manga_xml(void)
{
    static const char xml[] = "<manga version=\"1\"><MALitem version=\"1\"><series_itemdb_id>2</series_itemdb_id><series_title>Berserk</series_title><series_preferred_title></series_preferred_title><series_date_begin>1989-08-25</series_date_begin><series_date_end>0000-00-00</series_date_end><image_url>http://cdn.myanimelist.net/images/manga/3/26544.jpg</image_url><series_synonyms><series_synonym>Berserk</series_synonym></series_synonyms><series_synopsis>Guts, known as the Black Swordsman, seeks sanctuary from the demonic forces that pursue him and his woman, and also vengeance against the man who branded him as an unholy sacrifice. Aided only by his titanic strength, skill, and sword, Guts must struggle against his bleak destiny, all the while fighting with a rage that might strip him of his humanity. Berserk is a dark and brooding story of outrageous swordplay and ominous fate, in the theme of Shakespeare's Macbeth.\
\
[b]Included one-shot:[/b]\
\
Volume 14: Berserk: The Prototype</series_synopsis><tags/><date_start>0000-00-00</date_start><date_finish>0000-00-00</date_finish><id>13382867</id><last_updated>1367619562</last_updated><score>9.000000</score><enable_reconsuming>0</enable_reconsuming><fansub_group></fansub_group><comments></comments><downloaded_items>0</downloaded_items><times_consumed>0</times_consumed><reconsume_value>Invalid Reconsume Value</reconsume_value><priority>Invalid Priority</priority><enable_discussion>0</enable_discussion><has_details>1</has_details></MALitem><series_type>Manga</series_type><series_status>Publishing</series_status><series_chapters>99</series_chapters><series_volumes>88</series_volumes><status>Reading</status><chapters>77</chapters><volumes>66</volumes><rereading_chapter>55</rereading_chapter><retail_volumes>44</retail_volumes><storage_type>Harddrive</storage_type></manga>";
    g_autoptr(MalgtkManga) manga  = malgtk_manga_new();
    xmlTextReaderPtr       reader = xmlReaderForMemory(xml, G_N_ELEMENTS(xml) - 1, NULL, NULL, 0);
    xmlTextReaderRead(reader);

    gchar                  *series_title      = "Berserk";
    MalgtkMangaSeriesType   series_type       = MALGTK_MANGA_SERIES_TYPE_MANGA;
    MalgtkMangaSeriesStatus series_status     = MALGTK_MANGA_SERIES_STATUS_PUBLISHING;
    gint                    series_chapters   = 99;
    gint                    series_volumes    = 88;
    MalgtkMangaStatus       status            = MALGTK_MANGA_STATUS_READING;
    gint                    chapters          = 77;
    gint                    volumes           = 66;
    gint                    rereading_chapter = 55;
    gint                    retail_volumes    = 44;
    MalgtkMangaStorageType  storage_type      = MALGTK_MANGA_STORAGE_TYPE_HARD_DRIVE;

    malgtk_manga_set_from_xml(manga, reader);

    g_autofree gchar       *_series_title = NULL;
    MalgtkMangaSeriesType   _series_type;
    MalgtkMangaSeriesStatus _series_status;
    gint                    _series_chapters;
    gint                    _series_volumes;
    MalgtkMangaStatus       _status;
    gint                    _chapters;
    gint                    _volumes;
    gint                    _rereading_chapter;
    gint                    _retail_volumes;
    MalgtkMangaStorageType  _storage_type;

    g_object_get(G_OBJECT(manga),
                 "series_title",      &_series_title,
                 "series_type",       &_series_type,
                 "series_status",     &_series_status,
                 "series_chapters",   &_series_chapters,
                 "series_volumes",    &_series_volumes,
                 "status",            &_status,
                 "chapters",          &_chapters,
                 "volumes",           &_volumes,
                 "rereading_chapter", &_rereading_chapter,
                 "retail_volumes",    &_retail_volumes,
                 "storage_type",      &_storage_type,
                 NULL);

    g_assert_cmpstr(series_title,      ==, _series_title);
    g_assert_cmpint(series_type,       ==, _series_type);
    g_assert_cmpint(series_status,     ==, _series_status);
    g_assert_cmpint(series_chapters,   ==, _series_chapters);
    g_assert_cmpint(series_volumes,    ==, _series_volumes);
    g_assert_cmpint(status,            ==, _status);
    g_assert_cmpint(chapters,          ==, _chapters);
    g_assert_cmpint(volumes,           ==, _volumes);
    g_assert_cmpint(rereading_chapter, ==, _rereading_chapter);
    g_assert_cmpint(retail_volumes,    ==, _retail_volumes);
    g_assert_cmpint(storage_type,      ==, _storage_type);

    xmlFreeTextReader(reader);
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
test_manga_notify(void)
{
    struct notify_ctx ctx;
    g_autoptr(MalgtkManga) manga  = malgtk_manga_new();

    g_signal_connect(G_OBJECT(manga), "notify",
                     G_CALLBACK (notify_counter),
                     &ctx);

#define  TEST_NOTIFY(prop, val, val2)  do {                 \
        ctx.name = prop;                                    \
        ctx.cnt = 0;                                        \
        g_object_set(G_OBJECT(manga), prop, val, NULL);     \
        g_assert_cmpint(ctx.cnt, ==, 1);                    \
        g_object_set(G_OBJECT(manga), prop, val, NULL);     \
        g_assert_cmpint(ctx.cnt, ==, 1);                    \
        g_object_set(G_OBJECT(manga), prop, val2, NULL);    \
        g_assert_cmpint(ctx.cnt, ==, 2); } while (0);

    TEST_NOTIFY("series-type", MALGTK_MANGA_SERIES_TYPE_MANGA, MALGTK_MANGA_SERIES_TYPE_DOUJIN);
    TEST_NOTIFY("series-status", MALGTK_MANGA_SERIES_STATUS_PUBLISHING, MALGTK_MANGA_SERIES_STATUS_NOT_YET_PUBLISHED);
    TEST_NOTIFY("series-chapters", 4, 12);
    TEST_NOTIFY("series-volumes", 4, 12);
    TEST_NOTIFY("status", MALGTK_MANGA_STATUS_ON_HOLD, MALGTK_MANGA_STATUS_READING);
    TEST_NOTIFY("chapters", 3, 23);
    TEST_NOTIFY("volumes", 4, 24);
    TEST_NOTIFY("rereading-chapter", 3, 24);
    TEST_NOTIFY("retail-volumes", 2, 22);
    TEST_NOTIFY("storage-type", MALGTK_MANGA_STORAGE_TYPE_HARD_DRIVE, MALGTK_MANGA_STORAGE_TYPE_INVALID);
}

int
main(int argc, char *argv[])
{
    setlocale (LC_ALL, "");
    g_test_init (&argc, &argv, NULL);
    g_test_add_func("/malgtk/manga/getset", test_manga_getset);
    g_test_add_func("/malgtk/manga/xml", test_manga_xml);
    g_test_add_func("/malgtk/manga/notify", test_manga_notify);

    int res = g_test_run ();

    return res;
}
