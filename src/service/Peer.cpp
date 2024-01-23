#include "Peer.hpp"

namespace {

    oatpp::Object<ChatDto> mapMessageChatDto(oatpp::Object<ChatMessageDto> msg){

        auto chatDto = ChatDto::createShared();
        chatDto->id = msg->id;
        chatDto->history = msg->history;
        chatDto->peers = msg->peers;
        return chatDto;
    }

}
Peer::Peer(std::shared_ptr<AsyncWebSocket> socket, oatpp::Int32 peerId, oatpp::String nickname,  Lobby* lobby)
: m_socket(socket),  m_peerId(peerId), m_nickname(nickname), m_waitListener(this), lobby(lobby)
{
        m_waitList.setListener(&m_waitListener);
        auto message = MessageInitial::createShared();
        auto converstationsHistory = m_chatDao->getUserConverstationsHisotry(m_peerId);
        message->peerId = m_peerId;
        message->peerNickname = m_nickname;
        message->code = MessageCodes::CODE_INFO;
        message->chats = {};
        if (converstationsHistory.has_value())
            message->chats = converstationsHistory.value();
        sendMessageAsync(MSG(message)); 

}  
                                                    
void Peer::sendMessageAsync(const oatpp::Object<BaseMessageDto>& message){

    class SendMessageCoroutine : public oatpp::async::Coroutine<SendMessageCoroutine> {
    private:
        oatpp::async::Lock* m_lock;
        std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_websocket;
        oatpp::String m_message;
    public:

        SendMessageCoroutine(oatpp::async::Lock* lock, const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& websocket, const oatpp::String& message)
        : m_lock(lock)
        , m_websocket(websocket)
        , m_message(message)
        {}

        Action act() override {
            return oatpp::async::synchronize(m_lock, m_websocket->sendOneFrameTextAsync(m_message)).next(finish());
        }

    };

    m_asyncExecutor->execute<SendMessageCoroutine>(&m_writeLock, m_socket, m_objectMapper->writeToString(message));

}

void Peer::sendMessageAsync(const oatpp::String& message){

    class SendMessageCoroutine : public oatpp::async::Coroutine<SendMessageCoroutine> {
    private:
        oatpp::async::Lock* m_lock;
        std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_websocket;
        oatpp::String m_message;
    public:

        SendMessageCoroutine(oatpp::async::Lock* lock, const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& websocket, const oatpp::String& message)
        : m_lock(lock)
        , m_websocket(websocket)
        , m_message(message)
        {}

        Action act() override {
            return oatpp::async::synchronize(m_lock, m_websocket->sendOneFrameTextAsync(m_message)).next(finish());
        }

    };

    m_asyncExecutor->execute<SendMessageCoroutine>(&m_writeLock, m_socket, message);

}


void Peer::sendPingAsync(){

    class SendPingCoroutine : public oatpp::async::Coroutine<SendPingCoroutine> {
    private:
        oatpp::async::Lock* m_lock;
        std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_websocket;
    public:

        SendPingCoroutine(oatpp::async::Lock* lock, const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& websocket)
        : m_lock(lock)
        , m_websocket(websocket)
        {}

        Action act() override {
        return oatpp::async::synchronize(m_lock, m_websocket->sendPingAsync(nullptr)).next(finish());
        }

    };

    class WaitCoroutine : public oatpp::async::Coroutine<WaitCoroutine> {
    private:
        Peer* m_peer;
    public:

        WaitCoroutine(Peer* peer)
        : m_peer(peer)
        {}

        Action act() override {
        std::lock_guard<std::mutex> lock(m_peer->m_test);

        OATPP_LOGI("MyApp", "WAIT COROUTINE CALLED");
        const int temp = 0;
        if(m_peer->m_pingCounter == temp) {
            
            OATPP_LOGI("MyApp", "WAIT COROUTINE ENDED");
            return finish();
        }
        return Action::createWaitListAction(&m_peer->m_waitList);
        }   

    };

    ++ m_pingCounter;
    // int value = m_pingCounter.load(std::memory_order_relaxed);

    OATPP_LOGI("MyApp", "PING");

    // if(m_socket && m_pingCounter == 1) {
        m_asyncExecutor->execute<SendPingCoroutine>(&m_writeLock, m_socket);
    // }

    m_asyncExecutor->execute<WaitCoroutine>(this);
    
}

void Peer::sendPingAsyncWait(){   
    
    sendPingAsync();
}

std::shared_ptr<Room> Peer::getRoom(oatpp::Int32 roomId) {
    std::lock_guard<std::mutex> lock(m_roomsMutex);
    auto it = m_rooms.find(roomId);
    if(it != m_rooms.end()) {
        return it->second;
    }
    return nullptr;
}

void Peer::addRoom(const std::shared_ptr<Room>& room)
{
  std::lock_guard<std::mutex> lock(m_roomsMutex);
  m_rooms[room->getId()] = room;
}


oatpp::async::CoroutineStarter Peer::handleMessage(oatpp::String messageData, const oatpp::Object<BaseMessageDto>& message){

    switch(*message->code){
        case MessageCodes::CODE_PEER_IS_TYPING:{
            auto newMessage = m_objectMapper->readFromString<oatpp::Object<ChatMessageDto>>(messageData);
            std::shared_ptr<Room> room = getRoom(newMessage->id);
            if (!room) 
                return nullptr;
            room->sendMessageAsync(newMessage);
            break;
            }
        case MessageCodes::CODE_PEER_MESSAGE: {
            auto newMessage = m_objectMapper->readFromString<oatpp::Object<ChatMessageDto>>(messageData);
            std::shared_ptr<Room> room = getRoom(newMessage->id);
            if (!room) {        
                room = lobby->getOrCreateRoom(mapMessageChatDto(newMessage));
                addRoom(room);
            }
            room->addHistoryMessage(newMessage);
            room->sendMessageAsync(newMessage);
            break;  
            }
        case MessageCodes::CODE_PEER_JOINED:
            break;  
        case MessageCodes::CODE_PEER_LEFT:
            break;
        case MessageCodes::CODE_PEER_MESSAGE_FILE:
            break;  
        case MessageCodes::CODE_FIND_ROOMS:
        {
            // auto newMessage = m_objectMapper->readFromString<oatpp::Object<MessageFind>>(messageData);
            // auto query = newMessage->query;
            // auto users = m_chatDao->getUserStartWithByNickname(query);
            // auto resultMessage = MessageFind::createShared();
            // resultMessage->query = query;
            // resultMessage->users = users;
            // resultMessage->peerId = m_peerId;
            // resultMessage->peerNickname = m_nickname;
            // sendMessageAsync(resultMessage);

        }
        }
    
    return nullptr;
}

oatpp::async::CoroutineStarter Peer::readMessage(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)  {

    if(size == 0) {

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.setCurrentPosition(0);

        oatpp::Object<BaseMessageDto> message;

        try {
            message = m_objectMapper->readFromString<oatpp::Object<BaseMessageDto>>(wholeMessage);
        } 
        catch (const std::runtime_error& e) {
            OATPP_ASSERT(false) ;
        }        
        message->peerId = m_peerId;
        message->timestamp = oatpp::base::Environment::getMicroTickCount();

        return handleMessage(wholeMessage, message);

    } 
    else if(size > 0) {
        m_messageBuffer.writeSimple(data, size);
    }

    return nullptr; 
}

oatpp::async::CoroutineStarter Peer::onClose(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) {

      return nullptr; 

}
oatpp::async::CoroutineStarter Peer::onPong(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message) {
        std::lock_guard<std::mutex> lock(m_test);

    --m_pingCounter;

    OATPP_LOGI("MyApp", "PONG");

    return nullptr;
}

oatpp::async::CoroutineStarter Peer::onPing(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message) {
    
    return oatpp::async::synchronize(&m_writeLock, socket->sendPongAsync(message));

}

oatpp::Int32 Peer::getPeerId() const{
    return m_peerId;
}

oatpp::String Peer::getPeerName() const {
    return m_nickname;
}


