#ifndef AppComponent_hpp
#define AppComponent_hpp

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"

#include "oatpp/core/macro/component.hpp"

#include "service/ConnectionHandler.hpp"
#include "service/interceptors/LoggerInterceptor.hpp"
#include "service/auth/AuthServiceBase.hpp"
#include "dto/ConfigDto.hpp"
#include "service/dao/ChatDAO.hpp"
class AppComponent {
public:


  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared({"localhost", 8000, oatpp::network::Address::IP_4});
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
    return std::make_shared<oatpp::async::Executor>();
  }());


  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper)([] {
    return oatpp::parser::json::mapping::ObjectMapper::createShared();
  }());


  OATPP_CREATE_COMPONENT(std::shared_ptr<ChatDao>, m_chatDao)([] {
    return std::make_shared<ChatDao>();
  }());



  OATPP_CREATE_COMPONENT(std::shared_ptr<AuthServiceBase>, m_authServiceBase)([] {
    return std::make_shared<AuthServiceBase>();
  }());


  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)("http", [] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); 
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor); 
    auto connectionHandler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
    std::shared_ptr<LoggerRequestInterceptor> requestInterceptor = std::make_shared<LoggerRequestInterceptor>();
    std::shared_ptr<LoggerResponseInterceptor> responseInterceptor = std::make_shared<LoggerResponseInterceptor>();
    connectionHandler->addRequestInterceptor(requestInterceptor);
    connectionHandler->addResponseInterceptor(responseInterceptor);

    return connectionHandler;
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler)("websocket", [] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);
    auto connectionHandler = oatpp::websocket::AsyncConnectionHandler::createShared(executor);
    connectionHandler->setSocketInstanceListener(std::make_shared<ConnectionHandler>());
    return connectionHandler;
  }());



};

#endif /* AppComponent_hpp */
