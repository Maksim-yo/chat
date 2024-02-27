#ifndef __IUSERDAO_H__
#define __IUSERDAO_H__

#include <optional>

#include "dto/DTOs.hpp"
#include "dto/UserDto.hpp"

class IUserDao
{
public:
    virtual std::optional<oatpp::Object<UserDto>> getUserById(oatpp::Int32 id) = 0;
    virtual oatpp::Object<UserDto> createUser(oatpp::Object<UserDto> user) = 0;
    virtual std::optional<oatpp::Object<UserDto>> getUserByLoginAndPassword(oatpp::String login, oatpp::String password) = 0;
    virtual std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> getUserStartWithByNickname(oatpp::String name) = 0;
};

#endif // __IUSERDAO_H__