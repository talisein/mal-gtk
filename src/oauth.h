#pragma once
#include <string>
#include <span>
#include <set>
#include <optional>
#include <sigc++/trackable.h>

enum class code_challenge_method_e {
    PLAIN,
    S256
};

template<code_challenge_method_e CCM>
struct OAuthAuthorizationRequest
{
    static constexpr std::string_view response_type {"code"};
    std::string_view client_id;
    std::optional<std::string_view> redirect_uri;
    std::optional<std::string_view> scope;
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
    OAuthActor(const std::string_view &client_id,
               const std::optional<std::string_view> &redirect_uri,
               const std::optional<std::string> &scope = std::nullopt)
        :
        m_client_id(client_id),
        m_redirect_uri(redirect_uri),
        m_scope(scope)
    {
    }

    OAuthAuthorizationRequest<CCM> get_authorization_request() const;

private:
    template<code_challenge_method_e CCME>
    struct OAuthAuthorizationStateComparitor {
        bool operator()(const OAuthAuthorizationRequest<CCME> &a, const OAuthAuthorizationRequest<CCME> &b) const {
            return a.state < b.state;
        }
    };

    using pending_set_t=std::set<OAuthAuthorizationRequest<CCM>, OAuthAuthorizationStateComparitor<CCM>>;
    pending_set_t pending_authorizations;

    const std::string_view m_client_id;
    const std::optional<std::string_view> m_redirect_uri;
    const std::optional<std::string> m_scope;
};

namespace malgtk {
    std::string base64_url_encode(std::span<const unsigned char> sequence);
}
