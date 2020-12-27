#include <cstdlib>
#include <gtkmm.h>
#include <iostream>
#include <random>
#include "mal-cli_config.h"

namespace {
    std::random_device rd;
    std::seed_seq seq {rd(), rd(), rd(), rd()};
    std::default_random_engine rng{seq};
}

class OAuthRequest
{
private:
    constexpr static std::string_view unreserved_chars{"abcdefghijklmnopqrxtuvwxyzABCDEFGHIJKLMNOPQRXTUVWXYZ0123456789-._~"};
    static std::uniform_int_distribution<std::string_view::size_type> unreserved_dist;
    static std::uniform_int_distribution<std::size_t> verifier_len_dist;

    std::string generate_state() {
        constexpr std::string::size_type sz = 20;
        std::string state(sz, 'F');
        std::generate(std::begin(state), std::end(state),
                      [&](){return unreserved_chars.at(unreserved_dist(rng));});
        return state;
    }

    std::string generate_code_verifier() {
        const std::string::size_type sz = verifier_len_dist(rng);
        std::string code_verifier(sz, 'F');
        std::generate(std::begin(code_verifier), std::end(code_verifier),
                      [&](){return unreserved_chars.at(unreserved_dist(rng));});
        return code_verifier;
    }

public:
    /* An opaque value used by the client to maintain state between the request
         and callback.  The authorization server includes this value when
         redirecting the user-agent back to the client.  The parameter SHOULD be
         used for preventing cross-site request forgery as described in Section
         10.12. */
    std::string state;

    /* high-entropy cryptographic random STRING using the unreserved characters
         [A-Z] / [a-z] / [0-9] / "-" / "." / "_" / "~" from Section 2.3 of
         [RFC3986], with a minimum length of 43 characters and a maximum length
         of 128 characters.

       The code verifier SHOULD have enough entropy to make it impractical to
         guess the value.  It is RECOMMENDED that the output of a suitable
         random number generator be used to create a 32-octet sequence.  The
         octet sequence is then base64url-encoded to produce a 43-octet URL safe
         string to use as the code verifier.*/
    std::string code_verifier;

    OAuthRequest() :
        state(generate_state()),
        code_verifier(generate_code_verifier())
    {

    }
};
std::uniform_int_distribution<std::string_view::size_type> OAuthRequest::unreserved_dist{0, unreserved_chars.size() - 1};
std::uniform_int_distribution<std::size_t> OAuthRequest::verifier_len_dist{43, 128};


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
           << "&code_challenge=" << req.code_verifier
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
