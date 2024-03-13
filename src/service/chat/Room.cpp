#include "oatpp/core/utils/ConversionUtils.hpp"

#include "Peer.hpp"
#include "Room.hpp"
#include "utils/utils.hpp"

Room::Room(oatpp::Object<ChatDto> chatDto) : m_id(chatDto->id), m_messageHistory(chatDto->history), m_users({}), m_changedMessages({})
{
    if (chatDto->peers->size() != 0)
        std::for_each(chatDto->peers->begin(), chatDto->peers->end(), [this](oatpp::Object<PeerDto> peer) { m_users->emplace_back(peer); });
    m_newMessagesdIndex = m_messageHistory->size();
}

Room::~Room()
{
    saveData();
}

oatpp::Int32 Room::getId()
{
    return m_id;
}

void Room::sendMessageAsync(const oatpp::String& message)
{
    m_peersLock.lock();
    for (auto itPeer = m_peers.begin(); itPeer != m_peers.end(); itPeer++) {
        (*itPeer)->sendMessageAsync(message);
    }
    m_peersLock.unlock();
}

void Room::markMessagesAsRead(int count)
{
    std::lock_guard<std::mutex> lock(m_historyLock);
    auto firstUnreadMessage = std::find_if(m_messageHistory->begin(), m_messageHistory->end(), [](oatpp::Object<MessageDto> msg) {
        if (msg->is_read == false)
            return true;
        return false;
    });
    while (count > 0 && firstUnreadMessage != m_messageHistory->end()) {
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
    m_currentMessagesCount += 1;
    size_t messageHash;
    Utils::hash_combine(messageHash, message->peerId, message->timestamp);
    // Depends on system
    message->messageHash = messageHash;
    m_messageHistory->push_back(message);
    if (m_currentMessagesCount == maxMessagesForSave)
        saveData();
}

oatpp::Object<ChatDto> Room::getChatDto()
{
    std::lock_guard<std::mutex> lock(m_historyLock);
    auto chat = ChatDto::createShared();
    for (auto itChatHistory = m_messageHistory->begin(); itChatHistory != m_messageHistory->end(); itChatHistory++) {
        for (auto itChangedMessages = m_changedMessages->begin(); itChangedMessages != m_changedMessages->end(); itChangedMessages++) {
            if ((*itChangedMessages)->messageHash == (*itChatHistory)->messageHash)
                (*itChatHistory) = (*itChangedMessages);
        }
    }
    chat->history = m_messageHistory;
    chat->peers = {};
    for (auto itUser = m_users->begin(); itUser != m_users->end(); itUser++) {
        auto user = PeerDto::createShared();
        user->peerId = (*itUser)->peerId;
        user->peerName = (*itUser)->peerName;
        user->isOnline = (*itUser)->isOnline;
        for (auto peer : m_peers) {
            if (peer->getPeerId() == (*itUser)->peerId) {
                user->isOnline = true;
                break;
            }
        }
        chat->peers->push_back(user);
    }
    chat->id = m_id;
    return chat;
}

void Room::peerJoin(Peer* peer)
{
    m_peersLock.lock();
    m_peers.push_back(peer);
    m_peersLock.unlock();
    sendMessageAsync(peer->createPeerStatusMessage(true));
}

void Room::peerLeft(Peer* peer)
{
    m_peersLock.lock();
    m_peers.erase(std::remove_if(m_peers.begin(), m_peers.end(), [&peer](Peer* p) { return p == peer; }), m_peers.end());
    m_peersLock.unlock();
    sendMessageAsync(peer->createPeerStatusMessage(false));
}

void Room::saveData()
{
    std::lock_guard<std::mutex> lock(m_historyLock);
    while (m_newMessagesdIndex < m_messageHistory->size()) {

        for (auto itNewMessage = m_changedMessages->begin(); itNewMessage != m_changedMessages->end(); itNewMessage++) {

            if (m_messageHistory[m_newMessagesdIndex]->messageHash == (*itNewMessage)->messageHash)
                m_messageHistory[m_newMessagesdIndex] = (*itNewMessage);
        }
        m_postgresChatDao->appendMessage(m_messageHistory[m_newMessagesdIndex]);
        m_newMessagesdIndex++;
    }
    m_currentMessagesCount = 0;
}

bool Room::shouldDestroy()
{
    return m_peers.size() == 0;
}
