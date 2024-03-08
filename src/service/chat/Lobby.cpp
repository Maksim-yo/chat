#include "Lobby.hpp"
#include "Peer.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "service/logging/ILogger.hpp"

void Lobby::onAfterCreate_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket, const std::shared_ptr<const ParameterMap>& params)
{
    class PingListener : public oatpp::async::Coroutine<PingListener>
    {
    private:
        const std::weak_ptr<Peer> m_peer;
        int peerSessionId;

    public:
        PingListener(const std::weak_ptr<Peer>& peer, int sessionId) : m_peer(peer), peerSessionId(sessionId)
        {
            OATPP_LOGD("MyApp", "Ping created. Peer session id: %d", peerSessionId);
        }
        Action act() override
        {
            if (auto p = m_peer.lock())
                return p->sendPingAsyncScheduling().next(finish());
            return finish();
        }

        ~PingListener()
        {
            OATPP_LOGD("MyApp", "Ping destroyed. Peer session id: %d", peerSessionId);
        }
    };

    auto startPinging = [&](std::weak_ptr<Peer>& peer, int peerSessionId) {
        m_asyncExecutor->execute<PingListener>(peer, peerSessionId);
    };
    auto mail = params->find("userMail")->second;
    auto nickname = params->find("userNickname")->second;
    auto peerId = params->find("peerId")->second;
    bool success;
    auto peer = std::make_shared<Peer>(socket, oatpp::utils::conversion::strToInt32(peerId, success), nickname, m_roomManager);
    std::weak_ptr<Peer> p = peer;
    socket->setListener(peer);
    startPinging(p, peer->getSessionId());
}

void Lobby::onBeforeDestroy_NonBlocking(const std::shared_ptr<AsyncWebSocket>& socket)
{
    socket->setListener(nullptr);
}

Lobby::Lobby() : m_roomManager(std::make_shared<RoomManager>())
{
}
