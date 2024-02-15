#ifndef __CHATDAO_H__
#define __CHATDAO_H__


#include "oatpp/core/macro/component.hpp"

#include "service/db/Database.hpp"

class ChatDao {
private:
    OATPP_COMPONENT(std::shared_ptr<Database>, m_db);
    
public:
    oatpp::Object<UserToken>                createUserToken(oatpp::Object<UserToken> token);
    oatpp::Object<UserDto>                  createUser(oatpp::Object<UserDto> user);
    std::optional<oatpp::Object<UserToken>> findToken(oatpp::String token);
    std::optional<oatpp::Object<UserDto>>   getUserById(oatpp::Int32 id);
    std::optional<oatpp::Object<UserToken>> getTokenByUserId(oatpp::Object<UserDto> user);
    std::optional<oatpp::Vector<oatpp::Object<PeerDto>>>   getPeersInChat(oatpp::Int32 id);
    std::optional<oatpp::Vector<oatpp::Object<ChatDto>>>   getUserConverstationsHisotry(oatpp::Int32 id);
    std::optional<oatpp::Object<ChatDto>>                  getChatById(oatpp::Int32 od);
    std::optional<oatpp::Vector<oatpp::Object<MessageDto>>> getUsersHistoryInChat(oatpp::Int32 chatId, oatpp::Int32 count = 100);
    std::optional<oatpp::Object<UserDto>>   getUserByLoginAndPassword(oatpp::String login, oatpp::String password);
    std::optional<oatpp::Vector<oatpp::Object<PeerDto>>>   getUserStartWithByNickname(oatpp::String name);
    void                                    appendMessage(const oatpp::Object<ChatMessageDto>& message);
    void                                    deleteExpiredTokens(int time = -1);
    void                                    deleteTokenByValue(oatpp::String token);
    
};



#endif // __CHATDAO_H__