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
#include "malgtk_manga.h"

static void
test_manga_getset(void)
{
    g_autoptr(MalgtkManga) manga = malgtk_manga_new();
    MalgtkMangaSeriesType series_type = MALGTK_MANGA_SERIES_TYPE_DOUJIN;
    MalgtkMangaSeriesStatus series_status = MALGTK_MANGA_SERIES_STATUS_NOT_YET_PUBLISHED;
    MalgtkMangaStatus status = MALGTK_MANGA_STATUS_ON_HOLD;
    gint series_chapters = 13;
    gint series_volumes = 5;
    gint rereading_chapter = 11;
    gint chapters = 7;
    gint volumes = 3;
    gint retail_volumes = 5;
    MalgtkMangaStorageType storage_type = MALGTK_MANGA_STORAGE_TYPE_HARD_DRIVE;
    gchar *series_title = "derp";
    
    g_object_set (G_OBJECT(manga),
                  "series-type", series_type,
                  "series-status", series_status,
                  "series-chapters", series_chapters,
                  "series-volumes", series_volumes,
                  "status", status,
                  "chapters", chapters,
                  "volumes", volumes,
                  "rereading-chapter", rereading_chapter,
                  "retail-volumes", retail_volumes,
                  "storage-type", storage_type,
                  "series-title", series_title,
                  NULL);

    MalgtkMangaSeriesType _series_type;
    MalgtkMangaSeriesStatus _series_status;
    MalgtkMangaStatus _status;
    gint _series_chapters;
    gint _series_volumes;
    gint _rereading_chapter;
    gint _chapters;
    gint _volumes;
    gint _retail_volumes;
    MalgtkMangaStorageType _storage_type;
    gchar *_series_title;

    g_object_get (G_OBJECT(manga),
                  "series-type", &_series_type,
                  "series-status", &_series_status,
                  "series-chapters", &_series_chapters,
                  "series-volumes", &_series_volumes,
                  "status", &_status,
                  "chapters", &_chapters,
                  "volumes", &_volumes,
                  "rereading-chapter", &_rereading_chapter,
                  "retail-volumes", &_retail_volumes,
                  "storage-type", &_storage_type,
                  "series-title", &_series_title,
                  NULL);

    g_assert_cmpint (series_type, ==, _series_type);
    g_assert_cmpint (series_status, ==, _series_status);
    g_assert_cmpint (series_chapters, ==, _series_chapters);
    g_assert_cmpint (series_volumes, ==, _series_volumes);
    g_assert_cmpint (status, ==, _status);
    g_assert_cmpint (chapters, ==, _chapters);
    g_assert_cmpint (volumes, ==, _volumes);
    g_assert_cmpint (rereading_chapter, ==, _rereading_chapter);
    g_assert_cmpint (retail_volumes, ==, _retail_volumes);
    g_assert_cmpint (storage_type, ==, _storage_type);
    g_assert_cmpstr (series_title, ==, _series_title);
}

int
main(int argc, char *argv[])
{
    setlocale (LC_ALL, "");
    g_test_init (&argc, &argv, NULL);
    g_test_add_func("/malgtk-manga/test_getset", test_manga_getset);
    
    int res = g_test_run ();

    return res;
}
