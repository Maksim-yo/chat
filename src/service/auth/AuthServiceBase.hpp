#ifndef AUTH_HPP
#define AUTH_HPP
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "AuthTokenService.hpp"
#include "UserObject.hpp"
#include "dto/UserDto.hpp"
#include "service/dao/impl/postgres/TokenDao.hpp"
#include "service/dao/impl/postgres/UserDao.hpp"
class AuthServiceBase
{
private:
    OATPP_COMPONENT(std::shared_ptr<Postgres::UserDao>, m_postgresUserDao);
    OATPP_COMPONENT(std::shared_ptr<Postgres::TokenDao>, m_postgresTokenDao);

    AuthTokenService m_authTokenService;

public:
    AuthServiceBase();
    std::optional<oatpp::Object<UserToken>> authentication(oatpp::String login, oatpp::String password);
    std::optional<oatpp::Object<UserToken>> registration(oatpp::Object<UserDto> user);
    std::shared_ptr<UserObject> authorize(const oatpp::String& token);
    std::shared_ptr<UserObject> handleAuthorization(const oatpp::String& token);
};

#endif // AUTH_H