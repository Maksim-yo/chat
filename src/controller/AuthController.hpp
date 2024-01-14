#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

#include "oatpp-websocket/Handshaker.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/network/Url.hpp"

#include <inja/inja.hpp>



#include "dto/UserDto.hpp"
#include "service/auth/AuthServiceBase.hpp"
#include "service/auth/AuthTokenService.hpp"

using namespace oatpp::web::server::handler;
namespace http = oatpp::web::protocol::http;

#include OATPP_CODEGEN_BEGIN(ApiController)



class AuthController : public oatpp::web::server::api::ApiController {

private: 
  typedef AuthController __ControllerType;

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler, "websocket");
  OATPP_COMPONENT(std::shared_ptr<AuthServiceBase>, m_authServiceBase);
  OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);
private:
  void setTokenCookie(const oatpp::Object<UserToken>& token, const std::shared_ptr<ApiController::OutgoingResponse>& response){
    
        std::string params = AuthTokenService::TOKEN + "=" + token->token;
        params += ";Max-Age=" + std::to_string(token->expiry)  + ";Domain=" + config->host + ";HttpOnly; Path=/";
        
        response->putHeader("Set-Cookie", params);

  }
public:
  AuthController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {


  }

    ENDPOINT_ASYNC("POST", "accounts/registration", RegistrationPOST)
   {

      ENDPOINT_ASYNC_INIT(RegistrationPOST)


      Action act() override {
        return request->readBodyToStringAsync().callbackTo(&RegistrationPOST::onGood);
      }

      Action onGood(const oatpp::String& result) {
        oatpp::String new_string = "?" + result;
        oatpp::network::Url::Parameters params = oatpp::network::Url::Parser::parseQueryParams(new_string);

        auto user = UserDto::createShared();
        user->nickname = params.get("nickname");
        user->mail = params.get("mail");
        user->password = params.get("password1");
        
        auto token = controller->m_authServiceBase->registration(user);
        if (!token.has_value())
          return _return(controller->createResponse(Status::CODE_501, "Bad try"));  
        OATPP_LOGD("AuthServiceBase", "User registration - OK");
        auto response = controller->createResponse(Status::CODE_200, "Good");
        controller->setTokenCookie(token.value(), response);
        return _return(response);

      }

    };

    ENDPOINT_ASYNC("GET", "accounts/registration", RegistrationGET)
    {

    ENDPOINT_ASYNC_INIT(RegistrationGET)
  

     Action act() override {

      inja::Environment env;
      inja::json data;
      data["user"]["is_authenticated"] = false;
      inja::Template temp = env.parse_template(FRONT_PATH "html/registration.html");
      std::string result = env.render(temp, data);
      auto response = controller->createResponse(Status::CODE_200, result);
      response->putHeader(Header::CONTENT_TYPE, "text/html");
      return _return(response);
     }
    };

    ENDPOINT_ASYNC("GET", "accounts/auth", AuthGet) {

    ENDPOINT_ASYNC_INIT(AuthGet)

     Action act() override {

          inja::Environment env;
          inja::json data;
          data["user"]["is_authenticated"] = false;
          inja::Template temp = env.parse_template(FRONT_PATH "html/login.html");
          std::string result = env.render(temp, data); 
          auto response = controller->createResponse(Status::CODE_200, result);
          response->putHeader(Header::CONTENT_TYPE, "text/html");
          return _return(response);

      }
    };

    ENDPOINT_ASYNC("POST", "accounts/auth", AuthPOST){

      ENDPOINT_ASYNC_INIT(AuthPOST)

      Action act() override {
        return request->readBodyToStringAsync().callbackTo(&AuthPOST::onGood);
      }

      Action onGood(const oatpp::String& result) {

        oatpp::String new_string = "?" + result;
        oatpp::network::Url::Parameters params = oatpp::network::Url::Parser::parseQueryParams(new_string);
        auto mail = params.get("mail");
        auto password = params.get("password");
        auto token = controller->m_authServiceBase->authentication(mail, password);
        auto response = controller->createResponse(Status::CODE_200, "Good");
        controller->setTokenCookie(token.value(), response);
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

    ENDPOINT_ASYNC("GET", "test", TestPage){ 

    ENDPOINT_ASYNC_INIT(TestPage)

    Action act() override {

          inja::Environment env;
          inja::json data;
          data["user"]["is_authenticated"] = false;
          inja::Template temp = env.parse_template(FRONT_PATH "html/test_page.html");
          std::string result = env.render(temp, data); 
          auto response = controller->createResponse(Status::CODE_200, result);
          response->putHeader(Header::CONTENT_TYPE, "text/html");
          return _return(response);
    }
  };
};


#include OATPP_CODEGEN_END(ApiController)
