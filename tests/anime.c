#include <glib.h>
#include <locale.h>
#include <stdio.h>
#include "malgtk_anime.h"


static void
test_anime_getset(void)
{
    g_autoptr(MalgtkAnime) anime = malgtk_anime_new();
    MalgtkAnimeSeriesType series_type = MALGTK_ANIME_SERIES_TYPE_ONA;
    MalgtkAnimeSeriesStatus series_status = MALGTK_ANIME_SERIES_STATUS_FINISHED;
    gint series_episodes = g_test_rand_int_range(1, G_MAXINT);
    MalgtkAnimeStatus status = MALGTK_ANIME_STATUS_ON_HOLD;
    gint episodes = g_test_rand_int_range(0, series_episodes);
    gint rewatch_episode = g_test_rand_int_range(0, series_episodes);
    MalgtkAnimeStorageType storage_type = MALGTK_ANIME_STORAGE_TYPE_DVDCD;
    gdouble storage_value = g_test_rand_double_range(0.0, G_MAXDOUBLE);

    g_object_set (G_OBJECT(anime),
                  "series-type", series_type,
                  "series-status", series_status,
                  "series-episodes", series_episodes,
                  "status", status,
                  "episodes", episodes,
                  "rewatch-episode", rewatch_episode,
                  "storage-type", storage_type,
                  "storage-value", storage_value,
                  NULL);

    MalgtkAnimeSeriesType _series_type;
    MalgtkAnimeSeriesStatus _series_status;
    gint _series_episodes;
    MalgtkAnimeStatus _status;
    gint _episodes;
    gint _rewatch_episode;
    MalgtkAnimeStorageType _storage_type;
    gdouble _storage_value;

    g_object_get (G_OBJECT(anime),
                  "series-type", &_series_type,
                  "series-status", &_series_status,
                  "series-episodes", &_series_episodes,
                  "status", &_status,
                  "episodes", &_episodes,
                  "rewatch-episode", &_rewatch_episode,
                  "storage-type", &_storage_type,
                  "storage-value", &_storage_value,
                  NULL);

    g_assert_cmpint (series_type, ==,  _series_type);
    g_assert_cmpint (series_status, ==,  _series_status);
    g_assert_cmpint (series_episodes, ==,  _series_episodes);
    g_assert_cmpint (status, ==,  _status);
    g_assert_cmpint (episodes, ==,  _episodes);
    g_assert_cmpint (rewatch_episode, ==,  _rewatch_episode);
    g_assert_cmpint (storage_type, ==,  _storage_type);
    g_assert_cmpint (storage_value, ==,  _storage_value);
}

int
main(int argc, char *argv[])
{
    setlocale (LC_ALL, "");
    g_test_init (&argc, &argv, NULL);
    g_test_add_func("/malgtk-anime/test_getset", test_anime_getset);
    
    int res = g_test_run ();

    return res;
}
