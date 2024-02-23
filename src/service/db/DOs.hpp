#ifndef __DOS_H__
#define __DOS_H__


#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)


class MessageDo : public oatpp::DTO {

    DTO_INIT(MessageDo, DTO)
    DTO_FIELD(Int32, id);
    DTO_FIELD(Boolean, is_read);
    DTO_FIELD(Int32, chat_id);
    DTO_FIELD(Int32, user_id);
    DTO_FIELD(String, line_text);
    DTO_FIELD(Int32, created_at);
};


class ChatDo: public oatpp::DTO {
public:
  DTO_INIT(ChatDo, DTO)
  DTO_FIELD(Int32, id);
  DTO_FIELD(Int32, user_first);
  DTO_FIELD(Int32, user_second);

};

class ChatExtendedDo: public oatpp::DTO{
public: 
  DTO_INIT(ChatExtendedDo, DTO)
  DTO_FIELD(Int32, id);
  DTO_FIELD(Int32, user_id);    
  DTO_FIELD(String, nickname);

};







#include OATPP_CODEGEN_END(DTO)

#endif // __DOS_H__