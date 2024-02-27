#ifndef __DATABASECOMPONENT_H__
#define __DATABASECOMPONENT_H__

#include "dto/ConfigDto.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "service/db/Database.hpp"

class DatabaseComponent
{
public:
    OATPP_CREATE_COMPONENT(oatpp::Object<ConfigDto>, config)
    ([this] {
        const char* configPath = CONFIG_PATH;
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();

        oatpp::String configText = oatpp::String::loadFromFile(configPath);
        if (configText) {

            auto profiles = objectMapper->readFromString<oatpp::Fields<oatpp::Object<ConfigDto>>>(configText);
            auto profile = profiles.getValueByKey("default");
            if (!profile)
                throw std::runtime_error("No configuration profile found. Server won't run.");
            profile->rootStaticPath = STATIC_PATH;
            return profile;
        }
        OATPP_LOGE("AppComponent", "Can't load configuration file at path '%s'", configPath);
        throw std::runtime_error("[AppComponent]: Can't load configuration file");
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<Database>, m_database)
    ([] {
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

        auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>(config->dbConnectionString);

        auto connectionPool = oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
                                                                              10,
                                                                              std::chrono::seconds(5));

        auto executor = std::make_shared<oatpp::postgresql::Executor>(connectionPool);

        return std::make_shared<Database>(executor);
    }());
};

#endif // __DATABASECOMPONENT_H__