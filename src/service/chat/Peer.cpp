#include <chrono>

#include "Peer.hpp"
#include "RoomManager.hpp"
using namespace std::chrono;

Peer::Peer(std::shared_ptr<AsyncWebSocket> socket, oatpp::Int32 peerId, oatpp::String nickname, std::shared_ptr<RoomManager> roomManager)
    : m_socket(socket), m_peerId(peerId), m_nickname(nickname), m_roomManager(std::move(roomManager))
{
}

Peer::~Peer()
{
    // TODO: Refactor, not safe, add timer
    std::lock_guard lck(m_roomsLock);
    for (auto itRoom = m_rooms.begin(); itRoom != m_rooms.end(); itRoom++) {
        (*itRoom)->peerLeft(this);
    }
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

bool Peer::sendPingAsync()
{
    class WaitCoroutine : public oatpp::async::Coroutine<WaitCoroutine>
    {
    private:
        Peer* m_peer;
        bool m_wait;

    public:
        WaitCoroutine(Peer* peer)
            : m_peer(peer), m_wait(true)
        {
        }

        Action act() override
        {

            std::lock_guard<std::mutex> lock(m_peer->m_pingLock);

            if (m_peer->m_pingCounter == 0) {

                m_peer->m_pingWaitList.notifyFirst();
                return finish();
            }
            if (m_wait) {
                const std::chrono::time_point<std::chrono::steady_clock> start =
                    std::chrono::steady_clock::now() + 5s;
                m_wait = false;
                return Action::createWaitListActionWithTimeout(&m_peer->m_pongWaitList, start);
            }
            OATPP_LOGI("MyApp", "Request timeout exceeded.");
            return finish();
        }
    };
    class SendPingCoroutine : public oatpp::async::Coroutine<SendPingCoroutine>
    {
    private:
        oatpp::async::Lock* m_lock;
        std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_websocket;
        Peer* m_peer;

    public:
        SendPingCoroutine(oatpp::async::Lock* lock, const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& websocket, Peer* peer)
            : m_lock(lock), m_websocket(websocket), m_peer(peer)
        {
        }

        Action act() override
        {
            std::lock_guard<std::mutex> lock(m_peer->m_pingLock);

            if (m_peer->m_pingCounter == 0) {
                OATPP_LOGI("MyApp", "PING");
                m_peer->m_pingCounter++;
                return oatpp::async::synchronize(m_lock, m_websocket->sendPingAsync(nullptr)).next(WaitCoroutine::start(m_peer)).next(finish());
            }
            return Action::createWaitListAction(&m_peer->m_pingWaitList);
        }
    };
    // TODO: Refactor
    if (m_pingCounter == 0)
        return true;
    m_asyncExecutor->execute<SendPingCoroutine>(&m_writeLock, m_socket, this);
    return false;
}

Room* Peer::getOrCreateRoom(oatpp::Int32 roomId)
{
    std::lock_guard<std::mutex> lck(m_roomsLock);
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
    std::lock_guard<std::mutex> lock(m_pingLock);

    m_pingCounter--;

    OATPP_LOGI("MyApp", "PONG");
    m_pongWaitList.notifyFirst();

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
