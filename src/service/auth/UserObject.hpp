#ifndef __USEROJBECT_H__
#define __USEROJBECT_H__

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"

#include "dto/UserDto.hpp"

class UserObject: public oatpp::web::server::handler::AuthorizationObject {
public:
    oatpp::Object<UserDto> user;
    
    UserObject(oatpp::Object<UserDto> user) : user(user) {}
};
#endif // __USEROJBECT_H__