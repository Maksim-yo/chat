#include "RoomManager.hpp"
#include "Peer.hpp"

RoomManager::RoomManager()
{
    cleanUp();
}

void RoomManager::cleanUp()
{
    class CleanUp : public oatpp::async::Coroutine<CleanUp>
    {
    private:
        RoomManager* m_roomManager;

    public:
        CleanUp(RoomManager* roomManager) : m_roomManager(roomManager) {}
        Action act() override
        {
            OATPP_LOGD("RoomManager", "Clean up started")
            m_roomManager->deleteUnactiveRooms();
            return waitRepeat(std::chrono::seconds(60));
        }
    };

    m_asyncExecutor->execute<CleanUp>(this);
}
Room* RoomManager::getOrCreateRoom(oatpp::Int32 roomId)
{
    std::lock_guard<std::mutex> lock(m_roomsLock);
    std::unique_ptr<Room>& room = m_rooms[roomId];
    if (!room) {
        auto chat = m_postgresChatDao->getChatById(roomId);
        OATPP_ASSERT(chat.has_value());
        room = std::make_unique<Room>(chat.value());
    }
    return room.get();
}

std::vector<Room*> RoomManager::getOrCreateRooms(Peer* peer)
{
    std::vector<Room*> rooms;
    auto converstationsHistory = m_postgresChatDao->getUserConverstationsHisotry(peer->getPeerId());
    if (!converstationsHistory.has_value())
        return {};

    for (auto it = converstationsHistory.value()->begin(); it != converstationsHistory.value()->end(); it++) {

        m_roomsLock.lock();

        std::unique_ptr<Room>& room = m_rooms[(*it)->id];
        if (!room)
            m_rooms[(*it)->id] = std::make_unique<Room>(*it);

        rooms.push_back(room.get());
        room->peerJoin(peer);
        m_roomsLock.unlock();
    }
    return rooms;
}

void RoomManager::deleteRoom(oatpp::Int32 roomId)
{
    std::lock_guard<std::mutex> lock(m_roomsLock);
    auto it = m_rooms.find(roomId);
    if (it != m_rooms.end()) {
        it->second.reset(nullptr);
        m_rooms.erase(it);
    }
}

void RoomManager::deleteUnactiveRooms()
{
    std::lock_guard<std::mutex> lock(m_roomsLock);
    const auto count = std::erase_if(m_rooms, [](const auto& item) {
        auto const& [room_id, room] = item;
        return room->shouldDestroy();
    });
    OATPP_LOGD("RoomManager", "Cleanup delete : %d", count);
}
