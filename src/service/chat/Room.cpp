#include "oatpp/core/utils/ConversionUtils.hpp"

#include "Peer.hpp"
#include "Room.hpp"
#include "utils/utils.hpp"

Room::Room(oatpp::Object<ChatDto> chatDto, Lobby* lobby) : m_id(chatDto->id), lobby(lobby), m_messageHistory(chatDto->history), m_users({}), m_changedMessages({})
{
    if (chatDto->peers->size() != 0)
        std::for_each(chatDto->peers->begin(), chatDto->peers->end(), [this](oatpp::Object<PeerDto> peer) { m_users->emplace_back(peer); });
}

oatpp::Int32 Room::getId()
{
    return m_id;
}

void Room::sendMessageAsync(const oatpp::String& message)
{
    lobby->peersLock();
    for (auto it = m_users->begin(); it != m_users->end(); it++) {
        auto peers = lobby->getPeers();
        auto peer = peers.find((*it)->peerId);
        if (peer != peers.end())
            peer->second->sendMessageAsync(message);
    }
    lobby->peersUnlock();
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
    m_currentMessagesCount++;
    if (m_newMessagesdIndex == -1)
        m_newMessagesdIndex = m_messageHistory->size() - 1;
    size_t messageHash;
    Utils::hash_combine(messageHash, message->peerId, message->timestamp);
    // Depends on system
    message->messageHash = messageHash;
    m_messageHistory->push_back(message);
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
    chat->peers = m_users;
    chat->id = m_id;
    return chat;
}

void Room::saveData()
{
    std::lock_guard<std::mutex> lock(m_historyLock);
    while (m_newMessagesdIndex != m_messageHistory->size()) {

        for (auto itNewMessage = m_changedMessages->begin(); itNewMessage != m_changedMessages->end(); itNewMessage++) {

            if (m_messageHistory[m_newMessagesdIndex]->messageHash == (*itNewMessage)->messageHash)
                m_messageHistory[m_newMessagesdIndex] = (*itNewMessage);
        }
        m_postgresChatDao->appendMessage(m_messageHistory[m_newMessagesdIndex]);
        m_newMessagesdIndex++;
    }
}
