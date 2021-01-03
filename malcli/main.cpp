#include <cstdlib>
#include <gtkmm.h>
#include <iostream>
#include <random>
#include <cassert>
#include "oauth.h"
#include "config.h"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv,
                                        Glib::ustring(APPLICATION_ID.begin(), APPLICATION_ID.end()),
                                        Gio::ApplicationFlags::APPLICATION_HANDLES_OPEN);

    static_assert(CLIENT_ID.size() > 0, "myanimelist.net client ID must be set via meson configure. See README.md");
    OAuthActor<code_challenge_method_e::PLAIN> oauth(CLIENT_ID, REDIRECT_URI);

    app->signal_open().connect(sigc::mem_fun(oauth, &OAuthActor<code_challenge_method_e::PLAIN>::on_open));
    app->signal_open().connect([&](const auto &vec_files, const Glib::ustring& str) -> void {
            app->release();
    });

    app->signal_activate().connect([&]() {
        std::cout << "activated!\n";
        oauth.get_access_token_async([&](auto token) {
            std::cout << "got access token: " << token;
        });
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
