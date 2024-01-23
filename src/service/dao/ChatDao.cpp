
#include <algorithm>   

#include "ChatDao.hpp"
#include "utils/utils.hpp"
#include "service/db/DOs.hpp"

namespace {


    oatpp::Object<ChatDto> mapChatDo(oatpp::Object<ChatExtendedDo> msg){

        auto chatDto = ChatDto::createShared();
        auto peerDto = PeerDto::createShared();
        peerDto->peerId = msg->user_id;
        peerDto->peerName = msg->nickname;
        chatDto->peers = {peerDto};
        chatDto->id = msg->id;
        return chatDto;
    }


    oatpp::Object<MessageDto> mapMessageDo(oatpp::Object<MessageDo> msg){

        auto messageDto = MessageDto::createShared();
        messageDto->peerId = msg->user_id;
        messageDto->timestamp = msg->created_at;
        messageDto->message = msg->line_text;
        messageDto->code = MessageCodes::CODE_PEER_MESSAGE;
        return messageDto;
        }

}


std::optional<oatpp::Object<UserToken>> ChatDao::findToken(oatpp::String token)
{
    auto tokenQuery = m_db->findToken(token);
    OATPP_ASSERT(tokenQuery->isSuccess());
    if (!tokenQuery->hasMoreToFetch())
        return std::nullopt;
    return tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>()[0];

}

void ChatDao::deleteTokenByValue(oatpp::String token)
{
    m_db->deleteTokenByValue(token);

}

std::optional<oatpp::Object<UserDto>> ChatDao::getUserById(oatpp::Int32 id)
{
    auto userQuery = m_db->getUserById(id);
    OATPP_ASSERT(userQuery->isSuccess());
    if (!userQuery->hasMoreToFetch())
        return std::nullopt;
    return userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];

}

std::optional<oatpp::Object<UserToken>> ChatDao::getTokenByUserId(oatpp::Object<UserDto> user)
{
    auto tokenQuery = m_db->findTokenByUser(user->id);
    OATPP_ASSERT(tokenQuery->isSuccess());
    if (!tokenQuery->hasMoreToFetch())
        return std::nullopt;
    return tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>()[0];
}

std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> ChatDao::getPeersInChat(oatpp::Int32 id)
{
    auto peersQuery = m_db->getPeersInChat(id);
    OATPP_ASSERT(peersQuery->isSuccess());
    if (!peersQuery->hasMoreToFetch())
        return std::nullopt;
    return peersQuery->fetch<oatpp::Vector<oatpp::Object<PeerDto>>>();
}

std::optional<oatpp::Vector<oatpp::Object<ChatDto>>> ChatDao::getUserConverstationsHisotry(oatpp::Int32 id)
{
    auto dbResult = m_db->executeQuery(
    "SELECT chat.id, app_user.id as user_id, app_user.nickname FROM chat JOIN app_user ON chat.user_first = app_user.id or chat.user_second = app_user.id WHERE (chat.user_first = :userId or chat.user_second = :userId) and app_user.id != :userId;",{{"userId", oatpp::Int32(id)}});
    OATPP_ASSERT(dbResult->isSuccess());
    if (!dbResult->hasMoreToFetch())
        return std::nullopt;
    oatpp::Vector<oatpp::Object<ChatDto>> converstationsResult({});
    auto converstationHistory = dbResult->fetch<oatpp::Vector<oatpp::Object<ChatExtendedDo>>>();
    for(auto chatIter = converstationHistory->begin(); chatIter != converstationHistory->end(); chatIter++){
        auto messages = getUsersHistoryInChat((*chatIter)->id);
        auto userConverstaionDto = mapChatDo(*chatIter);
        auto peers = getPeersInChat((*chatIter)->id);
        if (peers.has_value())
            userConverstaionDto->peers = peers.value();
        if (messages.has_value())
            userConverstaionDto->history = {messages.value()};
        converstationsResult->push_back(userConverstaionDto);

    }
    return converstationsResult;
}

std::optional<oatpp::Vector<oatpp::Object<MessageDto>>> ChatDao::getUsersHistoryInChat(oatpp::Int32 chatId, oatpp::Int32 count)
{
    auto historyQuery = m_db->getUsersHistoryInChat(chatId, count);
    OATPP_ASSERT(historyQuery->isSuccess());
    if (!historyQuery->hasMoreToFetch())
        return std::nullopt;
    auto objects = historyQuery->fetch<oatpp::Vector<oatpp::Object<MessageDo>>>();
    oatpp::Vector<oatpp::Object<MessageDto>> resultMessages({});
    auto temp = resultMessages.get();
    for(auto it =  objects->begin(); it != objects->end(); it++)
        resultMessages->push_back(mapMessageDo(*it));
    return resultMessages;
}

std::optional<oatpp::Object<UserDto>> ChatDao::getUserByLoginAndPassword(oatpp::String login, oatpp::String password)
{
    auto userQuery = m_db->getUserByLoginAndPassword(login, password);
    OATPP_ASSERT(userQuery->isSuccess());
    if (!userQuery->hasMoreToFetch())
        return std::nullopt;
    return userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];    
}

std::optional<oatpp::Vector<oatpp::Object<PeerDto>>> ChatDao::getUserStartWithByNickname(oatpp::String name)
{
    auto userQuery = m_db->getUserStartWithByNickname(name);
    OATPP_ASSERT(userQuery->isSuccess());
    if (!userQuery->hasMoreToFetch())
        return std::nullopt;
    auto userResult = userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
    oatpp::Vector<oatpp::Object<PeerDto>> result;
    std::transform(userResult->begin(), userResult->end(), result->begin(), 
    [](oatpp::Object<UserDto> user){
        auto peer = PeerDto::createShared();
        peer->peerId = user->id;
        peer->peerName = user->nickname;
        return peer;
    });
    return result;    
}

void ChatDao::appendMessage(const oatpp::Object<ChatMessageDto>& message)
{
    auto messageQuery = m_db->createChatMessage(message);
    OATPP_ASSERT(messageQuery->isSuccess());
    return;
}

void ChatDao::deleteExpiredTokens(int time)
{
    if (time == -1)
        m_db->deleteExpiredTokens(Utils::getCurrentTimeInSeconds());
    else 
        m_db->deleteExpiredTokens(time);

}

oatpp::Object<UserToken> ChatDao::createUserToken(oatpp::Object<UserToken> token)
{
    auto tokenQuery = m_db->createUserToken(token);
    OATPP_ASSERT(tokenQuery->isSuccess());
    return tokenQuery->fetch<oatpp::Vector<oatpp::Object<UserToken>>>()[0];
}

oatpp::Object<UserDto> ChatDao::createUser(oatpp::Object<UserDto> user)
{
    auto userQuery = m_db->createUser(user);
    OATPP_ASSERT(userQuery->isSuccess())
    return userQuery->fetch<oatpp::Vector<oatpp::Object<UserDto>>>()[0];

}
