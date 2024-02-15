#include "Room.hpp"
#include "Peer.hpp"


Room::Room(oatpp::Object<ChatDto> chatDto, Lobby* lobby) : m_id(chatDto->id), lobby(lobby), m_messageHistory(chatDto->history), m_users({}), m_changedMessages({})
{
    m_users->resize(chatDto->peers->size());
    if (m_users->size() != 0)
        std::transform(chatDto->peers->begin(), chatDto->peers->end(), m_users->begin(), [](oatpp::Object<PeerDto> peer) {return peer->peerId;});     
}


oatpp::Int32 Room::getId(){
    return m_id;
}




void Room::sendMessageAsync(const oatpp::String& message)
{
    lobby->peersLock();
    for (auto it = m_users->begin(); it != m_users->end(); it++){
        auto peers = lobby->getPeers();
        auto peer = peers.find(*it);
        if (peer != peers.end())
            peer->second->sendMessageAsync(message);
    }
    lobby->peersUnlock();   
}

void Room::markMessagesAsRead(int count)
{
    std::lock_guard<std::mutex> lock(m_historyLock);
    auto firstUnreadMessage = std::find_if(m_messageHistory->begin(), m_messageHistory->end(), [](oatpp::Object<MessageDto> msg){
        if (msg->is_read == false)
            return true;
        return false;
    });
    while (count > 0 && firstUnreadMessage != m_messageHistory->end()){
        (*firstUnreadMessage)->is_read = true;
        m_changedMessagesLock.lock();
        m_changedMessages->push_back(*firstUnreadMessage);
        m_changedMessagesLock.unlock();
        count--;
        firstUnreadMessage++;
    }
}

void Room::addHistoryMessage(const oatpp::Object<MessageDto>& message)
{
    std::lock_guard<std::mutex> lock(m_historyLock);
    m_messageHistory->push_back(message);
}

