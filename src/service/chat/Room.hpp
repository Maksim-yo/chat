#ifndef ROOM_HPP
#define ROOM_HPP

#include "oatpp/core/async/Executor.hpp"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/component.hpp"

#include <chrono>
#include <unordered_map>

#include "dto/DTOs.hpp"
#include "service/dao/impl/postgres/ChatDao.hpp"

class Peer;
class Room
{
private:
private:
    oatpp::Int32 m_id;
    oatpp::Vector<oatpp::Object<MessageDto>> m_messageHistory;
    oatpp::Vector<oatpp::Object<MessageDto>> m_changedMessages;
    std::vector<Peer*> m_peers;

    int32_t maxMessagesForSave = 100;
    int32_t m_newMessagesdIndex;
    int32_t m_currentMessagesCount;
    oatpp::Vector<oatpp::Object<PeerDto>> m_users;

    std::mutex m_historyLock;
    std::mutex m_changedMessagesLock;
    std::mutex m_usersLock;
    std::mutex m_peersLock;
    oatpp::async::CoroutineWaitList m_dbWaitList;

    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);
    OATPP_COMPONENT(std::shared_ptr<Postgres::ChatDao>, m_postgresChatDao);
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, m_asyncExecutor);

public:
    Room(oatpp::Object<ChatDto> chatDto);
    ~Room();
    oatpp::Int32 getId();
    void sendMessageAsync(const oatpp::String& message);
    void markMessagesAsRead(int count);
    void addHistoryMessage(const oatpp::Object<MessageDto>& message);
    oatpp::Object<ChatDto> getChatDto();
    void peerJoin(Peer* peer);
    void peerLeft(Peer* peer);
    void saveData();
};
#endif