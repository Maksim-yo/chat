#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "DTOs.hpp"
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
     VALUE(CODE_PEER_READ, 6),
     VALUE(CODE_PEER_DELIVERED, 7),

     VALUE(CODE_FILE_SHARE, 8),
     VALUE(CODE_FILE_REQUEST_CHUNK, 9),
     VALUE(CODE_FILE_CHUNK_DATA, 10),

     VALUE(CODE_API_ERROR, 11),

     VALUE(CODE_SECURE_ON, 12),
     VALUE(CODE_SECURE_OFF, 13),

     VALUE(CODE_PEER_ONLINE, 14),
     VALUE(CODE_PEER_OFFLINE, 15),

     VALUE(CODE_CHAT_INFO, 16),
     VALUE(CODE_FIND_ROOMS, 17));

class BaseMessage : public oatpp::DTO
{
public:
    DTO_INIT(BaseMessage, DTO)
    DTO_FIELD(Int32, peerId);
    DTO_FIELD(String, peerNickname);
    DTO_FIELD(Int32, timestamp);
    DTO_FIELD(Enum<MessageCodes>::AsNumber::NotNull, code);
};

class ChatMessage : public BaseMessage
{
public:
    DTO_INIT(ChatMessage, BaseMessage)

    DTO_FIELD(Object<MessageDto>, message);
};

class InitialMessage : public BaseMessage
{
public:
    DTO_INIT(InitialMessage, BaseMessage)
    DTO_FIELD(Vector<Object<ChatDto>>, chats);
};

class FindMessage : public BaseMessage
{
public:
    DTO_INIT(FindMessage, BaseMessage)
    DTO_FIELD(String, query);
    DTO_FIELD(Vector<Object<PeerDto>>, users);
};

class ReadMessage : public BaseMessage
{
public:
    DTO_INIT(ReadMessage, BaseMessage)
    DTO_FIELD(Int32, chat_id);
    DTO_FIELD(Int32, count);
};

#include OATPP_CODEGEN_END(DTO)

#endif // __MESSAGES_H__