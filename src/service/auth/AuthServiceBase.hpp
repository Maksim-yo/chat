#ifndef AUTH_HPP
#define AUTH_HPP
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include "dto/UserDto.hpp"
#include "service/dao/ChatDao.hpp"
#include "AuthTokenService.hpp"

class AuthServiceBase {
private:
    OATPP_COMPONENT(std::shared_ptr<ChatDao>, m_chatDao);
    AuthTokenService m_authTokenService;    
public:

    AuthServiceBase();
    std::optional<oatpp::Object<UserToken>> authentication(oatpp::String login, oatpp::String password);
    std::optional<oatpp::Object<UserToken>> registration(oatpp::Object<UserDto> user);
    std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> authorize(const oatpp::String& token);
    std::shared_ptr<oatpp::web::server::handler::AuthorizationObject> handleAuthorization(const oatpp::String &token);


 }; 


 #endif // AUTH_H