#ifndef PEER_HPP
#define PEER_HPP

#include "oatpp-websocket/AsyncWebSocket.hpp"

#include "oatpp/network/ConnectionProvider.hpp"

#include "oatpp/core/async/Lock.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include "dto/DTOs.hpp"
#include "Lobby.hpp"
#include "Room.hpp"
#include "dao/ChatDao.hpp"



#define MSG(msg) \
m_objectMapper->writeToString(msg)

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
    oatpp::Int32 m_peerId;
     Lobby* lobby;

    std::unordered_map<oatpp::Int32, std::shared_ptr<Room>> m_rooms;    // key - id of peer
    std::mutex m_roomsMutex;
    std::atomic<v_int32> m_pingCounter;

    std::mutex m_test;
    //socket sync
    oatpp::async::Lock m_writeLock;           

    oatpp::async::CoroutineWaitList m_waitList;
    WaitListListener m_waitListener;

    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);
    OATPP_COMPONENT(std::shared_ptr<ChatDao>, m_chatDao);

   


public:
    Peer(std::shared_ptr<AsyncWebSocket> socket, oatpp::Int32 peerId, oatpp::String nickname,  Lobby* lobby);   

    void sendMessageAsync(const oatpp::String& message);
    void sendPingAsyncWait();
    void sendPingAsync();

    std::shared_ptr<Room> getRoom(oatpp::Int32 roomId);
    void addRoom(const std::shared_ptr<Room>& room);


    oatpp::async::CoroutineStarter handleMessage(oatpp::String messageData, const oatpp::Object<BaseMessageDto>& message);
    oatpp::Int32 getPeerId() const;
    oatpp::String getPeerName() const;
    CoroutineStarter readMessage(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
    CoroutineStarter onClose(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, v_uint16 code, const oatpp::String& message) override;
    CoroutineStarter onPong(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message) override;
    CoroutineStarter onPing(const std::shared_ptr<oatpp::websocket::AsyncWebSocket>& socket, const oatpp::String& message) override;

    ~Peer() {}
};

#endif 