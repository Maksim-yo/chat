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

    auto res = m_db->createUserToken(token);
    OATPP_ASSERT(res->isSuccess() == true);
    auto dbResult = res->fetch<oatpp::Vector<oatpp::Object<UserToken>>>();
    OATPP_ASSERT(dbResult->size() == 1);
    auto token_value = dbResult[0];
    OATPP_LOGD("AuthTokenService", "Token %s created success", token_value->token.getValue("None"));
    clearExpiredTokens();

    return token_value;
}

void AuthTokenService::onUserAuthenticated(oatpp::Object<UserDto> user)
{
    auto res = m_db->findTokenByUser(user->id);
    if (!res->isSuccess())
        return;
    // auto dbResult = res->fetch<oatpp::Vector<oatpp::Object><UserToken>>();

}

std::optional<oatpp::Object<UserDto>> AuthTokenService::proccessToken(oatpp::String tokenValue)
{
    auto res = m_db->findToken(tokenValue);
    if (!res->isSuccess())
        return std::nullopt;

    auto dbResult = res->fetch<oatpp::Vector<oatpp::Object<UserToken>>>();
    OATPP_ASSERT(dbResult->size() == 1);
    auto token = dbResult[0];
    if (Utils::isDateExpired(token->expiry)){
        m_db->deleteTokenByValue(tokenValue);
        OATPP_LOGD("AuthTokenService", "Token %s expired", tokenValue);
        return std::nullopt;
    }

    res = m_db->getUserById(token->id);
    return res->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];
}

void AuthTokenService::clearExpiredTokens()
{
    m_db->deleteExpiredTokens(Utils::getCurrentTimeInSeconds());
}
