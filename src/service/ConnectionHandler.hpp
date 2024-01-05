
#ifndef ASYNC_SERVER_ROOMS_ConnectionHandler_HPP
#define ASYNC_SERVER_ROOMS_ConnectionHandler_HPP

#include "oatpp-websocket/AsyncConnectionHandler.hpp"

class Peer;
class ConnectionHandler : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener {
public:
  std::atomic<v_int32> m_userIdCounter;
  std::mutex m_roomsMutex;
public:

  ConnectionHandler()
    : m_userIdCounter(0)
  {}

  v_int32 obtainNewUserId();

public:

  /**
   *  Called when socket is created
   */
  void onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) override;

  /**
   *  Called before socket instance is destroyed.
   */
  void onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) override;

  void onPing(Peer* peer);

};


#endif //ASYNC_SERVER_ROOMS_ConnectionHandler_HPP