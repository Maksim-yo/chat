#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class PeerDto : public oatpp::DTO
{
public:
    DTO_INIT(PeerDto, DTO)

    DTO_FIELD(Int32, peerId);
    DTO_FIELD(String, peerName);
    DTO_FIELD(Boolean, isOnline) = false;
};

class FileDto : public oatpp::DTO
{

    DTO_INIT(FileDto, DTO)

    DTO_FIELD(Int32, clientFileId);
    DTO_FIELD(Int32, serverFileId);
    DTO_FIELD(String, name);
    DTO_FIELD(Int32, size);

    DTO_FIELD(Int32, chunkPosition);
    DTO_FIELD(Int32, chunkSize);
    DTO_FIELD(Int32, subscriberId);
    DTO_FIELD(String, data); // base64 data
};

class MessageDto : public oatpp::DTO
{
public:
    DTO_INIT(MessageDto, DTO)
    DTO_FIELD(UInt32, messageHash);
    DTO_FIELD(String, message);
    DTO_FIELD(Boolean, is_read);
    DTO_FIELD(Int32, timestamp);
    DTO_FIELD(Int32, peerId);
    DTO_FIELD(Int32, chat_id);
};

class ChatDto : public oatpp::DTO
{
public:
    DTO_INIT(ChatDto, DTO)
    DTO_FIELD(Int32, id);

    DTO_FIELD(Vector<Object<MessageDto>>, history);
    DTO_FIELD(Vector<Object<PeerDto>>, peers);
};

#include OATPP_CODEGEN_END(DTO)

#endif // DTOs_hpp