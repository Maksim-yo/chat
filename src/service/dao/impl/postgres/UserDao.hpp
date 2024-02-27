#ifndef __USERDAO_H__
#define __USERDAO_H__

#include "oatpp/core/macro/component.hpp"

#include "interfaces/IUserDao.hpp"
#include "service/db/Database.hpp"

namespace Postgres {

    class UserDao : public IUserDao
    {
    private:
        OATPP_COMPONENT(std::shared_ptr<Database>, m_db);

    public:
        std::optional<oatpp::Object<UserDto>> getUserById(oatpp::Int32 id) override;
        oatpp::Object<UserDto> createUser(oatpp::Object<UserDto> user) override;
        std::optional<oatpp::Object<UserDto>> getUserByLoginAndPassword(oatpp::String login, oatpp::String password) override;
        std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> getUserStartWithByNickname(oatpp::String name) override;
    };
};     // namespace Postgres
#endif // __USERDAO_H__