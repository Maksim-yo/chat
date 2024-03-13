#include <chrono>
#include <cstdlib>

#include "Peer.hpp"
#include "RoomManager.hpp"
#include "dto/Messages.hpp"
#include "service/logging/ILogger.hpp"
#include "utils/utils.hpp"

using namespace std::chrono;

Peer::Peer(const std::shared_ptr<AsyncWebSocket>& socket, oatpp::Int32 peerId, oatpp::String nickname, std::shared_ptr<RoomManager> roomManager)
    : m_socket(socket), m_peerId(peerId), m_nickname(nickname), m_roomManager(std::move(roomManager))
{
    sessionId = rand();
    OATPP_LOGD("MyApp", "Peer created. Peer session id: %d", sessionId);
}

Peer::~Peer()
{
    OATPP_LOGD("MyApp", "Peer destroyed. Peer session id: %d", sessionId);
    shouldDestroy = true;
    // Refactor, not safe
    std::lock_guard lck(m_roomsLock);
    for (auto itRoom = m_rooms.begin(); itRoom != m_rooms.end(); itRoom++) {
        (*itRoom)->peerLeft(this);
    }
}

oatpp::Object<PeerDto> Peer::createDto()
{

    auto peer = PeerDto::createShared();
    peer->peerId = m_peerId;
    peer->peerName = m_nickname;
    return peer;
}

oatpp::String Peer::createPeerStatusMessage(bool status)
{

    auto peerMessage = BaseMessage::createShared();
    peerMessage->peerId = m_peerId;
    peerMessage->peerNickname = m_nickname;
    peerMessage->timestamp = Utils::getCurrentTimeInMiliseconds();
    if (status == true)
        peerMessage->code = MessageCodes::CODE_PEER_ONLINE;
    else
        peerMessage->code = MessageCodes::CODE_PEER_OFFLINE;
    return MSG(peerMessage);
}

void Peer::sendMessageAsync(const oatpp::String& message)
{

    class SendMessageCoroutine : public oatpp::async::Coroutine<SendMessageCoroutine>
    {
    private:
        oatpp::async::Lock* m_lock;
        std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_websocket;
        oatpp::String m_message;

    public:
        SendMessageCoroutine(oatpp::async::Lock* lock, const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& websocket, const oatpp::String& message)
            : m_lock(lock), m_websocket(websocket), m_message(message)
        {
        }

        Action act() override
        {
            return oatpp::async::synchronize(m_lock, m_websocket->sendOneFrameTextAsync(m_message)).next(finish());
        }
    };

    m_asyncExecutor->execute<SendMessageCoroutine>(&m_writeLock, m_socket, message);
}
oatpp::async::CoroutineStarter Peer::sendPingAsyncScheduling(int maxTimeWait, int maxRetries, int interval)
{
    class SendPingCoroutine : public oatpp::async::Coroutine<SendPingCoroutine>
    {
    private:
        oatpp::async::Lock* m_lock;
        oatpp::async::Lock m_pongLock;
        std::mutex scheduleLock;
        std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_socket;
        Peer* m_peer;
        int peerSessionId;
        int m_maxTimeWait;
        int maxRetries;
        int maxRetriesBackup;
        bool m_wait;
        bool is_schedule{false};
        int m_interval;

    private:
        void onSuccess()
        {
            OATPP_LOGD("MyApp", "PING is good. Peer session id %d", peerSessionId);
            maxRetries = maxRetriesBackup;
        }

        void onFailure()
        {
            OATPP_LOGD("MyApp", "Request timeout exceeded. Peer session id %d", peerSessionId);
        }

    public:
        SendPingCoroutine(oatpp::async::Lock* lock, std::shared_ptr<oatpp::websocket::AsyncWebSocket> socket,
                          Peer* peer, int maxTimeWait, int maxRetries, int interval)
            : m_lock(lock), m_peer(peer), m_maxTimeWait(maxTimeWait), maxRetries(maxRetries), m_interval(interval), m_socket(socket)
        {
            peerSessionId = peer->getPeerId();
            maxRetriesBackup = maxRetries;
        }

        Action act() override
        {
            std::lock_guard lck(scheduleLock);
            if (is_schedule) {
                is_schedule = false;
                return waitRepeat(std::chrono::seconds(m_interval));
            }
            return yieldTo(&SendPingCoroutine::ping);
        }
        Action ping()
        {
            if (m_peer->shouldDestroy)
                return finish();
            m_peer->m_pongArrived = false;
            maxRetries--;
            m_wait = true;
            OATPP_LOGD("MyApp", "Trying to ping. Thread id: %d. Peer session id: %d ", std::this_thread::get_id(), peerSessionId);
            return oatpp::async::synchronize(m_lock, m_socket->sendPingAsync(nullptr)).next(yieldTo(&SendPingCoroutine::waitPing));
        }
        Action waitPing()
        {
            oatpp::async::LockGuard lck(&m_pongLock);
            if (m_peer->shouldDestroy)
                return finish();

            if (m_peer->m_pongArrived) {
                onSuccess();
                is_schedule = true;
                return yieldTo(&SendPingCoroutine::act);
            }
            if (m_wait && !m_peer->m_pongArrived) {
                m_wait = false;
                return waitRepeat(std::chrono::seconds(m_maxTimeWait));
            }
            if (maxRetries == 0) {
                onFailure();
                return oatpp::async::synchronize(m_lock, m_socket->sendCloseAsync()).next(finish());
            }
            m_wait = true;
            return yieldTo(&SendPingCoroutine::ping);
        }
    };
    return SendPingCoroutine::start(&m_writeLock, m_socket, this, maxTimeWait, maxRetries, interval);
}

Room* Peer::getOrCreateRoom(oatpp::Int32 roomId)
{
    std::lock_guard lck(m_roomsLock);
    Room* room = nullptr;
    for (auto itRoom = m_rooms.begin(); itRoom != m_rooms.end(); itRoom++) {
        if ((*itRoom)->getId() == roomId)
            room = *itRoom;
    }
    if (room)
        return room;
    room = m_roomManager->getOrCreateRoom(roomId);
    m_rooms.push_back(room);
    return room;
}

oatpp::async::CoroutineStarter Peer::handleMessage(oatpp::String messageData, const oatpp::Object<BaseMessage>& message)
{
    switch (*message->code) {
    case MessageCodes::CODE_INFO: {
        auto message = InitialMessage::createShared();
        message->peerId = m_peerId;
        message->peerNickname = m_nickname;
        message->code = MessageCodes::CODE_INFO;
        message->chats = {};

        std::vector<Room*> _rooms = m_roomManager->getOrCreateRooms(this);
        for (auto itRoom = _rooms.begin(); itRoom != _rooms.end(); itRoom++) {
            message->chats->push_back((*itRoom)->getChatDto());
            m_roomsLock.lock();
            m_rooms.push_back(*itRoom);
            m_roomsLock.unlock();
        }
        sendMessageAsync(MSG(message));
        break;
    }
    case MessageCodes::CODE_PEER_IS_TYPING: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<ChatMessage>>(messageData);
        Room* room = getOrCreateRoom(newMessage->message->chat_id);
        room->sendMessageAsync(MSG(newMessage));
        break;
    }
    case MessageCodes::CODE_PEER_MESSAGE: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<ChatMessage>>(messageData);
        Room* room = getOrCreateRoom(newMessage->message->chat_id);
        room->addHistoryMessage(newMessage->message);
        room->sendMessageAsync(MSG(newMessage));
        break;
    }
    case MessageCodes::CODE_PEER_READ: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<ReadMessage>>(messageData);
        Room* room = getOrCreateRoom(newMessage->chat_id);
        room->markMessagesAsRead(newMessage->count);
        room->sendMessageAsync(MSG(newMessage));
        break;
    }
    case MessageCodes::CODE_FIND_ROOMS: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<FindMessage>>(messageData);
        auto query = newMessage->query;
        auto users = m_postgresUserDao->getUserStartWithByNickname(query);
        auto resultMessage = FindMessage::createShared();
        resultMessage->query = query;
        if (users.has_value())
            resultMessage->users = users.value();
        resultMessage->peerId = m_peerId;
        resultMessage->peerNickname = m_nickname;
        resultMessage->code = MessageCodes::CODE_FIND_ROOMS;
        sendMessageAsync(MSG(resultMessage));
        break;
    }
    }

    return nullptr;
}

oatpp::async::CoroutineStarter Peer::readMessage(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)
{

    if (size == 0) {

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.setCurrentPosition(0);

        oatpp::Object<BaseMessage> message;

        try {
            message = m_objectMapper->readFromString<oatpp::Object<BaseMessage>>(wholeMessage);
        } catch (const std::runtime_error& e) {
            OATPP_ASSERT(false);
        }
        message->peerId = m_peerId;
        message->timestamp = oatpp::base::Environment::getMicroTickCount();

        return handleMessage(wholeMessage, message);

    } else if (size > 0) {
        m_messageBuffer.writeSimple(data, size);
    }

    return nullptr;
}

oatpp::async::CoroutineStarter Peer::onClose(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message)
{
    return nullptr;
}
oatpp::async::CoroutineStarter Peer::onPong(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message)
{
    m_pongArrived = true;
    return nullptr;
}

oatpp::async::CoroutineStarter Peer::onPing(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message)
{

    return oatpp::async::synchronize(&m_writeLock, socket->sendPongAsync(message));
}

oatpp::Int32 Peer::getPeerId() const
{
    return m_peerId;
}

oatpp::String Peer::getPeerName() const
{
    return m_nickname;
}

int Peer::getSessionId() const
{
    return sessionId;
}
