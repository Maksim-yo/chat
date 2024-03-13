#ifndef __ROOMMANAGER_H__
#define __ROOMMANAGER_H__

#include "Room.hpp"
#include "oatpp/core/macro/component.hpp"
#include "service/dao/impl/postgres/ChatDao.hpp"
#include "utils/Timer.hpp"

class Peer;
class RoomManager
{
private:
    std::unordered_map<oatpp::Int32, std::unique_ptr<Room>> m_rooms;
    std::mutex m_roomsLock;

    OATPP_COMPONENT(std::shared_ptr<Postgres::ChatDao>, m_postgresChatDao);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);

public:
    RoomManager();
    Room* getOrCreateRoom(oatpp::Int32 roomId);
    std::vector<Room*> getOrCreateRooms(Peer* peer);
    void deleteRoom(oatpp::Int32 id);
    void deleteUnactiveRooms();
    void cleanUp();

};
#endif // __ROOMMANAGER_H__