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

#include "generate_extra_defs.h"
#include <iostream>

#include "malgtk_malitem.h"
#include "malgtk_anime.h"
#include "malgtk_manga.h"


int
main(int, char**)
{
  std::cout << get_defs(MALGTK_TYPE_MALITEM)
            << get_defs(MALGTK_TYPE_ANIME)
            << get_defs(MALGTK_TYPE_MANGA)
            << std::endl;
  
  return 0;
}
