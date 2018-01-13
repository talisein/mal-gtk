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

#include <locale>
#include <iostream>
#include <string>
#include "glibmm.h"
#include "malgtkmm/init.h"
#include "malgtkmm/malitem.h"

static void
test_malitem_season(void)
{
    auto malitem = MALnew::Malitem::create();

    auto begin = malitem->property_season_begin().get_value();

    g_assert_cmpstr  (begin.c_str(), ==, "Unknown");

    /* Test explicit constructors */
    malitem->property_series_date_begin().set_value(MALnew::Date("2018-01-13"));
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Winter 2018");

    Glib::Date d {1, Glib::Date::JUNE, 2019};
    malitem->property_series_date_begin().set_value(MALnew::Date(d));
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Summer 2019");

    std::string str {"2020-10-1"};
    malitem->property_series_date_begin().set_value(MALnew::Date(str));
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Autumn 2020");

    Glib::ustring ustr {"2006-02-01"};
    malitem->property_series_date_begin().set_value(MALnew::Date(ustr));
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Winter 2006");

    /* Test implicit constructors */
    malitem->property_series_date_begin().set_value("2018-01-13");
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Winter 2018");

    malitem->property_series_date_begin().set_value(d);
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Summer 2019");
    
    malitem->property_series_date_begin().set_value(str);
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Autumn 2020");

    malitem->property_series_date_begin().set_value(ustr);
    begin = malitem->property_season_begin().get_value();    
    g_assert_cmpstr  (begin.c_str(), ==, "Winter 2006");
}

int
main(int argc, char *argv[])
{
    std::locale::global(std::locale(""));
    MALnew::init();

    g_test_init(&argc, &argv, nullptr);

    g_test_add_func("/malgtkmm/malitem/season", test_malitem_season);

    return g_test_run();
}

