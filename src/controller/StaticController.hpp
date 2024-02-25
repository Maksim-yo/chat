#ifndef __STATICCONTROLLER_H__
#define __STATICCONTROLLER_H__

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "StaticFilesManager.hpp"
#include "dto/ConfigDto.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class StaticController : public oatpp::web::server::api::ApiController {

private: 
  typedef StaticController __ControllerType;
  StaticFilesManager m_staticManager;

public:

  StaticController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper), OATPP_COMPONENT(oatpp::Object<ConfigDto>, config))
    : oatpp::web::server::api::ApiController(objectMapper), m_staticManager(config->rootStaticPath)
  {
  
  }

   ENDPOINT_ASYNC("GET", "*", StaticRoute){ 

    ENDPOINT_ASYNC_INIT(StaticRoute)

    Action act() override {

          oatpp::String file = controller->m_staticManager.getFile(request->getPathTail());
          OATPP_ASSERT_HTTP(file.get() != nullptr, Status::CODE_404, "File not found");
          auto response = controller->createResponse(Status::CODE_200, file);
          return _return(response);
    } 
  };
};

#include OATPP_CODEGEN_END(ApiController)

#endif // __STATICCONTROLLER_H__