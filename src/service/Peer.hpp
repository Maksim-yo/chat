#ifndef PEER_HPP
#define PEER_HPP

#include "oatpp-websocket/AsyncWebSocket.hpp"

#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/core/async/Lock.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include "dto/DTOs.hpp"


class Peer: public oatpp::websocket::AsyncWebSocket::Listener {

protected:

    class WaitListListener : public oatpp::async::CoroutineWaitList::Listener {
    private:
      Peer* m_subscriber;
    public:

      WaitListListener(Peer* subscriber)
        : m_subscriber(subscriber)
      {}

      void onNewItem(oatpp::async::CoroutineWaitList& list) override {
        std::lock_guard<std::mutex> lock(m_subscriber->m_test);

        const int temp = 0;
        if (m_subscriber->m_pingCounter == temp)
          OATPP_LOGI("MyApp", "WAIT NOTYIFY ALL");
          list.notifyAll();
        }

    };
    oatpp::data::stream::BufferOutputStream m_messageBuffer;
    std::shared_ptr<oatpp::websocket::AsyncWebSocket> m_socket;

    oatpp::String m_nickname;
    v_int64 m_peerId;

    std::atomic<v_int32> m_pingCounter;

    std::mutex m_test;
    //socket sync
    oatpp::async::Lock m_writeLock;           

    oatpp::async::CoroutineWaitList m_waitList;
    WaitListListener m_waitListener;

    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);

   


public:
    Peer(std::shared_ptr<AsyncWebSocket> socket, v_int64 peerId, oatpp::String nickname)
        : m_socket(socket),  m_peerId(peerId), m_nickname(nickname), m_waitListener(this) 
        {
              m_waitList.setListener(&m_waitListener);
        }


    void sendMessageAsync(const oatpp::Object<MessageDto>& message);
    void sendPingAsyncWait();
    void sendPingAsync();

    oatpp::async::CoroutineStarter handleMessage(const oatpp::Object<MessageDto>& message);
    v_int64 getPeerId();
    oatpp::String getPeerName();
    CoroutineStarter readMessage(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
    CoroutineStarter onClose(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) override;
    CoroutineStarter onPong(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message) override;
    CoroutineStarter onPing(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message) override;

    ~Peer() {}
};

#endif 