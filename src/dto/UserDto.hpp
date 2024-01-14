#ifndef __USERDTO_H__
#define __USERDTO_H__

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class UserDto: public oatpp::DTO {
  
    DTO_INIT(UserDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD(String, nickname);
    DTO_FIELD(String, password);
    DTO_FIELD(String, mail);

};

class UserToken: public oatpp::DTO {
public:

  DTO_INIT(UserToken, DTO)

  DTO_FIELD(Int32, id);   // id of user
  DTO_FIELD(String, token);
  DTO_FIELD(Int32, expiry);

};

#include OATPP_CODEGEN_END(DTO)

#endif // __USERDTO_H__