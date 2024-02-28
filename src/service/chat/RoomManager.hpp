#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

#include "oatpp/core/macro/component.hpp"
#include "service/dao/impl/postgres/ChatDao.hpp"
#include "Room.hpp"
class Peer;
class RoomManager
{
private:
    std::unordered_map<oatpp::Int32, std::unique_ptr<Room>> m_rooms;
    std::mutex m_roomsLock;

    OATPP_COMPONENT(std::shared_ptr<Postgres::ChatDao>, m_postgresChatDao);

public:
    Room* getOrCreateRoom(oatpp::Int32 roomId);
    std::vector<Room*> getOrCreateRooms(Peer* peer);
};
#endif // __ROOMMANAGER_H__