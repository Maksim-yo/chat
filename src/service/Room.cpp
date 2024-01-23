#include "Room.hpp"
#include "Peer.hpp"


Room::Room(oatpp::Object<ChatDto> chatDto, Lobby* lobby) : m_id(chatDto->id), lobby(lobby), m_messageHistory(chatDto->history), m_users({})
{
    m_users->resize(chatDto->peers->size());
    if (m_users->size() != 0)
        std::transform(chatDto->peers->begin(), chatDto->peers->end(), m_users->begin(), [](oatpp::Object<PeerDto> peer) {return peer->peerId;});     
}


oatpp::Int32 Room::getId(){
    return m_id;
}

void Room::addPeer(const Peer& peer){
    // std::lock_guard<std::mutex> mtx(m_peerByIdLock);
    // m_peerById.emplace(peer->getPeerId(), peer);
}

void Room::welcomePeer(const Peer& peer){
    // auto joinedMessage = MessageDto::createShared();
    // joinedMessage->peerId = peer->getPeerId();
    // joinedMessage->peerName = peer->getPeerName();
    // joinedMessage->code = MessageCodes::CODE_PEER_JOINED;
    // joinedMessage->message = peer->getPeerName() + " - joined room";
    // sendMessageAsync(joinedMessage);
}

void Room::goodbyePeer(const Peer& peer){
    // auto goodbyeMessage = MessageDto::createShared();
    // goodbyeMessage->peerId = peer->getPeerId();
    // goodbyeMessage->peerName = peer->getPeerName();
    // goodbyeMessage->code = MessageCodes::CODE_PEER_LEFT;
    // goodbyeMessage->message = peer->getPeerName() + " - left room";
    // sendMessageAsync(goodbyeMessage);

}

void Room::removePeer(oatpp::Int32 peerId){
    // std::lock_guard<std::mutex> mtx(m_peerByIdLock);
    // auto peer = m_peerById.find(peerId);
    // if (peer != m_peerById.end())
    //     m_peerById.erase(peer);

}


void Room::sendMessageAsync(const oatpp::Object<ChatMessageDto>& message)
{
    lobby->peersLock();
    for (auto it = m_users->begin(); it != m_users->end(); it++){
        auto peers = lobby->getPeers();
        auto peer = peers.find(*it);
        if (peer != peers.end())
            peer->second->sendMessageAsync(MSG(message));
    }
    lobby->peersUnlock();
}

void Room::addHistoryMessage(const oatpp::Object<ChatMessageDto>& message)
{
    m_chatDao->appendMessage(message);
}

