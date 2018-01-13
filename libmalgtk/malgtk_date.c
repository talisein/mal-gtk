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

#include "malgtk_date.h"
#include <string.h>

static void
xform_from_g_date(const GValue *src_value, GValue *dest_value)
{
    const GDate *src = g_value_get_boxed(src_value);
    MalgtkDate *d = malgtk_date_new_from_date(src);
    g_value_take_boxed(dest_value, d);
}

static void
xform_to_g_date(const GValue *src_value, GValue *dest_value)
{
    const MalgtkDate *src = g_value_get_boxed(src_value);
    g_value_take_boxed(dest_value, malgtk_date_get_g_date(src));
}

static void
xform_from_string(const GValue *src_value, GValue *dest_value)
{
    const gchar *src = g_value_get_string(src_value);
    MalgtkDate *d = malgtk_date_new_from_string(src);
    g_value_take_boxed(dest_value, d);
}

static void
xform_to_string(const GValue *src_value, GValue *dest_value)
{
    const MalgtkDate *src = g_value_get_boxed(src_value);
    g_value_take_string(dest_value, malgtk_date_get_string(src));
}

static void
register_date_transform_func(GType typeid)
{
    g_value_register_transform_func(G_TYPE_DATE, typeid, xform_from_g_date);
    g_value_register_transform_func(typeid, G_TYPE_DATE, xform_to_g_date);
    g_value_register_transform_func(G_TYPE_STRING, typeid, xform_from_string);
    g_value_register_transform_func(typeid, G_TYPE_STRING, xform_to_string);
}

G_DEFINE_BOXED_TYPE_WITH_CODE (MalgtkDate, malgtk_date,
                               malgtk_date_copy,
                               malgtk_date_free,
                               register_date_transform_func(g_define_type_id))

MalgtkDate*
malgtk_date_new(void)
{
    MalgtkDate *d = g_malloc (sizeof (MalgtkDate));
    malgtk_date_clear (d);

    return d;
}

MalgtkDate*
malgtk_date_new_from_string(const gchar *str)
{
    MalgtkDate *d = g_malloc (sizeof (MalgtkDate));
    malgtk_date_set_from_string(d, str);

    return d;
}

MalgtkDate*
malgtk_date_new_from_date(const GDate *gdate)
{
    MalgtkDate *d = g_malloc (sizeof (MalgtkDate));

    *d = (MalgtkDate){
        .day   = gdate->day,
        .month = gdate->month,
        .year  = gdate->year,
    };

    return d;
}

MalgtkDate*
malgtk_date_copy(const MalgtkDate *date)
{
    MalgtkDate *d = g_malloc (sizeof (MalgtkDate));
    *d = *date;
    return d;
}

void
malgtk_date_free(MalgtkDate *date)
{
    g_free (date);
}

void
malgtk_date_clear(MalgtkDate *date)
{
    date->day   = G_DATE_BAD_DAY;
    date->month = G_DATE_BAD_MONTH;
    date->year  = G_DATE_BAD_YEAR;
}

GDateDay
malgtk_date_get_day(const MalgtkDate *date)
{
    if (g_date_valid_day (date->day))
        return date->day;
    else
        return G_DATE_BAD_DAY;
}

GDateMonth
malgtk_date_get_month(const MalgtkDate *date)
{
    if (g_date_valid_month (date->month))
        return date->month;
    else
        return G_DATE_BAD_MONTH;
}

GDateYear
malgtk_date_get_year(const MalgtkDate *date)
{
    if (g_date_valid_year (date->year))
        return date->year;
    else
        return G_DATE_BAD_MONTH;
}

GDate*
malgtk_date_get_g_date(const MalgtkDate *date)
{
    g_return_val_if_fail(malgtk_date_is_complete (date), g_date_new());
    return g_date_new_dmy (date->day, date->month, date->year);
}

void
malgtk_date_set_dmy(MalgtkDate *date, GDateDay day, GDateMonth month, GDateYear year)
{
    if (g_date_valid_day(day))
        date->day = day;
    else
        date->day = G_DATE_BAD_DAY;
    if (g_date_valid_month(month))
        date->month = month;
    else
        date->month = G_DATE_BAD_MONTH;
    if (g_date_valid_year(year))
        date->year = year;
    else
        date->year = G_DATE_BAD_YEAR;
}

gchar*
malgtk_date_get_season(const MalgtkDate *date)
{
    GDateYear year;

    if (!g_date_valid_year(date->year)) {
        return g_strdup("Unknown");
    }

    year = date->year;
    if ( g_date_valid_month(date->month) ) {
        switch (date->month) {
            case G_DATE_DECEMBER:
                ++year;
                __attribute__ ((fallthrough));
            case G_DATE_JANUARY:
            case G_DATE_FEBRUARY:
                return g_strdup_printf("Winter %u", year);
            case G_DATE_MARCH:
            case G_DATE_APRIL:
            case G_DATE_MAY:
                return g_strdup_printf("Spring %u", year);
            case G_DATE_JUNE:
            case G_DATE_JULY:
            case G_DATE_AUGUST:
                return g_strdup_printf("Summer %u", year);
            case G_DATE_SEPTEMBER:
            case G_DATE_OCTOBER:
            case G_DATE_NOVEMBER:
                return g_strdup_printf("Autumn %u", year);
            case G_DATE_BAD_MONTH:
            default:
                break;
        }
    }

    return g_strdup_printf("%u", year);
}

/* Expected format: YYYY-MM-DD */
void
malgtk_date_set_from_string (MalgtkDate *date, const gchar *str)
{
    size_t len;
    GDateYear year;
    GDateMonth month;
    GDateDay day;

    malgtk_date_clear(date);

    len = strlen(str);
    if (len < 4)
        return;

    year = g_ascii_strtoll(str, NULL, 10);
    if (g_date_valid_year(year))
        date->year = year;
    if (len < 6)
        return;

    month = g_ascii_strtoll(str + 5, NULL, 10);
    if (g_date_valid_month(month))
        date->month = month;
    if (len < 9)
        return;

    day = g_ascii_strtoll(str + 8, NULL, 10);
    if (g_date_valid_day(day))
        date->day = day;
}

gchar *
malgtk_date_get_string(const MalgtkDate *date)
{
    if (NULL == date)
        return NULL;
    if (malgtk_date_is_complete(date))
        return g_strdup_printf("%u-%02u-%02u", date->year, date->month, date->day);
    if (g_date_valid_month(date->month) && g_date_valid_year(date->year))
        return g_strdup_printf("%u-%02u", date->year, date->month);
    if (g_date_valid_year(date->year))
        return g_strdup_printf("%u", date->year);
    return g_strdup("0000-00-00");
}

gboolean
malgtk_date_is_complete(const MalgtkDate *date)
{
    if (NULL == date)
        return FALSE;
    
    return g_date_valid_dmy(date->day, date->month, date->year);
}

gboolean
malgtk_date_is_clear(const MalgtkDate *date)
{
    return !g_date_valid_year(date->year) && !g_date_valid_month(date->month) && !g_date_valid_day(date->day);
}

gboolean
malgtk_date_is_equal(const MalgtkDate *a, const MalgtkDate *b)
{
    return a->day == b->day && a->month == b->month && a->year == b->year;
}

gboolean
malgtk_date_compare(const MalgtkDate *a, const MalgtkDate *b)
{
    g_return_val_if_fail(a != NULL, 0);
    g_return_val_if_fail(b != NULL, 0);

    if (a->year < b->year)
        return -1;
    else if (a->year > b->year)
        return 1;
    else {
        if (a->month < b->month)
            return -1;
        else if (a->month > b->month)
            return 1;
        else {
            if (a->day < b->day)
                return -1;
            else if (a->day > b->day)
                return 1;
            else
                return 0;
        }
    }
}
