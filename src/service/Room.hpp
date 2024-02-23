#ifndef ROOM_HPP
#define ROOM_HPP

#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"

#include <unordered_map>

#include "dto/DTOs.hpp"
#include "dao/ChatDao.hpp"
class Peer;
class Lobby;
class Room {
private:
    oatpp::Int32 m_id;
    oatpp::Vector<oatpp::Object<MessageDto>> m_messageHistory;
    oatpp::Vector<oatpp::Object<MessageDto>> m_changedMessages;
    oatpp::Vector<oatpp::Object<PeerDto>> m_users;
    std::mutex m_historyLock;
    std::mutex m_peerByIdLock;
    std::mutex m_changedMessagesLock;
    std::mutex m_usersLock;
    Lobby* lobby;

    OATPP_COMPONENT(std::shared_ptr<ChatDao>, m_chatDao);    
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);

public:
    Room(oatpp::Object<ChatDto> chatDto, Lobby* lobby);
    oatpp::Int32 getId();
    void sendMessageAsync(const oatpp::String& message) ;
    void markMessagesAsRead(int count);
    void addHistoryMessage(const oatpp::Object<MessageDto>& message);
    oatpp::Object<ChatDto> getChatDto();
};
#endif