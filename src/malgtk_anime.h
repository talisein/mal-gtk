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

G_BEGIN_DECLS

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_STATUS_INVALID       = -1,
    MALGTK_ANIME_STATUS_NONE          = 0,
    MALGTK_ANIME_STATUS_WATCHING      = 1,
    MALGTK_ANIME_STATUS_COMPLETED     = 2,
    MALGTK_ANIME_STATUS_ON_HOLD       = 3,
    MALGTK_ANIME_STATUS_DROPPED       = 4,
    MALGTK_ANIME_STATUS_PLAN_TO_WATCH = 6,
} MalgtkAnimeStatus;

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_SERIES_STATUS_INVALID     = -1,
    MALGTK_ANIME_SERIES_STATUS_AIRING      = 1,
    MALGTK_ANIME_SERIES_STATUS_FINISHED    = 2,
    MALGTK_ANIME_SERIES_STATUS_NOTYETAIRED = 3,
} MalgtkAnimeSeriesStatus;

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_SERIES_TYPE_INVALID = -1,
    MALGTK_ANIME_SERIES_TYPE_TV      = 1, 
    MALGTK_ANIME_SERIES_TYPE_OVA     = 2,
    MALGTK_ANIME_SERIES_TYPE_MOVIE   = 3,
    MALGTK_ANIME_SERIES_TYPE_SPECIAL = 4,
    MALGTK_ANIME_SERIES_TYPE_ONA     = 5,
    MALGTK_ANIME_SERIES_TYPE_MUSIC   = 6
} MalgtkAnimeSeriesType;

typedef enum /*< prefix=MALGTK_ANIME >*/
{
    MALGTK_ANIME_STORAGE_TYPE_INVALID     = 0,
    MALGTK_ANIME_STORAGE_TYPE_HARD_DRIVE  = 1,
    MALGTK_ANIME_STORAGE_TYPE_DVDCD       = 2,
    MALGTK_ANIME_STORAGE_TYPE_NONE        = 3,
    MALGTK_ANIME_STORAGE_TYPE_RETAIL_DVD  = 4,
    MALGTK_ANIME_STORAGE_TYPE_VHS         = 5,
    MALGTK_ANIME_STORAGE_TYPE_EXTERNAL_HD = 6,
    MALGTK_ANIME_STORAGE_TYPE_NAS         = 7,
} MalgtkAnimeStorageType;

#define MALGTK_TYPE_ANIME             (malgtk_anime_get_type ())
G_DECLARE_DERIVABLE_TYPE(MalgtkAnime, malgtk_anime, MALGTK, ANIME, MalgtkMalitem)

struct _MalgtkAnimeClass
{
    MalgtkMalitemClass  parent_class;
};

MalgtkMalitem     *malgtk_malitem_new (void);

G_END_DECLS
