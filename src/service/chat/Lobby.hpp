
#ifndef LOBBY_HPP
#define LOBBY_HPP

#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp/core/macro/component.hpp"

#include "service/dao/impl/postgres/ChatDao.hpp"
#include "Room.hpp"
class Peer;
class Lobby : public oatpp::websocket::AsyncConnectionHandler::SocketInstanceListener
{
public:
    std::atomic<v_int32> m_peerIdCounter;
    std::unordered_map<oatpp::Int32, std::shared_ptr<Room>> m_rooms;
    std::unordered_map<oatpp::Int32, std::shared_ptr<Peer>> m_peers;
    std::mutex m_roomsMutex;
    std::mutex m_peersMutex;

private:
    OATPP_COMPONENT(std::shared_ptr<Postgres::ChatDao>, m_postgresChatDao);

public:
    Lobby()
        : m_peerIdCounter(0)
    {
    }

public:
    std::shared_ptr<Room> getOrCreateRoom(oatpp::Int32 chat_id);
    std::shared_ptr<Room> getRoom(oatpp::Int32 roomId);
    void addRoom(std::shared_ptr<Room> room);
    void onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params) override;

    void onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket) override;

    void onPing(Peer* peer);

    void peersLock()
    {
        m_peersMutex.lock();
    }

    void peersUnlock()
    {
        m_peersMutex.unlock();
    }

    const std::unordered_map<oatpp::Int32, std::shared_ptr<Peer>>& getPeers() const
    {
        return m_peers;
    }
};

#endif // LOBBY_HPP