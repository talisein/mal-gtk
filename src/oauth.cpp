#include <random>
#include <algorithm>
#include <cassert>
#include <gtkmm.h>
#include "oauth.h"
#include "rng.h"
#include "mal-cli_config.h"

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
OAuthAuthorizationRequest<CCM>
OAuthActor<CCM>::get_authorization_request() const
{
    return {
        m_client_id,
        m_redirect_uri,
        m_scope,
        generate_state(),
        generate_code_verifier()
    };
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
