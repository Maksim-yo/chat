#ifndef __DATABASECOMPONENT_H__
#define __DATABASECOMPONENT_H__

#include <iostream>
#include <stdlib.h>
#include <string>

#include "dto/ConfigDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "service/db/Database.hpp"
#include "utils/utils.hpp"

class DatabaseComponent
{
public:
    OATPP_CREATE_COMPONENT(oatpp::Object<ConfigDto>, config)
    ([this] {
        const char* configPath = CONFIG_PATH;
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        try {
            oatpp::String configText = oatpp::String::loadFromFile(configPath);
            auto profile = objectMapper->readFromString<oatpp::Object<ConfigDto>>(configText);
            profile->rootStaticPath = STATIC_PATH;
            return profile;

        } catch (std::exception& err) {
            std::string host = Utils::getEnvVariable("CHAT_HOST", "127.0.0.1");
            std::string host_port = Utils::getEnvVariable("CHAT_PORT", "8000");
            std::string db_password = Utils::getEnvVariable("CHAT_DB_PASSWORD", "postgres");
            std::string db_name = Utils::getEnvVariable("CHAT_DB_NAME", "postgres");
            std::string db_user = Utils::getEnvVariable("CHAT_DB_USER", "postgres");
            std::string db_host = Utils::getEnvVariable("CHAT_DB_HOST", "127.0.0.1");
            std::string db_port = Utils::getEnvVariable("CHAT_DB_PORT", "5432");

            auto profile = ConfigDto::createShared();
            profile->host = host;
            profile->port = atoi(host_port.c_str());
            profile->dbConnectionString = "postgresql://" + std::string(db_user) + ":" + std::string(db_password) + "@" + std::string(db_host) + ":" + db_port + "/" + std::string(db_name);
            profile->rootStaticPath = STATIC_PATH;
            return profile;
        }
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<Database>, m_database)
    ([] {
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);
        OATPP_LOGE("AppComponent", "Connecting to database: '%s'", config->dbConnectionString->c_str());

        auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>(config->dbConnectionString);

        auto connectionPool = oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
                                                                              10,
                                                                              std::chrono::seconds(5));

        auto executor = std::make_shared<oatpp::postgresql::Executor>(connectionPool);

        return std::make_shared<Database>(executor);
    }());
};

#endif // __DATABASECOMPONENT_H__