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
#include <cstdlib>
#include <curl/curl.h>
#include "application.hpp"

int main(int argc, char* argv[]) {
    std::locale::global(std::locale(""));

    CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (code != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(code) << std::endl;
        return EXIT_FAILURE;
    }

    MAL::Application app(argc, argv);
    app.run();

    curl_global_cleanup();
    return EXIT_SUCCESS;
}
