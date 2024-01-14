#ifndef __CONFIGDTO_H__
#define __CONFIGDTO_H__



#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ConfigDto: public oatpp::DTO {

  DTO_INIT(ConfigDto, DTO)

  DTO_FIELD(Int32, port);
  DTO_FIELD(oatpp::String, host);
  DTO_FIELD(oatpp::String, dbConnectionString);

};
#include OATPP_CODEGEN_END(DTO)

#endif // __CONFIGDTO_H__