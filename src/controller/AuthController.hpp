
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

#include "oatpp-websocket/Handshaker.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <inja/inja.hpp>

#include "oatpp/network/Url.hpp"


#include OATPP_CODEGEN_BEGIN(ApiController)



class AuthController : public oatpp::web::server::api::ApiController {

private: 
  typedef AuthController __ControllerType;

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler, "websocket");

public:
  AuthController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {}

  

    ENDPOINT_ASYNC("POST", "accounts/registration", RegistrationPOST) {

    ENDPOINT_ASYNC_INIT(RegistrationPOST)
  
    Action act() override {
    auto headers = request->getHeaders().getAll();
    return request->readBodyToStringAsync().callbackTo(&RegistrationPOST::onGood);

  
    }

    Action onGood(const oatpp::String& result) {
      auto val = result.getValue("hello");
      OATPP_LOGD("MyCoroutine", "result='%s'", result);
      oatpp::String new_string = "?" + result;
      oatpp::network::Url::Parameters params = oatpp::network::Url::Parser::parseQueryParams(new_string);
      for (auto [k, v] : params.getAll())
      {
      auto key = k.toString().getValue("hello");
      auto value = v.toString().getValue("hello");
      std::cout << "hello";
      }
      auto response = controller->createResponse(Status::CODE_200, "Good");

      return _return(response);
    }

    };



    ENDPOINT_ASYNC("GET", "accounts/registration", RegistrationGET) {

    ENDPOINT_ASYNC_INIT(RegistrationGET)
  
     Action act() override {

      inja::Environment env;
      inja::json data;
      data["user"]["is_authenticated"] = false;
      inja::Template temp = env.parse_template("D:/code/c++/proj/chat/chat/front/html/registration.html");
      std::string result = env.render(temp, data); // "Hello world!"
      std::string _p = FRONT_PATH "html/registration.html";
      auto response = controller->createResponse(Status::CODE_200, result);
      response->putHeader(Header::CONTENT_TYPE, "text/html");
      return _return(response);
     }
    };

    ENDPOINT_ASYNC("GET", "ws/", WS) {

    ENDPOINT_ASYNC_INIT(WS)

     Action act() override {

      auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(), controller->websocketConnectionHandler);
      auto parameters = std::make_shared<oatpp::network::ConnectionHandler::ParameterMap>();
      response->setConnectionUpgradeParameters(parameters);
      return _return(response);

     }

   };



};


#include OATPP_CODEGEN_END(ApiController)
