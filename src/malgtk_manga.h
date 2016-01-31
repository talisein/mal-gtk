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

typedef enum /*< prefix=MALGTK_MANGA >*/
{
    MALGTK_MANGA_STATUS_INVALID       = -1, /*< nick=Invalid Status >*/
    MALGTK_MANGA_STATUS_READING       = 1,  /*< nick=Reading >*/
    MALGTK_MANGA_STATUS_COMPLETED     = 2,  /*< nick=Completed >*/
    MALGTK_MANGA_STATUS_ON_HOLD       = 3,  /*< nick=On Hold >*/
    MALGTK_MANGA_STATUS_DROPPED       = 4,  /*< nick=Dropped >*/
    MALGTK_MANGA_STATUS_PLAN_TO_READ  = 6,  /*< nick=Plan To Read >*/
} MalgtkMangaStatus;

typedef enum /*< prefix=MALGTK_MANGA >*/
{
    MALGTK_MANGA_SERIES_STATUS_INVALID           = -1, /*< nick=Invalid Status >*/
    MALGTK_MANGA_SERIES_STATUS_PUBLISHING        = 1, /*< nick=Publishing >*/
    MALGTK_MANGA_SERIES_STATUS_FINISHED          = 2, /*< nick=Finished >*/
    MALGTK_MANGA_SERIES_STATUS_NOT_YET_PUBLISHED = 3, /*< nick=Not Yet Published >*/
} MalgtkMangaSeriesStatus;

typedef enum /*< prefix=MALGTK_MANGA >*/
{
    MALGTK_MANGA_SERIES_TYPE_INVALID  = -1, /*< nick=Invalid Type >*/
    MALGTK_MANGA_SERIES_TYPE_MANGA    = 1,  /*< nick=Manga >*/
    MALGTK_MANGA_SERIES_TYPE_NOVEL    = 2,  /*< nick=Novel >*/
    MALGTK_MANGA_SERIES_TYPE_ONE_SHOT = 3,  /*< nick=One Shot >*/
    MALGTK_MANGA_SERIES_TYPE_DOUJIN   = 4,  /*< nick=Doujin >*/
    MALGTK_MANGA_SERIES_TYPE_MANHWA   = 5,  /*< nick=Manhwa >*/
    MALGTK_MANGA_SERIES_TYPE_MANHUA   = 6,  /*< nick=Manhua >*/
    MALGTK_MANGA_SERIES_TYPE_OEL      = 7,  /*< nick=OEL >*/
} MalgtkMangaSeriesType;

typedef enum /*< prefix=MALGTK_MANGA >*/
{
    MALGTK_MANGA_STORAGE_TYPE_INVALID      = -1, /*< nick=Invalid Storage Type >*/
    MALGTK_MANGA_STORAGE_TYPE_NONE         = 0,  /*< nick=None >*/
    MALGTK_MANGA_STORAGE_TYPE_HARD_DRIVE   = 1,  /*< nick=Harddrive >*/
    MALGTK_MANGA_STORAGE_TYPE_EXTERNAL_HD  = 2,  /*< nick=External HD >*/
    MALGTK_MANGA_STORAGE_TYPE_DVDCD        = 3,  /*< nick=DVD/CD >*/
    MALGTK_MANGA_STORAGE_TYPE_RETAIL_MANGA = 4,  /*< nick=Retail Manga >*/
    MALGTK_MANGA_STORAGE_TYPE_MAGAZINE     = 5,  /*< nick=Magazine >*/
    MALGTK_MANGA_STORAGE_TYPE_NAS          = 6,  /*< nick=NAS >*/
} MalgtkMangaStorageType;
    
#define MALGTK_TYPE_MANGA             (malgtk_manga_get_type ())
G_DECLARE_FINAL_TYPE(MalgtkManga, malgtk_manga, MALGTK, MANGA, MalgtkMalitem)

struct _MalgtkMangaClass
{
    MalgtkMalitemClass parent_class;
};

MalgtkManga     *malgtk_manga_new (void);
void             malgtk_manga_set_from_xml(MalgtkManga *manga, xmlTextReaderPtr reader);

G_END_DECLS

