#include "ConnectionHandler.hpp"
#include "Peer.hpp"


v_int32 ConnectionHandler::obtainNewUserId() {
  return m_userIdCounter ++;
}

void ConnectionHandler::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) {

  std::lock_guard<std::mutex> mtx(m_roomsMutex); 
  OATPP_LOGI("MyApp", "-------------------- NEW CONNECTION ----------------------");

  oatpp::String temp = "hello";
  auto peer = std::make_shared<Peer>(socket, obtainNewUserId(), temp);
  socket->setListener(peer);
  
}

void ConnectionHandler::onPing(Peer* perr){


}

void ConnectionHandler::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) {


  socket->setListener(nullptr);

}