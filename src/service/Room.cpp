#include "Room.hpp"
#include "Peer.hpp"



void Room::addPeer(const std::shared_ptr<Peer>& peer){
    std::lock_guard<std::mutex> mtx(m_peerByIdLock);
    m_peerById[peer->getPeerId()] =  peer;
}

void Room::welcomePeer(const std::shared_ptr<Peer>& peer){
    auto joinedMessage = MessageDto::createShared();
    joinedMessage->peerId = peer->getPeerId();
    joinedMessage->peerName = peer->getPeerName();
    joinedMessage->code = MessageCodes::CODE_PEER_JOINED;
    joinedMessage->message = peer->getPeerName() + " - joined room";
    sendMessageAsync(joinedMessage);
}

void Room::goodbyePeer(const std::shared_ptr<Peer>& peer){
    auto goodbyeMessage = MessageDto::createShared();
    goodbyeMessage->peerId = peer->getPeerId();
    goodbyeMessage->peerName = peer->getPeerName();
    goodbyeMessage->code = MessageCodes::CODE_PEER_LEFT;
    goodbyeMessage->message = peer->getPeerName() + " - left room";
    sendMessageAsync(goodbyeMessage);

}

void Room::removePeer(v_int64 peerId){
    std::lock_guard<std::mutex> mtx(m_peerByIdLock);
    auto peer = m_peerById.find(peerId);
    if (peer != m_peerById.end())
        m_peerById.erase(peer);

}

void Room::sendMessageAsync(const oatpp::Object<MessageDto>& message)
{
    std::lock_guard<std::mutex> mtx(m_peerByIdLock);
    for (auto& pair : m_peerById){
        pair.second->sendMessageAsync(message);
    }
}

