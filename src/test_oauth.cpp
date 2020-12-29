#include <boost/ut.hpp>
#include <string>
#include "oauth.h"


std::basic_string<unsigned char> operator"" _us(const char *str, size_t len) {
    return {reinterpret_cast<const unsigned char*>(str), len};
}

int main() {
    using namespace boost::ut;
    using namespace std::string_literals;

    "RFC 4648 Test Vectors"_test = [] {
        expect(malgtk::base64_url_encode(""_us) == ""s);
        expect(malgtk::base64_url_encode("f"_us) == "Zg"s);
        expect(malgtk::base64_url_encode("fo"_us) == "Zm8"s);
        expect(malgtk::base64_url_encode("foo"_us) == "Zm9v"s);
        expect(malgtk::base64_url_encode("foob"_us) == "Zm9vYg"s);
        expect(malgtk::base64_url_encode("fooba"_us) == "Zm9vYmE"s);
        expect(malgtk::base64_url_encode("foobar"_us) == "Zm9vYmFy"s);
    };

    "RFC 7636 Appendix A"_test = [] {
        std::array<unsigned char, 5> arr {3, 236, 255, 224, 193};
        expect(malgtk::base64_url_encode(arr) == "A-z_4ME"s);
        expect(malgtk::base64_url_encode("\x3\xEC\xFF\xE0\xC1"_us) == "A-z_4ME"s);
    };

    "RFC 7636 Appendix B"_test = [] {
        OAuthRequest req;
        constexpr std::array<unsigned char,32> arr {
            116, 24, 223, 180, 151, 153, 224, 37, 79, 250, 96, 125, 216, 173,
            187, 186, 22, 212, 37, 77, 105, 214, 191, 240, 91, 88, 5, 88, 83,
            132, 141, 121
        };
        req.code_verifier = malgtk::base64_url_encode(arr);
        expect(req.code_verifier == "dBjftJeZ4CVP-mB92K27uhbUJU1p1r_wW1gFWFOEjXk"s);
        expect(req.get_code_challenge(OAuthRequest::code_challenge_method::S256) == "E9Melhoa2OwvFrEMTJguCHaoeK1t8URWbuGJSstw-cM"s);
    };
}
