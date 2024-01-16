#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "oatpp/web/protocol/http/Http.hpp"

#include <functional>


class LoggerRequestInterceptor: public oatpp::web::server::interceptor::RequestInterceptor {
private:
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

public:
  std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override {


    auto routes = request->getStartingLine().path.std_str();

    OATPP_LOGD("LoggerRequestInterceptor","Request [%s] %s %s",
              request->getStartingLine().protocol.std_str().c_str(),
              request->getStartingLine().method.std_str().c_str(),
              request->getStartingLine().path.std_str().c_str());

    oatpp::Int64 start_time(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());

	  request->putBundleData("start_time", start_time);


    return nullptr;
  }
};

class LoggerResponseInterceptor: public oatpp::web::server::interceptor::ResponseInterceptor {
private:
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper);

public:
std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<oatpp::web::server::interceptor::ResponseInterceptor::IncomingRequest> &request,
                                      const std::shared_ptr<oatpp::web::server::interceptor::ResponseInterceptor::OutgoingResponse> &response) override
{
	int64_t duration;

	oatpp::Int64 end_time(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());

	duration = end_time - request->getBundleData<oatpp::Int64>("start_time");

	OATPP_LOGD("LoggerResponseInterceptor","Request [%s] %s %s -> Response %u (%lu ms)",
	         request->getStartingLine().protocol.std_str().c_str(),
	         request->getStartingLine().method.std_str().c_str(),
	         request->getStartingLine().path.std_str().c_str(),
	         response->getStatus().code,
	         duration);

	for (auto const &p : request->getHeaders().getAll())
	{
		OATPP_LOGD("LoggerResponseInterceptor", "Request header [%s]=\"%s\"", p.first.toString()->c_str(), p.second.toString()->c_str());
	}

	return (response);
  }
};