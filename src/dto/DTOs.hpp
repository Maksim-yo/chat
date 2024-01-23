
#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(MessageCodes, v_int32,
  VALUE(CODE_INFO, 0),

  VALUE(CODE_PEER_JOINED, 1),
  VALUE(CODE_PEER_LEFT, 2),
  VALUE(CODE_PEER_MESSAGE, 3),
  VALUE(CODE_PEER_MESSAGE_FILE, 4),
  VALUE(CODE_PEER_IS_TYPING, 5),

  VALUE(CODE_FILE_SHARE, 6),
  VALUE(CODE_FILE_REQUEST_CHUNK, 7),
  VALUE(CODE_FILE_CHUNK_DATA, 8),

  VALUE(CODE_API_ERROR, 9),
  
  VALUE(CODE_SECURE_ON, 10),
  VALUE(CODE_SECURE_OFF, 11),

  VALUE(CODE_ONLINE, 12),
  VALUE(CODE_OFFLINE, 13),

  VALUE(CODE_CHAT_INFO, 14),
  VALUE(CODE_FIND_ROOMS, 15)
);

class PeerDto : public oatpp::DTO {
public:

  DTO_INIT(PeerDto, DTO)

  DTO_FIELD(Int64, peerId);
  DTO_FIELD(String, peerName);

};


class FileDto : public oatpp::DTO {

  DTO_INIT(FileDto, DTO)

  DTO_FIELD(Int64, clientFileId);
  DTO_FIELD(Int64, serverFileId);
  DTO_FIELD(String, name);
  DTO_FIELD(Int64, size);

  DTO_FIELD(Int64, chunkPosition);
  DTO_FIELD(Int64, chunkSize);
  DTO_FIELD(Int64, subscriberId);
  DTO_FIELD(String, data); // base64 data

};

class BaseMessageDto: public oatpp::DTO {
public:

  DTO_INIT(BaseMessageDto, DTO)
  DTO_FIELD(Int64, peerId);
  DTO_FIELD(String, peerNickname);
  DTO_FIELD(Int64, timestamp);
  DTO_FIELD(Enum<MessageCodes>::AsNumber::NotNull, code);
};

// class MessageDto : public BaseMessageDto {
// public:
//   typedef List<Object<FileDto>> FilesList;
// public:

//   DTO_INIT(MessageDto, BaseMessageDto)

//   DTO_FIELD(Int64, peerId);
//   DTO_FIELD(String, peerName);
//   DTO_FIELD(Enum<MessageCodes>::AsNumber::NotNull, code);
//   DTO_FIELD(String, message);
//   DTO_FIELD(Int64, timestamp);
//   DTO_FIELD(String, roomHash);

//   DTO_FIELD(Int32, roomId);
//   DTO_FIELD(List<Object<PeerDto>>, peers);
//   DTO_FIELD(List<Object<MessageDto>>, history);

//   DTO_FIELD(List<Object<FileDto>>, files);

// };

class MessageDto: public BaseMessageDto {
public:
  DTO_INIT(MessageDto, BaseMessageDto)
  DTO_FIELD(oatpp::String, message);

};
// class ChatMessageDto: public BaseMessageDto {
// public:
//   DTO_INIT(ChatMessageDto, BaseMessageDto)
//   DTO_FIELD(Int32, id);
//   DTO_FIELD(Vector<Object<MessageDto>>, messages);
  
// };

class ChatDto: public oatpp::DTO {
public:
  DTO_INIT(ChatDto, DTO)
  DTO_FIELD(Int32, id);

  DTO_FIELD(Vector<Object<MessageDto>>, history);
  DTO_FIELD(Vector<Object<PeerDto>>, peers);
  
};

class ChatMessageDto: public BaseMessageDto {
public:
  DTO_INIT(ChatMessageDto, BaseMessageDto)

  DTO_FIELD(Int32, id);
  DTO_FIELD(String, name);
  DTO_FIELD(Object<MessageDto>, message);
  DTO_FIELD(Vector<Object<MessageDto>>, history);
  DTO_FIELD(Vector<Object<PeerDto>>, peers);

  };

class MessageInitial: public BaseMessageDto {
public: 
  DTO_INIT(MessageInitial, BaseMessageDto)
  DTO_FIELD(Vector<Object<ChatDto>>, chats);
};

class MessageFind: public BaseMessageDto {
public: 
  DTO_INIT(MessageFind, BaseMessageDto)
  DTO_FIELD(String, query);
  DTO_FIELD(Vector<Object<PeerDto>>, users);
};



#include OATPP_CODEGEN_END(DTO)

#endif // DTOs_hpp