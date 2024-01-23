#ifndef ROOM_HPP
#define ROOM_HPP

#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"

#include <unordered_map>

#include "dto/DTOs.hpp"
#include "AbstractRoom.hpp"
#include "dao/ChatDao.hpp"
class Peer;
class Lobby;
class Room {
private:
    oatpp::Int32 m_id;
    oatpp::Vector<oatpp::Object<MessageDto>> m_messageHistory;
    oatpp::Vector<oatpp::Int32> m_users;
    std::mutex m_historyLock;
    std::mutex m_peerByIdLock;
    std::mutex m_usersLock;
    Lobby* lobby;

    OATPP_COMPONENT(std::shared_ptr<ChatDao>, m_chatDao);    
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);

public:
    Room(oatpp::Object<ChatDto> chatDto, Lobby* lobby);
    oatpp::Int32 getId();
    void addPeer(const Peer& peer);
    void welcomePeer(const Peer& peer);
    void goodbyePeer(const Peer& peer);
    void removePeer(oatpp::Int32  peerId);
    void sendMessageAsync(const oatpp::Object<ChatMessageDto>& message) ;
    // void sendMessageToPeerAsync(const oatpp::Object<BaseMessageDto>& message, );
    void addHistoryMessage(const oatpp::Object<ChatMessageDto>& message) ;

};

#endif