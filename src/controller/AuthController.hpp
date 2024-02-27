#ifndef __AUTHCONTROLLER_H__
#define __AUTHCONTROLLER_H__

#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
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

#define AUTH_INIT() \
auto routes = request->getStartingLine().path.std_str();                  \
oatpp::String authToken = controller->_cookieAuth(  request);             \
std::shared_ptr<UserObject> authObject = nullptr;\
try {                                                                     \
authObject = controller->m_authServiceBase->handleAuthorization(authToken);\
}                                                                         \
catch(const oatpp::web::protocol::http::HttpError& err) {                 \
                                                                          \
  if (routes != "/accounts/registration" || routes != "/accounts/auth") { \
    auto response = oatpp::web::protocol::http::outgoing::ResponseFactory::createResponse(oatpp::web::protocol::http::Status::CODE_302, ""); \
    response->putHeader("Location", "/accounts/auth");                    \
  return _return(response);                                               \
  }                                                                       \
}

#include OATPP_CODEGEN_BEGIN(ApiController)

class AuthController : public oatpp::web::server::api::ApiController {

private: 
  typedef AuthController __ControllerType;

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocketConnectionHandler, "websocket");
  OATPP_COMPONENT(std::shared_ptr<AuthServiceBase>, m_authServiceBase);
  OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);
private:

//TODO: refactor
oatpp::String _cookieAuth(const std::shared_ptr<IncomingRequest>& request){
    auto cookie = request->getHeader("Cookie");
    oatpp::String authToken;
    if (!cookie)
      return authToken;
    oatpp::web::protocol::http::HeaderValueData data;
    oatpp::web::protocol::http::Parser::parseHeaderValueData(data, cookie, ';');
    authToken = data.getTitleParamValue({AuthTokenService::TOKEN});
    return authToken;
    
}                                                                         
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
        auto response = controller->createResponse(Status::CODE_302, ""); 
        response->putHeader("Location", "/");
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
          response->putHeader("Location", "/");
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
        auto response = controller->createResponse(Status::CODE_302, "");
        response->putHeader("Location", "/");
        controller->setTokenCookie(token.value(), response);
        return _return(response);

      }
    };

    ENDPOINT_ASYNC("GET", "ws/", WS) {

    ENDPOINT_ASYNC_INIT(WS)

     Action act() override {

      AUTH_INIT()

      auto user = authObject->getUser();
      auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(), controller->websocketConnectionHandler);
      auto parameters = std::make_shared<oatpp::network::ConnectionHandler::ParameterMap>();
      (*parameters)["userMail"] = user->mail;
      (*parameters)["userNickname"] = user->nickname;
      (*parameters)["peerId"] = oatpp::utils::conversion::int32ToStr(user->id);

      response->setConnectionUpgradeParameters(parameters);
      return _return(response);

       }  
   };


    ENDPOINT_ASYNC("GET", "/", TestPage){ 

    ENDPOINT_ASYNC_INIT(TestPage)

    Action act() override {

          AUTH_INIT()

          inja::Environment env;
          inja::json data;

          oatpp::String json = controller->m_defaultObjectMapper->writeToString(authObject->getUser());
          auto userJson = inja::json::parse(json.getValue(""));
          data["user"].update(userJson);

          inja::Template temp = env.parse_template(FRONT_PATH "html/chat.html");
          std::string result = env.render(temp, data); 
          auto response = controller->createResponse(Status::CODE_200, result);
          response->putHeader(Header::CONTENT_TYPE, "text/html");
          return _return(response);
    }
  };

    ENDPOINT_ASYNC("GET", "chat.js", temp){ 

    ENDPOINT_ASYNC_INIT(temp)

    Action act() override {

          AUTH_INIT()

          inja::Environment env;
          inja::json data;

          oatpp::String json = controller->m_defaultObjectMapper->writeToString(authObject->getUser());
          auto userJson = inja::json::parse(json.getValue(""));
          data["user"].update(userJson);

          inja::Template temp = env.parse_template(FRONT_PATH "html/chat.js");
          std::string result = env.render(temp, data); 
          auto response = controller->createResponse(Status::CODE_200, result);
          response->putHeader(Header::CONTENT_TYPE, "text/javascript");
          return _return(response);
    }
  };
};


#include OATPP_CODEGEN_END(ApiController)

#endif // __AUTHCONTROLLER_H__