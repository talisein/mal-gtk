#include <string>
#include <span>
//#include <compare>

class OAuthRequest
{
public:
    OAuthRequest();

    enum class code_challenge_method {
        PLAIN,
        S256
    };

    /* An opaque value used by the client to maintain state between the request
         and callback.  The authorization server includes this value when
         redirecting the user-agent back to the client.  The parameter SHOULD be
         used for preventing cross-site request forgery as described in Section
         10.12. */
    std::string state;

    /* high-entropy cryptographic random STRING using the unreserved characters
         [A-Z] / [a-z] / [0-9] / "-" / "." / "_" / "~" from Section 2.3 of
         [RFC3986], with a minimum length of 43 characters and a maximum length
         of 128 characters.*/
    std::string code_verifier;

    std::string get_code_challenge(code_challenge_method method);

    //auto operator<=>(const OAuthRequest &other) const {return state <=> other.state; }
};

struct OAuthRequestSetComparitor {
    bool operator()(const OAuthRequest &a, const OAuthRequest &b) const {
        return a.state < b.state;
    }
};

namespace malgtk {
    std::string base64_url_encode(std::span<const unsigned char> sequence);
}
