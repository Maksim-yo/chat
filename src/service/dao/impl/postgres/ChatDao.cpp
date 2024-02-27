#include "ChatDao.hpp"
#include "service/db/DOs.hpp"

namespace {

    oatpp::Object<ChatDto> mapChatDo(oatpp::Object<ChatExtendedDo> msg)
    {

        auto chatDto = ChatDto::createShared();
        auto peerDto = PeerDto::createShared();
        peerDto->peerId = msg->user_id;
        peerDto->peerName = msg->nickname;
        chatDto->peers = {peerDto};
        chatDto->id = msg->id;
        return chatDto;
    }

    oatpp::Object<MessageDto> mapMessageDo(oatpp::Object<MessageDo> msg)
    {

        auto messageDto = MessageDto::createShared();
        messageDto->peerId = msg->user_id;
        messageDto->timestamp = msg->created_at;
        messageDto->message = msg->line_text;
        messageDto->messageHash = msg->message_hash;
        messageDto->is_read = msg->is_read;
        messageDto->chat_id = msg->id;
        return messageDto;
    }

} // namespace

namespace Postgres {

    std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> ChatDao::getPeersInChat(oatpp::Int32 id)
    {
        auto peersQuery = m_db->getPeersInChat(id);
        OATPP_ASSERT(peersQuery->isSuccess());
        if (!peersQuery->hasMoreToFetch())
            return std::nullopt;
        return peersQuery->fetch<oatpp::Vector<oatpp::Object<PeerDto>>>();
    }

    std::optional<oatpp::Vector<oatpp::Object<MessageDto>>> ChatDao::getUsersHistoryInChat(oatpp::Int32 chatId, oatpp::Int32 count)
    {
        auto historyQuery = m_db->getUsersHistoryInChat(chatId, count);
        OATPP_ASSERT(historyQuery->isSuccess());
        if (!historyQuery->hasMoreToFetch())
            return std::nullopt;
        auto objects = historyQuery->fetch<oatpp::Vector<oatpp::Object<MessageDo>>>();
        oatpp::Vector<oatpp::Object<MessageDto>> resultMessages({});
        auto temp = resultMessages.get();
        for (auto it = objects->begin(); it != objects->end(); it++)
            resultMessages->push_back(mapMessageDo(*it));
        return resultMessages;
    }

    std::optional<oatpp::Object<ChatDto>> ChatDao::getChatById(oatpp::Int32 id)
    {
        auto chatQuery = m_db->getChatById(id);
        OATPP_ASSERT(chatQuery->isSuccess());
        if (!chatQuery->hasMoreToFetch())
            return std::nullopt;
        auto chatDto = ChatDto::createShared();
        auto messages = getUsersHistoryInChat(id);
        if (messages.has_value())
            chatDto->history = messages.value();
        auto peers = getPeersInChat(id);
        OATPP_ASSERT(peers.has_value());
        chatDto->peers = peers.value();
        chatDto->id = id;
        return chatDto;
    }

    void ChatDao::appendMessage(const oatpp::Object<MessageDto>& message)
    {
        auto messageQuery = m_db->createChatMessage(message);
        OATPP_ASSERT(messageQuery->isSuccess());
        return;
    }

    std::optional<oatpp::Vector<oatpp::Object<ChatDto>>> ChatDao::getUserConverstationsHisotry(oatpp::Int32 id)
    {

        auto dbResult = m_db->executeQuery(
            "SELECT chat.id, app_user.id as user_id, app_user.nickname FROM chat JOIN app_user ON chat.user_first = app_user.id or chat.user_second = app_user.id WHERE (chat.user_first = :userId or chat.user_second = :userId) and app_user.id != :userId;", {{"userId", oatpp::Int32(id)}});
        OATPP_ASSERT(dbResult->isSuccess());
        if (!dbResult->hasMoreToFetch())
            return std::nullopt;
        oatpp::Vector<oatpp::Object<ChatDto>> converstationsResult({});
        auto conversationHistory = dbResult->fetch<oatpp::Vector<oatpp::Object<ChatExtendedDo>>>();
        for (auto chatIter = conversationHistory->begin(); chatIter != conversationHistory->end(); chatIter++) {
            auto messages = getUsersHistoryInChat((*chatIter)->id);
            auto userConversationDto = mapChatDo(*chatIter);
            auto peers = getPeersInChat((*chatIter)->id);
            if (peers.has_value())
                userConversationDto->peers = peers.value();
            if (messages.has_value())
                userConversationDto->history = {messages.value()};
            converstationsResult->push_back(userConversationDto);
        }
        return converstationsResult;
    }
} // namespace Postgres