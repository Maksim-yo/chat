#include "Lobby.hpp"
#include "Peer.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"


// v_int32 Lobby::obtainNewUserId() {
//   return m_peerIdCounter ++;
// }

std::shared_ptr<Room> Lobby::getOrCreateRoom(oatpp::Object<ChatDto> chatDto)
{
  std::lock_guard<std::mutex> lock(m_roomsMutex);
  std::shared_ptr<Room>& room = m_rooms[chatDto->id];
  if(!room) {
    room = std::make_shared<Room>(chatDto, this);
  }
  return room;

}

std::shared_ptr<Room> Lobby::getRoom(oatpp::Int32 roomId) {
  std::lock_guard<std::mutex> lock(m_roomsMutex);
  auto it = m_rooms.find(roomId); 
  if(it != m_rooms.end()) {
    return it->second;
  }
  return nullptr;
}

void Lobby::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) {

  std::lock_guard<std::mutex> mtx(m_peersMutex);  
  auto mail = params->find("userMail")->second;
  auto nickname = params->find("userNickname")->second;
  auto peerId = params->find("peerId")->second;
  bool success;
  auto peer = std::make_shared<Peer>(socket, oatpp::utils::conversion::strToInt32(peerId, success), nickname, this);
  socket->setListener(peer);
  m_peers[peer->getPeerId()] = peer;
  
}

void Lobby::onPing(Peer* perr){


}

void Lobby::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) {


  socket->setListener(nullptr);

}

