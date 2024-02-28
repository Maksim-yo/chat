#include "Lobby.hpp"
#include "Peer.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

void Lobby::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params)
{
    auto mail = params->find("userMail")->second;
    auto nickname = params->find("userNickname")->second;
    auto peerId = params->find("peerId")->second;
    bool success;
    auto peer = std::make_shared<Peer>(socket, oatpp::utils::conversion::strToInt32(peerId, success), nickname, m_roomManager);
    socket->setListener(peer);
}

void Lobby::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket)
{
    socket->setListener(nullptr);
}

Lobby::Lobby() : m_roomManager(std::make_shared<RoomManager>())
{
}
