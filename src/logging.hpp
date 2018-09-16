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
#include "glib.h"

namespace MAL
{
    constexpr const char * log_level_to_priority(GLogLevelFlags log_level)
    {
        if (log_level & G_LOG_LEVEL_ERROR)
            return "3";
        else if (log_level & G_LOG_LEVEL_CRITICAL)
            return "4";
        else if (log_level & G_LOG_LEVEL_WARNING)
            return "4";
        else if (log_level & G_LOG_LEVEL_MESSAGE)
            return "5";
        else if (log_level & G_LOG_LEVEL_INFO)
            return "6";
        else if (log_level & G_LOG_LEVEL_DEBUG)
            return "7";

        /* Default to LOG_NOTICE for custom log levels. */
        return "5";
    }
}
