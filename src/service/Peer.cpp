#include "Peer.hpp"

void Peer::sendMessageAsync(const oatpp::Object<MessageDto>& message){

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

oatpp::async::CoroutineStarter Peer::handleMessage(const oatpp::Object<MessageDto>& message){

    switch(*message->code){
        case MessageCodes::CODE_PEER_IS_TYPING:
            break;
        case MessageCodes::CODE_PEER_MESSAGE:
            break;
        case MessageCodes::CODE_PEER_JOINED:
            break;
        case MessageCodes::CODE_PEER_LEFT:
            break;
        case MessageCodes::CODE_PEER_MESSAGE_FILE:
            break;
        case MessageCodes::CODE_ONLINE:
            break;
        case MessageCodes::CODE_OFFLINE:
            break;

        
    }
    return nullptr;
}

oatpp::async::CoroutineStarter Peer::readMessage(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)  {

    if(size == 0) {

        auto wholeMessage = m_messageBuffer.toString();
        m_messageBuffer.setCurrentPosition(0);

        auto message = m_objectMapper->readFromString<oatpp::Object<MessageDto>>(wholeMessage);
        
        message->peerName = m_nickname;
        message->peerId = m_peerId;
        message->timestamp = oatpp::base::Environment::getMicroTickCount();
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

v_int64 Peer::getPeerId(){
    return m_peerId;
}

oatpp::String Peer::getPeerName(){
    return m_nickname;
}