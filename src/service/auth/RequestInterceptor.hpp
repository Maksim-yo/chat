#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"

#include "oatpp/web/protocol/http/Http.hpp"


#include "AuthServiceBase.hpp"
#include <functional>

class RequestInterceptor: public oatpp::web::server::interceptor::RequestInterceptor {
private:
    OATPP_COMPONENT(std::shared_ptr<AuthServiceBase>, m_authServiceBase);
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

public:
  std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override {

    auto routes = request->getStartingLine().path.std_str();

    auto cookie = request->getHeader("Cookie");
    oatpp::String authToken;
    if (cookie) {
      oatpp::web::protocol::http::HeaderValueData data;
      oatpp::web::protocol::http::Parser::parseHeaderValueData(data, cookie, ';');
      authToken = data.getTitleParamValue({AuthTokenService::TOKEN});
    }

    try {
        m_authServiceBase->handleAuthorization(authToken);
    }
    catch(const oatpp::web::protocol::http::HttpError& err) {
        
        if (routes == "/accounts/registration" || routes == "/accounts/auth")
          return nullptr;

        auto response = oatpp::web::protocol::http::outgoing::ResponseFactory::createResponse(oatpp::web::protocol::http::Status::CODE_302, "");
        response->putHeader("Location", "/accounts/auth");
        return response;

    }
    return nullptr;


}
};