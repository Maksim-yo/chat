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


#include OATPP_CODEGEN_END(DTO)

#endif // __USERDTO_H__