#include "Peer.hpp"
#include "utils/utils.hpp"
#include <chrono>

using namespace std::chrono;

Peer::Peer(std::shared_ptr<AsyncWebSocket> socket, oatpp::Int32 peerId, oatpp::String nickname, Lobby* lobby)
    : m_socket(socket), m_peerId(peerId), m_nickname(nickname), lobby(lobby)
{
    auto message = InitialMessage::createShared();
    auto converstationsHistory = m_postgresChatDao->getUserConverstationsHisotry(m_peerId);
    message->peerId = m_peerId;
    message->peerNickname = m_nickname;
    message->code = MessageCodes::CODE_INFO;
    message->chats = {};
    if (converstationsHistory.has_value()) {
        message->chats = converstationsHistory.value();
        for (auto it = converstationsHistory.value()->begin(); it != converstationsHistory.value()->end(); it++) {
            auto room = lobby->getRoom((*it)->id);
            if (room == nullptr)
                addRoom(std::make_shared<Room>(*it, lobby));
            else {
                *it = room->getChatDto();
            }
        }
    }
    sendMessageAsync(MSG(message));
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

void Peer::sendPingAsync()
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

            std::lock_guard<std::mutex> lock(m_peer->m_test);

            if (m_peer->m_pingCounter == 0) {

                m_peer->m_pingWaitList.notifyFirst();
                return finish();
            }
            if (m_wait) {
                const std::chrono::time_point<std::chrono::steady_clock> start =
                    std::chrono::steady_clock::now() + 10s;
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
            std::lock_guard<std::mutex> lock(m_peer->m_test);

            if (m_peer->m_pingCounter == 0) {
                OATPP_LOGI("MyApp", "PING");
                m_peer->m_pingCounter++;
                return oatpp::async::synchronize(m_lock, m_websocket->sendPingAsync(nullptr)).next(WaitCoroutine::start(m_peer)).next(finish());
            }
            return Action::createWaitListAction(&m_peer->m_pingWaitList);
        }
    };

    m_asyncExecutor->execute<SendPingCoroutine>(&m_writeLock, m_socket, this);
}

void Peer::sendPingAsyncWait()
{

    sendPingAsync();
}

std::shared_ptr<Room> Peer::getRoom(oatpp::Int32 roomId)
{
    auto room = lobby->getRoom(roomId);
    return room;
}

void Peer::addRoom(const std::shared_ptr<Room>& room)
{
    lobby->addRoom(room);
}

oatpp::async::CoroutineStarter Peer::handleMessage(oatpp::String messageData, const oatpp::Object<BaseMessage>& message)
{

    switch (*message->code) {
    case MessageCodes::CODE_PEER_IS_TYPING: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<ChatMessage>>(messageData);
        std::shared_ptr<Room> room = getRoom(newMessage->message->chat_id);
        if (!room)
            return nullptr;
        room->sendMessageAsync(MSG(newMessage));
        break;
    }
    case MessageCodes::CODE_PEER_MESSAGE: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<ChatMessage>>(messageData);
        std::shared_ptr<Room> room = getRoom(newMessage->message->chat_id);
        if (!room) {
            throw std::runtime_error("Room doesn't exist");
        }
        room->addHistoryMessage(newMessage->message);
        room->sendMessageAsync(MSG(newMessage));
        break;
    }
    case MessageCodes::CODE_PEER_JOINED:
        break;
    case MessageCodes::CODE_PEER_READ: {
        auto newMessage = m_objectMapper->readFromString<oatpp::Object<ReadMessage>>(messageData);
        std::shared_ptr<Room> room = getRoom(newMessage->chat_id);
        if (!room) {
            throw std::runtime_error("Room doesn't exist");
        }
        room->markMessagesAsRead(newMessage->count);
        room->sendMessageAsync(MSG(newMessage));
        break;
    }
    case MessageCodes::CODE_PEER_LEFT:
        break;
    case MessageCodes::CODE_PEER_MESSAGE_FILE:
        break;
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
    std::lock_guard<std::mutex> lock(m_test);

    --m_pingCounter;

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
