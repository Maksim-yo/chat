#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/core/macro/component.hpp"

#include "dto/ConfigDto.hpp"
#include "service/chat/Lobby.hpp"
#include "service/auth/AuthServiceBase.hpp"
#include "service/dao/interfaces/IChatDao.hpp"
#include "service/dao/interfaces/ITokenDao.hpp"
#include "service/dao/interfaces/IUserDao.hpp"
#include "service/dao/impl/postgres/ChatDao.hpp"
#include "service/dao/impl/postgres/TokenDao.hpp"
#include "service/dao/impl/postgres/UserDao.hpp"

#include "service/interceptors/LoggerInterceptor.hpp"
class AppComponent
{
public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)
    ([] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8000, oatpp::network::Address::IP_4});
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)
    ([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)
    ([] {
        return std::make_shared<oatpp::async::Executor>();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)
    ([] {
        return oatpp::parser::json::mapping::ObjectMapper::createShared();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<Postgres::ChatDao>, m_postgresChatDao)
    ([] {
        return std::make_shared<Postgres::ChatDao>();
    }());
    
    OATPP_CREATE_COMPONENT(std::shared_ptr<Postgres::TokenDao>, m_postgresTokenDao)
    ([] {
        return std::make_shared<Postgres::TokenDao>();
    }());
    
    OATPP_CREATE_COMPONENT(std::shared_ptr<Postgres::UserDao>, m_postgresUserDao)
    ([] {
        return std::make_shared<Postgres::UserDao>();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<AuthServiceBase>, m_authServiceBase)
    ([] {
        return std::make_shared<AuthServiceBase>();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)
    ("http", [] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
        auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
        std::shared_ptr<LoggerRequestInterceptor> requestInterceptor = std::make_shared<LoggerRequestInterceptor>();
        std::shared_ptr<LoggerResponseInterceptor> responseInterceptor = std::make_shared<LoggerResponseInterceptor>();
        connectionHandler->addRequestInterceptor(requestInterceptor);
        connectionHandler->addResponseInterceptor(responseInterceptor);

        return connectionHandler;
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)
    ("websocket", [] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
        auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
        connectionHandler->setSocketInstanceListener(std::make_shared<Lobby>());
        return connectionHandler;
    }());
};

#endif /* AppComponent_hpp */
