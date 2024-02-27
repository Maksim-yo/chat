#ifndef __ICHATDTO_H__
#define __ICHATDTO_H__

#include <optional>

#include "dto/DTOs.hpp"

class IChatDao
{
public:
    virtual std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> getPeersInChat(oatpp::Int32 id) = 0;
    virtual std::optional<oatpp::Vector<oatpp::Object<MessageDto>>> getUsersHistoryInChat(oatpp::Int32 chatId, oatpp::Int32 count = 100) = 0;
    virtual std::optional<oatpp::Object<ChatDto>> getChatById(oatpp::Int32 od) = 0;
    virtual std::optional<oatpp::Vector<oatpp::Object<ChatDto>>> getUserConverstationsHisotry(oatpp::Int32 id) = 0;
    virtual void appendMessage(const oatpp::Object<MessageDto>& message) = 0;
};
#endif // __ICHATDTO_H__