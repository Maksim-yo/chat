#ifndef __ITOKENDAO_H__
#define __ITOKENDAO_H__

#include <optional>

#include "dto/UserDto.hpp"

class ITokenDao
{
public:
    virtual oatpp::Object<UserToken> createUserToken(oatpp::Object<UserToken> token) = 0;
    virtual std::optional<oatpp::Object<UserToken>> findToken(oatpp::String token) = 0;
    virtual std::optional<oatpp::Object<UserToken>> getTokenByUserId(oatpp::Object<UserDto> user) = 0;
    virtual void deleteExpiredTokens(int time = -1) = 0;
    virtual void deleteTokenByValue(oatpp::String token) = 0;
};
#endif // __ITOKENDAO_H__