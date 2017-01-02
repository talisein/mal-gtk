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

typedef struct _MalgtkDate
{
    guint day : 8;
    guint month : 8;
    guint year : 16;
} MalgtkDate;

#define MALGTK_TYPE_DATE malgtk_date_get_type()

GType       malgtk_date_get_type        (void);
MalgtkDate* malgtk_date_new             (void);
MalgtkDate* malgtk_date_copy            (const MalgtkDate *date);
void        malgtk_date_free            (MalgtkDate *date);
void        malgtk_date_clear           (MalgtkDate *date);

GDateDay    malgtk_date_get_day         (const MalgtkDate *date);
GDateMonth  malgtk_date_get_month       (const MalgtkDate *date);
GDateYear   malgtk_date_get_year        (const MalgtkDate *date);
GDate*      malgtk_date_get_g_date      (const MalgtkDate *date);
gchar*      malgtk_date_get_season      (const MalgtkDate *date);
void        malgtk_date_set_dmy         (MalgtkDate *date, GDateDay day, GDateMonth month, GDateYear year);
void        malgtk_date_set_from_string (MalgtkDate *date, const gchar *str);
gchar*      malgtk_date_get_string      (const MalgtkDate *date);
gboolean    malgtk_date_is_complete     (const MalgtkDate *date);
gboolean    malgtk_date_is_clear        (const MalgtkDate *date);
gboolean    malgtk_date_is_equal        (const MalgtkDate *a, const MalgtkDate *b);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(MalgtkDate, malgtk_date_free)

G_END_DECLS
