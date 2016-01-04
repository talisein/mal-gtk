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
#include <libxml/xmlreader.h>

G_BEGIN_DECLS

typedef enum /*< prefix=MALGTK_MALITEM >*/
{
    MALGTK_MALITEM_PRIORITY_INVALID = -1, /*< nick=Invalid Priority >*/
    MALGTK_MALITEM_PRIORITY_LOW     = 0,  /*< nick=Low >*/
    MALGTK_MALITEM_PRIORITY_MEDIUM  = 1,  /*< nick=Medium >*/
    MALGTK_MALITEM_PRIORITY_HIGH    = 2,  /*< nick=High >*/
} MalgtkMalitemPriority;

typedef enum /*< prefix=MALGTK_MALITEM >*/
{
    MALGTK_MALITEM_RECONSUME_VALUE_INVALID   = -1, /*< nick=Invalid Reconsume Value >*/
    MALGTK_MALITEM_RECONSUME_VALUE_VERY_LOW  = 1,  /*< nick=Very Low >*/
    MALGTK_MALITEM_RECONSUME_VALUE_LOW       = 2,  /*< nick=Low >*/
    MALGTK_MALITEM_RECONSUME_VALUE_MEDIUM    = 3,  /*< nick=Medium >*/
    MALGTK_MALITEM_RECONSUME_VALUE_HIGH      = 4,  /*< nick=High >*/
    MALGTK_MALITEM_RECONSUME_VALUE_VERY_HIGH = 5,  /*< nick=Very High >*/
} MalgtkMalitemReconsumeValue;


#define MALGTK_TYPE_MALITEM             (malgtk_malitem_get_type ())
G_DECLARE_DERIVABLE_TYPE(MalgtkMalitem, malgtk_malitem, MALGTK, MALITEM, GObject)

struct _MalgtkMalitemClass
{
    GObjectClass  parent_class;
};

typedef gboolean (*MalgtkSetForeachFunc)(const gchar *str, gpointer user_data);

MalgtkMalitem     *malgtk_malitem_new (void);
void               malgtk_malitem_add_synonym(MalgtkMalitem *item, const gchar *synonym);
void               malgtk_malitem_foreach_synonym(const MalgtkMalitem *item, MalgtkSetForeachFunc cb, gpointer user_data);
void               malgtk_malitem_add_tag(MalgtkMalitem *item, const gchar *synonym);
void               malgtk_malitem_foreach_tag(const MalgtkMalitem *item, MalgtkSetForeachFunc cb, gpointer user_data);
void               malgtk_malitem_set_from_xml(MalgtkMalitem *item, xmlTextReaderPtr reader);

G_END_DECLS
