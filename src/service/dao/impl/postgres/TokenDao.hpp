#ifndef __TOKENDAO_H__
#define __TOKENDAO_H__

#include "oatpp/core/macro/component.hpp"
#include <optional>

#include "dto/UserDto.hpp"
#include "interfaces/ITokenDao.hpp"
#include "service/db/Database.hpp"

namespace Postgres {

    class TokenDao : public ITokenDao
    {
    private:
        OATPP_COMPONENT(std::shared_ptr<Database>, m_db);

    public:
        oatpp::Object<UserToken> createUserToken(oatpp::Object<UserToken> token) override;
        std::optional<oatpp::Object<UserToken>> findToken(oatpp::String token) override;
        std::optional<oatpp::Object<UserToken>> getTokenByUserId(oatpp::Object<UserDto> user) override;
        void deleteExpiredTokens(int time = -1) override;
        void deleteTokenByValue(oatpp::String token) override;
    };
}; // namespace Postgres

#endif // __TOKENDAO_H__