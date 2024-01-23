#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "oatpp-postgresql/orm.hpp"
#include "oatpp/core/Types.hpp"

#include "dto/UserDto.hpp"
#include "dto/DTOs.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

class Database: public oatpp::orm::DbClient {
public: 
  Database(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {

    oatpp::orm::SchemaMigration migration(executor);
    // @ts-ignore
    migration.addFile(1, DATABASE_MIGRATIONS "/001_init.sql");
    migration.migrate();
    
    auto version = executor->getSchemaVersion();
    OATPP_LOGD("Database", "Migration - OK. =%d.", version);

  }

QUERY(createUser, 
      "INSERT INTO app_user "
      "(nickname, mail, password) VALUES "
      "(:user.nickname, :user.mail, :user.password) "
      "RETURNING *;",
      PREPARE(true), 
      PARAM(oatpp::Object<UserDto>, user))
    
QUERY(getUserByLogin,
      "SELECT * FROM app_user WHERE mail=:login",
      PARAM(oatpp::String, login))

QUERY(getUserByLoginAndPassword,
      "SELECT * FROM app_user WHERE mail=:login AND password=:password;",
      PREPARE(true),
      PARAM(oatpp::String, login),
      PARAM(oatpp::String, password))

QUERY(getUserById,
      "SELECT * FROM app_user WHERE id=:id;",
      PREPARE(true),
      PARAM(oatpp::Int32, id))

QUERY(getUserStartWithByNickname, 
      "SELECT id, nickname FROM app_user WHERE nickname LIKE :query",
      PREPARE(true),
      PARAM(oatpp::String, query))

QUERY(getUsersHistoryInChat, 
      "SELECT * FROM chat_line WHERE chat_id=:chat_id ORDER BY created_at DESC LIMIT :count",
      PREPARE(true),
      PARAM(oatpp::Int32, chat_id),
      PARAM(oatpp::Int32, count))

QUERY(getPeersInChat, 
      "SELECT app_user.id as \"peerId\", app_user.nickname as \"peerName\" FROM chat JOIN app_user ON chat.user_first = app_user.id or chat.user_second = app_user.id WHERE chat.id = :chat_id;",
      PREPARE(true),
      PARAM(oatpp::Int32, chat_id))

QUERY(deleteUserByLogin,
      "DELETE FROM app_user WHERE mail=:login;",
       PREPARE(true), 
       PARAM(oatpp::String, login))

QUERY(createUserToken,
      "INSERT INTO user_token "
      "(id, token, expiry) VALUES "
      "(:token.id, :token.token, :token.expiry) "
      "RETURNING *;",
      PREPARE(true),
      PARAM(oatpp::Object<UserToken>, token))
      
QUERY(findToken,
      "SELECT * FROM user_token WHERE token=:token;",
      PREPARE(true),
      PARAM(oatpp::String, token))

QUERY(findTokenByUser,
      "SELECT * FROM user_token WHERE id=:id;",
      PREPARE(true),
      PARAM(oatpp::Int32, id))

QUERY(deleteTokenByValue,
      "DELETE FROM user_token WHERE token=:token;",
      PREPARE(true),
      PARAM(oatpp::String, token))

QUERY(deleteExpiredTokens, 
      "DELETE FROM user_token WHERE expiry<=:expiry;",
      PREPARE(true),
      PARAM(oatpp::Int32, expiry))

QUERY(getUserConverstationsHisotry,
      "SELECT * FROM chat WHERE user_first=:user_id OR user_second=:user_id;",
      PREPARE(true),
      PARAM(oatpp::Int32, expiry))    
QUERY(createChatMessage,
      "INSERT INTO chat_line (chat_id, user_id, line_text, created_at) VALUES (msg.id, msg.peerId, msg.message.message, msg.timestamp);",
      PREPARE(true),
      PARAM(oatpp::Object<ChatMessageDto>, msg))
};



#include OATPP_CODEGEN_END(DbClient) //<- End Codegen

#endif // __DATABASE_H__