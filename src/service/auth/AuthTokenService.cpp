#include "AuthTokenService.hpp"
#include "oatpp/core/utils/Random.hpp"
#include "utils/utils.hpp"

const std::string AuthTokenService::TOKEN = "Auth_token";

AuthTokenService::AuthTokenService() 
{

}

oatpp::Object<UserToken> AuthTokenService::createToken(oatpp::Object<UserDto> user)
{
 
    oatpp::Object<UserToken> token = UserToken::createShared();
    token->token =  Utils::randomString(32);
    token->id = user->id;
    token->expiry = Utils::getDateFromCurrent(7);

    auto dbToken = m_chatDao->createUserToken(token);
    OATPP_LOGD("AuthTokenService", "Token %s created success", dbToken->token.getValue("None"));
    clearExpiredTokens();

    return dbToken;
}

void AuthTokenService::onUserAuthenticated(oatpp::Object<UserDto> user)
{
    // auto res = m_db->findTokenByUser(user->id);
    // if (!res->isSuccess())
    //     return;
    // auto dbResult = res->fetch<oatpp::Vector<oatpp::Object><UserToken>>();

}

std::optional<oatpp::Object<UserDto>> AuthTokenService::proccessToken(oatpp::String tokenValue)
{
    auto token = m_chatDao->findToken(tokenValue);
    if (!token.has_value())
        return std::nullopt;
    if (Utils::isDateExpired(token.value()->expiry)){
        m_chatDao->deleteTokenByValue(tokenValue);
        OATPP_LOGD("AuthTokenService", "Token %s expired", tokenValue);
        return std::nullopt;
    }
    return m_chatDao->getUserById(token.value()->id);

}
    

void AuthTokenService::clearExpiredTokens()
{
    m_chatDao->deleteExpiredTokens();
}
