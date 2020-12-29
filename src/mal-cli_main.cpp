#include <cstdlib>
#include <gtkmm.h>
#include <iostream>
#include <random>
#include <cassert>
#include "oauth.h"
#include "mal-cli_config.h"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv,
                                        Glib::ustring(APPLICATION_ID.begin(), APPLICATION_ID.end()),
                                        Gio::ApplicationFlags::APPLICATION_HANDLES_OPEN);

    app->signal_open().connect([&](const auto &vec_files, const Glib::ustring& str) -> void {
        // vec files is a std::vector<glib::refptr<file>>
        std::cout << "In signal open!\n"
                  << "str is '" << str << "'\n"
                  << "Files:\n";
        for (auto &file : vec_files) {
            std::cout << "\turi: " << file->get_uri() << '\n';
            std::cout << "\tscheme: " << file->get_uri_scheme() << '\n';
            std::cout << '\n';

        }
        std::cout << "done.\n";
        app->release();
    });

    app->signal_activate().connect([&]() {
        std::cout << "activated!\n";

        const std::string_view baseurl {"https://myanimelist.net/v1/oauth2/authorize?response_type=code"};
        std::stringstream ss;
        static_assert(CLIENT_ID.size() > 0, "myanimelist.net client ID must be set via meson configure. See README.md");
        OAuthRequest req;
        ss << baseurl
           << "&client_id=" << CLIENT_ID
           << "&state=" << req.state
           << "&redirect_uri=" << REDIRECT_URI
           << "&code_challenge=" << req.get_code_challenge(OAuthRequest::code_challenge_method::PLAIN)
           << "&code_challenge_method=plain";
        auto url = ss.str();
        GError *err = NULL;
        std::cout << "Going to " << url << '\n';
        gtk_show_uri_on_window(nullptr, url.c_str(), GDK_CURRENT_TIME, &err);
        app->hold();
    });

    app->add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_STRING, "username");

    Glib::ustring username;
    app->signal_handle_local_options().connect([&](const Glib::RefPtr<Glib::VariantDict> &options) -> int {
                                                   options->lookup_value("username", username);
                                                   std::cout << "handled local options\n";
                                                   return -1;
                                               });

    app->signal_startup().connect([&]() -> void {
        std::cout << "Starting up!\n";
    });

    app->signal_shutdown().connect([&]() -> void {
        std::cout << "Shutting down...\n";
    });

    return app->run();
}
