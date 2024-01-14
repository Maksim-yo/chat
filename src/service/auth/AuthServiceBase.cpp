#ifndef __AUTHSERVICEBASE_H__
#define __AUTHSERVICEBASE_H__

#include  "AuthServiceBase.hpp"
#include "UserObject.hpp"

AuthServiceBase::AuthServiceBase()
{
    
}

std::optional<oatpp::Object<UserToken>> AuthServiceBase::authentication(oatpp::String login, oatpp::String password)
{
    auto userQuery = m_db->getUserByLoginAndPassword(login, password);
    if (!userQuery->isSuccess())
        return std::nullopt;
    auto userResult  = userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
    OATPP_ASSERT(userResult->size() == 1)
    auto user = userResult[0];
    auto tokenQuery = m_db->findTokenByUser(user->id);
    if (!tokenQuery->isSuccess())
        return m_authTokenService.createToken(user);
    auto tokenResult =  tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>();
    OATPP_ASSERT(tokenResult->size() == 1)
    return tokenResult[0];
    

}

std::optional<oatpp::Object<UserToken>> AuthServiceBase::registration(oatpp::Object<UserDto> user)
{

    auto dbResult = m_db->createUser(user);
    if (!dbResult->isSuccess())
        return std::nullopt;
    // TODO: change
    auto userRes = dbResult->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();        // Getting id of user
    OATPP_ASSERT(userRes->size() == 1);
    return m_authTokenService.createToken(userRes[0]);
    
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


#endif // __AUTHSERVICEBASE_H__