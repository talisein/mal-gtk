#pragma once
#include <string>
#include <span>
#include <map>
#include <optional>
#include <sigc++/trackable.h>
#include <giomm/file.h>
#include <glibmm.h>
#include <functional>

enum class code_challenge_method_e {
    PLAIN,
    S256
};

template<code_challenge_method_e CCM>
struct OAuthAuthorizationRequest
{
    static constexpr std::string_view response_type {"code"};
    std::string_view client_id;
    std::string redirect_uri;
    std::string state;
    std::string code_verifier;

    std::string get_code_challenge() const;

    constexpr std::string_view get_code_challenge_method() const {
        using namespace std::string_view_literals;
        switch (CCM) {
            case code_challenge_method_e::PLAIN:
                return "plain"sv;
            case code_challenge_method_e::S256:
                return "S256"sv;
        }
    }
};


template<code_challenge_method_e CCM>
class OAuthActor : public sigc::trackable
{
public:
    OAuthActor(const std::string_view &client_id, const std::string_view &redirect_uri);

    void on_open(const std::vector<Glib::RefPtr<Gio::File>> &vec_files, const Glib::ustring& hint);

    void get_access_token_async(const std::function<void (std::string_view)>& cb);

    OAuthAuthorizationRequest<CCM> get_authorization_request() const;

private:
    struct URIDeleter {
        void operator()(GUri *uri) const {
            g_uri_unref(uri);
        }
    };
    using GUriPtr = std::unique_ptr<GUri, URIDeleter>;

    std::map<std::string, OAuthAuthorizationRequest<CCM>> pending_authorizations;

    const std::string_view m_client_id;
    GUriPtr m_redirect_uri;
};

namespace malgtk {
    std::string base64_url_encode(std::span<const unsigned char> sequence);
}
