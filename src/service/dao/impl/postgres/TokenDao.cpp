#include "TokenDao.hpp"
#include "service/db/DOs.hpp"
#include "utils/utils.hpp"

namespace Postgres {

    oatpp::Object<UserToken> TokenDao::createUserToken(oatpp::Object<UserToken> token)
    {
        auto tokenQuery = m_db->createUserToken(token);
        OATPP_ASSERT(tokenQuery->isSuccess());
        return tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>()[0];
    }

    std::optional<oatpp::Object<UserToken>> TokenDao::findToken(oatpp::String token)
    {
        auto tokenQuery = m_db->findToken(token);
        OATPP_ASSERT(tokenQuery->isSuccess());
        if (!tokenQuery->hasMoreToFetch())
            return std::nullopt;
        return tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>()[0];
    }

    std::optional<oatpp::Object<UserToken>> TokenDao::getTokenByUserId(oatpp::Object<UserDto> user)
    {
        auto tokenQuery = m_db->findTokenByUser(user->id);
        OATPP_ASSERT(tokenQuery->isSuccess());
        if (!tokenQuery->hasMoreToFetch())
            return std::nullopt;
        return tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>()[0];
    }

    void TokenDao::deleteExpiredTokens(int time)
    {
        if (time == -1)
            m_db->deleteExpiredTokens(Utils::getCurrentTimeInSeconds());
        else
            m_db->deleteExpiredTokens(time);
    }

    void TokenDao::deleteTokenByValue(oatpp::String token)
    {
        auto tokenQuery = m_db->deleteTokenByValue(token);
        OATPP_ASSERT(tokenQuery->isSuccess());
    }
} // namespace Postgres