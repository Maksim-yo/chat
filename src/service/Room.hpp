#ifndef ROOM_HPP
#define ROOM_HPP

#include "oatpp/core/macro/component.hpp"

#include "dto/DTOs.hpp"
#include <unordered_map>

class Peer;

class Room {
private:
    oatpp::String m_name;
    std::unordered_map<v_int64, std::shared_ptr<Peer>> m_peerById;
    oatpp::List<oatpp::Object<MessageDto>> m_messsageHistory;
    std::mutex m_historyLock;
    std::mutex m_peerByIdLock;

public:
    void addPeer(const std::shared_ptr<Peer>& peer);
    void welcomePeer(const std::shared_ptr<Peer>& peer);
    void goodbyePeer(const std::shared_ptr<Peer>& peer);
    void removePeer(v_int64 peerId);
    void sendMessageAsync(const oatpp::Object<MessageDto>& message);

};

#endif