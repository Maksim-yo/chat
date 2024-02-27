
#include "AuthServiceBase.hpp"

AuthServiceBase::AuthServiceBase()
{
}

std::optional<oatpp::Object<UserToken>> AuthServiceBase::authentication(oatpp::String login, oatpp::String password)
{
    auto user = m_postgresUserDao->getUserByLoginAndPassword(login, password);
    if (!user.has_value())
        return std::nullopt;
    auto token = m_postgresTokenDao->getTokenByUserId(user.value());
    if (!token.has_value())
        return m_authTokenService.createToken(user.value());

    return token;
}

std::optional<oatpp::Object<UserToken>> AuthServiceBase::registration(oatpp::Object<UserDto> user)
{
    auto createdUser = m_postgresUserDao->createUser(user);
    return m_authTokenService.createToken(createdUser);
}

std::shared_ptr<UserObject> AuthServiceBase::authorize(const oatpp::String& token)
{
    auto userId = m_authTokenService.proccessToken(token);
    if (userId.has_value()) {
        auto userObject = m_postgresUserDao->getUserById(userId.value()).value();
        return std::make_shared<UserObject>(userObject);
    }
    return nullptr;
}

std::shared_ptr<UserObject> AuthServiceBase::handleAuthorization(const oatpp::String& token)
{

    if (token) {

        auto authResult = authorize(token);

        if (authResult) {
            return authResult;
        }

        throw oatpp::web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_401, "Unauthorized");
    }

    throw oatpp::web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_401, "Authorization Required");
}
