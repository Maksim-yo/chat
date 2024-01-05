#ifndef __DATABASECOMPONENT_H__
#define __DATABASECOMPONENT_H__

#include "service/db/Database.hpp"

class DatabaseComponent {
public:


  OATPP_CREATE_COMPONENT(std::shared_ptr<Database>, m_database)([] {

    const char* dbConnectionString = std::getenv("dbConnectionString"); 
    auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>(dbConnectionString);

    auto connectionPool = oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
                                                                          10,
                                                                          std::chrono::seconds(5));

    auto executor = std::make_shared<oatpp::postgresql::Executor>(connectionPool);

    return std::make_shared<Database>(executor);

  }());


};


#endif // __DATABASECOMPONENT_H__