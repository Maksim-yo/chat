#ifndef __CHATDAO_H__
#define __CHATDAO_H__

#include "oatpp/core/macro/component.hpp"

#include "dto/DTOs.hpp"
#include "interfaces/IChatDao.hpp"
#include "service/db/Database.hpp"

namespace Postgres {

    class ChatDao : public IChatDao
    {
    private:
        OATPP_COMPONENT(std::shared_ptr<Database>, m_db);

    public:
        std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> getPeersInChat(oatpp::Int32 id) override;
        std::optional<oatpp::Vector<oatpp::Object<MessageDto>>> getUsersHistoryInChat(oatpp::Int32 chatId, oatpp::Int32 count = 100) override;
        std::optional<oatpp::Object<ChatDto>> getChatById(oatpp::Int32 od) override;
        std::optional<oatpp::Vector<oatpp::Object<ChatDto>>> getUserConverstationsHisotry(oatpp::Int32 id) override;
        void appendMessage(const oatpp::Object<MessageDto>& message) override;
    };
};     // namespace Postgres
#endif // __CHATDAO_H__