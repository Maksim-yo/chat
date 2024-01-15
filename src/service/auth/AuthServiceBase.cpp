
#include "AuthServiceBase.hpp"

AuthServiceBase::AuthServiceBase()
{
    
}

std::optional<oatpp::Object<UserToken>> AuthServiceBase::authentication(oatpp::String login, oatpp::String password)
{
    auto user = m_chatDao->getUserByLoginAndPassword(login, password);
    if (!user.has_value())
        return std::nullopt;
    auto token = m_chatDao->getTokenByUserId(user.value());
    if (!token.has_value())
        return m_authTokenService.createToken(user.value());

    return std::nullopt;
}

std::optional<oatpp::Object<UserToken>> AuthServiceBase::registration(oatpp::Object<UserDto> user)
{
    auto createdUser = m_chatDao->createUser(user);
    return m_authTokenService.createToken(createdUser);
    
}

std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> AuthServiceBase::authorize(const oatpp::String& token)
{
    auto res = m_authTokenService.proccessToken(token);
    if (res.has_value())
        return std::make_shared<UserObject>(res.value());
    return nullptr;
}

std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> AuthServiceBase::handleAuthorization(const oatpp::String &token)
{   
    
    if (token) { 

        auto authResult = authorize(token);

        if(authResult) {
            return authResult;
        }

        throw oatpp::web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_401, "Unauthorized");

    }
    
    throw oatpp::web::protocol::http::HttpError(oatpp::web::protocol::http::Status::CODE_401, "Authorization Required");
}


