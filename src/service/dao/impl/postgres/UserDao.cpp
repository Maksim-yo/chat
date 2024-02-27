#include "UserDao.hpp"

namespace Postgres {

    std::optional<oatpp::Object<UserDto>> UserDao::getUserById(oatpp::Int32 id)
    {
        auto userQuery = m_db->getUserById(id);
        OATPP_ASSERT(userQuery->isSuccess());
        if (!userQuery->hasMoreToFetch())
            return std::nullopt;
        return userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];
    }

    oatpp::Object<UserDto> UserDao::createUser(oatpp::Object<UserDto> user)
    {
        auto userQuery = m_db->createUser(user);
        OATPP_ASSERT(userQuery->isSuccess())
        return userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];
    }

    std::optional<oatpp::Object<UserDto>> UserDao::getUserByLoginAndPassword(oatpp::String login, oatpp::String password)
    {
        auto userQuery = m_db->getUserByLoginAndPassword(login, password);
        OATPP_ASSERT(userQuery->isSuccess());
        if (!userQuery->hasMoreToFetch())
            return std::nullopt;
        return userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];
    }

    std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> UserDao::getUserStartWithByNickname(oatpp::String name)
    {
        auto userQuery = m_db->getUserStartWithByNickname(name + "%");
        OATPP_ASSERT(userQuery->isSuccess());
        if (!userQuery->hasMoreToFetch())
            return std::nullopt;
        auto userResult = userQuery->fetch<oatpp::Vector<oatpp::Object<PeerDto>>>();
    }
} // namespace Postgres