#include <iostream>
#include <cstdlib>
#include <curl/curl.h>
#include "application.hpp"

int main(int argc, char** argv) {
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
