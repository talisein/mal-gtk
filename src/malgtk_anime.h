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

#pragma once
#include <glib-object.h>
#include "malgtk_malitem.h"

G_BEGIN_DECLS

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_STATUS_INVALID       = -1, /*< nick=Unknown >*/
    MALGTK_ANIME_STATUS_NONE          = 0,  /*< skip >*/
    MALGTK_ANIME_STATUS_WATCHING      = 1,  /*< nick=Watching >*/
    MALGTK_ANIME_STATUS_COMPLETED     = 2,  /*< nick=Completed >*/
    MALGTK_ANIME_STATUS_ON_HOLD       = 3,  /*< nick=On Hold >*/
    MALGTK_ANIME_STATUS_DROPPED       = 4,  /*< nick=Dropped >*/
    MALGTK_ANIME_STATUS_PLAN_TO_WATCH = 6,  /*< nick=Plan To Watch >*/
} MalgtkAnimeStatus;

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_SERIES_STATUS_INVALID     = -1, /*< nick=Unknown >*/
    MALGTK_ANIME_SERIES_STATUS_AIRING      = 1,  /*< nick=Airing >*/
    MALGTK_ANIME_SERIES_STATUS_FINISHED    = 2,  /*< nick=Finished >*/
    MALGTK_ANIME_SERIES_STATUS_NOTYETAIRED = 3,  /*< nick=Not Yet Aired >*/
} MalgtkAnimeSeriesStatus;

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_SERIES_TYPE_INVALID = -1, /*< nick=Unknown >*/
    MALGTK_ANIME_SERIES_TYPE_TV      = 1,  /*< nick=TV >*/
    MALGTK_ANIME_SERIES_TYPE_OVA     = 2,  /*< nick=OVA >*/
    MALGTK_ANIME_SERIES_TYPE_MOVIE   = 3,  /*< nick=Movie >*/
    MALGTK_ANIME_SERIES_TYPE_SPECIAL = 4,  /*< nick=Special >*/
    MALGTK_ANIME_SERIES_TYPE_ONA     = 5,  /*< nick=ONA >*/
    MALGTK_ANIME_SERIES_TYPE_MUSIC   = 6,  /*< nick=Music >*/
} MalgtkAnimeSeriesType;

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_STORAGE_TYPE_INVALID     = 0, /*< nick=Invalid Anime Storage >*/
    MALGTK_ANIME_STORAGE_TYPE_HARD_DRIVE  = 1, /*< nick=Hard Drive >*/
    MALGTK_ANIME_STORAGE_TYPE_DVDCD       = 2, /*< nick=DVD/CD >*/
    MALGTK_ANIME_STORAGE_TYPE_NONE        = 3, /*< nick=None >*/
    MALGTK_ANIME_STORAGE_TYPE_RETAIL_DVD  = 4, /*< nick=Retail DVD >*/
    MALGTK_ANIME_STORAGE_TYPE_VHS         = 5, /*< nick=VHS >*/
    MALGTK_ANIME_STORAGE_TYPE_EXTERNAL_HD = 6, /*< nick=External HD >*/
    MALGTK_ANIME_STORAGE_TYPE_NAS         = 7, /*< nick=NAS >*/
} MalgtkAnimeStorageType;

#define MALGTK_TYPE_ANIME             (malgtk_anime_get_type ())
G_DECLARE_FINAL_TYPE(MalgtkAnime, malgtk_anime, MALGTK, ANIME, MalgtkMalitem)

MalgtkAnime     *malgtk_anime_new (void);
void             malgtk_anime_set_from_xml(MalgtkAnime *anime, xmlTextReaderPtr reader);

G_END_DECLS
