#ifndef __AUTHTOKENSERVICE_H__
#define __AUTHTOKENSERVICE_H__


#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/core/macro/component.hpp"

#include "UserObject.hpp"
#include "service/db/Database.hpp"

class AuthTokenService {
private:
    OATPP_COMPONENT(std::shared_ptr<Database>, m_db);

public:

    static const std::string TOKEN;

    AuthTokenService();
    oatpp::Object<UserToken> createToken(oatpp::Object<UserDto> user);
    void onUserAuthenticated(oatpp::Object<UserDto> user);
    std::optional<oatpp::Object<UserDto>> proccessToken(oatpp::String token);
    void clearExpiredTokens();
};


#endif // __AUTHTOKENSERVICE_H__