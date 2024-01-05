#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "oatpp-postgresql/orm.hpp"

#include "dto/UserDto.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

class Database: public oatpp::orm::DbClient {
public: 
  Database(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {

    oatpp::orm::SchemaMigration migration(executor);
    migration.addFile(1, DATABASE_MIGRATIONS "/001_init.sql");
    migration.migrate();
    auto version = executor->getSchemaVersion();
    OATPP_LOGD("Database", "Migration - OK. =%d.", version);

  }

QUERY(createUser, 
      "INSERT INTO chat_user"
      "(nickname, mail, password) VALUES"
      ":user.nickname, :user.mail, :user.password"
      "RETURNING *;",
      PREPARE(true), 
      PARAM(oatpp::Object<UserDto>, user))
    
QUERY(getUserByLogin,
      "SELECT * FROM chat_user WHERE mail=:login;",
      PREPARE(true), 
      PARAM(oatpp::String, login))

QUERY(getUserByLoginAndPassword,
      "SELECT * FROM chat_user WHERE mail=:login AND password=:password;",
      PREPARE(true),
      PARAM(oatpp::String, login),
      PARAM(oatpp::String, password))


QUERY(deleteUserByLogin,
      "DELETE FROM chat_user WHERE mail=:login;",
       PREPARE(true), 
       PARAM(oatpp::String, login))


};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen

#endif // __DATABASE_H__