#define NOMINMAX
#include "oatpp/network/Server.hpp"

#include "oatpp/core/macro/codegen.hpp"

#include "AppComponent.hpp"
#include "DatabaseComponent.hpp"
#include "controller/AuthController.hpp"
#include "controller/StaticController.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

void run()
{

    DatabaseComponent databaseComponent;
    AppComponent components;

    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    auto authController = std::make_shared<AuthController>();
    auto staticController = std::make_shared<StaticController>();
    router->addController(authController);
    router->addController(staticController);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler, "http");

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}

int main()
{

    oatpp::base::Environment::init();

    run();

    oatpp::base::Environment::destroy();

    return 0;
}