#ifndef __AUTHTOKENSERVICE_H__
#define __AUTHTOKENSERVICE_H__

#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"

#include "UserObject.hpp"
#include "service/dao/impl/postgres/TokenDao.hpp"
#include "service/dao/impl/postgres/UserDao.hpp"
class AuthTokenService
{
private:
    OATPP_COMPONENT(std::shared_ptr<Postgres::TokenDao>, m_postgresTokenDao);

public:
    static const std::string TOKEN;

    AuthTokenService();
    oatpp::Object<UserToken> createToken(oatpp::Object<UserDto> user);
    void onUserAuthenticated(oatpp::Object<UserDto> user);
    std::optional<oatpp::Int32> proccessToken(oatpp::String tokenValue);
    void clearExpiredTokens();
};

#endif // __AUTHTOKENSERVICE_H__