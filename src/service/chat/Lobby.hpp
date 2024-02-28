
#ifndef LOBBY_HPP
#define LOBBY_HPP

#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp/core/macro/component.hpp"

#include "RoomManager.hpp"

class Peer;
class Lobby : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener
{
private:
private:
    std::shared_ptr<RoomManager> m_roomManager;

public:
    Lobby();

public:
    void onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) override;
    void onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) override;
};

#endif // LOBBY_HPP