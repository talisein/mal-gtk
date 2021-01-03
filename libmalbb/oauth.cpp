#include <random>
#include <algorithm>
#include <cassert>
#include <gtkmm.h>
#include "oauth.h"
#include "rng.h"

namespace {
    constexpr static std::string_view unreserved_chars{"abcdefghijklmnopqrxtuvwxyzABCDEFGHIJKLMNOPQRXTUVWXYZ0123456789-._~"};

    std::string generate_state() {
        std::uniform_int_distribution<std::string_view::size_type> unreserved_dist{0UL, unreserved_chars.size() - 1UL};
        constexpr std::string::size_type sz = 20;
        std::string state(sz, 'F');
        std::generate(std::begin(state), std::end(state),
                      [&](){return unreserved_chars.at(unreserved_dist(malgtk::rng));});
        return state;
    }

    /* NOTE: The code verifier SHOULD have enough entropy to make it impractical
             to guess the value.  It is RECOMMENDED that the output of a
             suitable random number generator be used to create a 32-octet
             sequence.  The octet sequence is then base64url-encoded to produce
             a 43-octet URL safe string to use as the code verifier. */

    std::string generate_code_verifier() {
        std::uniform_int_distribution<guchar> verifier_dist {std::numeric_limits<guchar>::min(),
                                                             std::numeric_limits<guchar>::max()};
        std::array<guchar, 32> sequence;
        std::generate(sequence.begin(), sequence.end(), [&](){return verifier_dist(malgtk::rng);});
        return malgtk::base64_url_encode(sequence);
    }

    std::string get_code_challenge_s256(const std::string &code_verifier) {
        Glib::Checksum cksum{Glib::Checksum::CHECKSUM_SHA256};
        std::vector<guint8> buf(Glib::Checksum::get_length(Glib::Checksum::CHECKSUM_SHA256));
        cksum.update(code_verifier);
        gsize len = buf.size();
        cksum.get_digest(buf.data(), &len);
        assert(len == buf.size());
        return malgtk::base64_url_encode(buf);
    }
}

template<code_challenge_method_e CCM>
std::string OAuthAuthorizationRequest<CCM>::get_code_challenge() const
{
    switch (CCM) {
        case code_challenge_method_e::PLAIN:
            return code_verifier;
        case code_challenge_method_e::S256:
            return get_code_challenge_s256(code_verifier);
    }
}

template<code_challenge_method_e CCM>
OAuthActor<CCM>::OAuthActor(const std::string_view &client_id,
                            const std::string_view &redirect_uri)
    : m_client_id(client_id)
{
    GError *err = nullptr;
    GUriPtr guri(g_uri_parse(redirect_uri.data(), G_URI_FLAGS_NON_DNS, &err));
    if (err) Glib::Error::throw_exception(err);
    m_redirect_uri = std::move(guri);
}

template<code_challenge_method_e CCM>
OAuthAuthorizationRequest<CCM>
OAuthActor<CCM>::get_authorization_request() const
{
    return {
        m_client_id,
        Glib::convert_return_gchar_ptr_to_stdstring(g_uri_to_string(m_redirect_uri.get())),
        generate_state(),
        generate_code_verifier()
    };
}

template<code_challenge_method_e CCM>
void
OAuthActor<CCM>::on_open(const std::vector<Glib::RefPtr<Gio::File>> &vec_files, const Glib::ustring& hint)
{
    for (auto &file : vec_files) {
        auto uri = file->get_uri();

        GError *err = nullptr;
        GUriPtr guri(g_uri_parse(uri.c_str(), G_URI_FLAGS_NON_DNS, &err));;
        if (err) Glib::Error::throw_exception(err);

        std::string_view scheme{g_uri_get_scheme(guri.get())};
        std::string_view host{g_uri_get_host(guri.get())};
        std::string_view path{g_uri_get_path(guri.get())};
        std::string_view query{g_uri_get_query(guri.get())};

        if (scheme != std::string_view(g_uri_get_scheme(m_redirect_uri.get()))) {
            g_warning("Unexpected scheme '%s' in uri: %s", scheme.data(), uri.c_str());
            continue;
        }
        if (host != std::string_view(g_uri_get_host(m_redirect_uri.get()))) {
            g_warning("Unexpected host '%s' in uri: %s", host.data(), uri.c_str());
            continue;
        }

        GUriParamsIter iter;
        GError *error = NULL;
        gchar *unowned_attr, *unowned_value;
        std::string authorization_code;
        std::string returned_state;

        g_uri_params_iter_init (&iter, query.data(), query.size(), "&", G_URI_PARAMS_NONE);
        while (g_uri_params_iter_next (&iter, &unowned_attr, &unowned_value, &error))
        {
            //meow
            std::string attr = Glib::convert_return_gchar_ptr_to_stdstring(unowned_attr);
            std::string value = Glib::convert_return_gchar_ptr_to_stdstring(unowned_value);
            using namespace std::string_literals;
            if (attr == "code"s)
                authorization_code = value;
            else if (attr == "state"s)
                returned_state = value;
            else {
                g_info("Unexpected attribute %s=%s in ur: %s", attr.c_str(), value.c_str(), uri.c_str());
            }
        }
        if (err) Glib::Error::throw_exception(err);

        auto it = pending_authorizations.find(returned_state);
        if (it == pending_authorizations.end()) {
            g_warning("Unexpected state returned, this request doesn't match up");
            continue;
        }

        std::cout << "Everything worked as expected, code: " << authorization_code << std::endl;
    }
}

template<code_challenge_method_e CCM>
void
OAuthActor<CCM>::get_access_token_async(const std::function<void (std::string_view)>& cb)
{
    const std::string_view baseurl {"https://myanimelist.net/v1/oauth2/authorize?"};
    std::stringstream ss;

    auto req = get_authorization_request();
    ss << baseurl
       << "response_type=" << req.response_type
       << "&client_id=" << req.client_id
       << "&state=" << req.state
       << "&redirect_uri=" << req.redirect_uri
       << "&code_challenge=" << req.get_code_challenge()
       << "&code_challenge_method=" << req.get_code_challenge_method();
    auto url = ss.str();
    GError *err = nullptr;
    pending_authorizations.insert({req.state, req});
    std::cout << "Going to " << url << '\n';
    gtk_show_uri_on_window(nullptr, url.c_str(), GDK_CURRENT_TIME, &err);
    if (err) Glib::Error::throw_exception(err);
}

std::string
malgtk::base64_url_encode(std::span<const unsigned char> sequence) {
    auto base64 = Glib::convert_return_gchar_ptr_to_stdstring(g_base64_encode(sequence.data(), sequence.size()));
    while (base64.ends_with('=')) base64.pop_back();
    std::replace_if(std::begin(base64), std::end(base64), [](auto p){return p == '+';}, '-');
    std::replace_if(std::begin(base64), std::end(base64), [](auto p){return p == '/';}, '_');
    return base64;
}

template class OAuthAuthorizationRequest<code_challenge_method_e::PLAIN>;
template class OAuthAuthorizationRequest<code_challenge_method_e::S256>;
template class OAuthActor<code_challenge_method_e::PLAIN>;
template class OAuthActor<code_challenge_method_e::S256>;
