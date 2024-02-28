#include "RoomManager.hpp"
#include "Peer.hpp"

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
        m_roomsLock.unlock();

        rooms.push_back(room.get());
        room->peerJoin(peer);
    }
    return rooms;
}
