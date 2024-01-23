
#ifndef ASYNC_SERVER_ROOMS_ConnectionHandler_HPP
#define ASYNC_SERVER_ROOMS_ConnectionHandler_HPP

#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "AbstractRoom.hpp"
#include "Room.hpp"
class Peer;
class Lobby : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener {
public:
  std::atomic<v_int32> m_peerIdCounter;
  std::unordered_map<oatpp::Int32, std::shared_ptr<Room>> m_rooms;
  std::unordered_map<oatpp::Int32, std::shared_ptr<Peer>> m_peers;
  std::mutex m_roomsMutex;
  std::mutex m_peersMutex;

public:

  Lobby()
    : m_peerIdCounter(0)
  {}

  // v_int32 obtainNewUserId();

public:

  std::shared_ptr<Room> getOrCreateRoom(oatpp::Object<ChatDto> chatDto);
  std::shared_ptr<Room> getRoom(oatpp::Int32 roomId);
  void onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) override;

  void onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) override;

  void onPing(Peer* peer);

  void peersLock(){
    m_peersMutex.lock();
  }

  void peersUnlock(){
    m_peersMutex.unlock();
  }

  const std::unordered_map<oatpp::Int32, std::shared_ptr<Peer>>& getPeers() const {
    return m_peers;
  }



};


#endif //ASYNC_SERVER_ROOMS_ConnectionHandler_HPP