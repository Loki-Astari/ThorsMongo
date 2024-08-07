#include "MessageHandler.h"
#include "ConnectionBufferMongo.h"

namespace ThorsAnvil::DB::Mongo::Internal
{

THORS_MONGO_HEADER_ONLY_INCLUDE
MessageId& getMessageId()
{
    static MessageId nextMessageId = 0;
    return nextMessageId;
}

THORS_MONGO_HEADER_ONLY_INCLUDE
MessageId getNextMessageId()
{
    return getMessageId()++;
}

}

using namespace ThorsAnvil::DB::Mongo;

THORS_MONGO_HEADER_ONLY_INCLUDE
std::uint32_t MessageHandler::calcSize(MongoActionWriteInterface const& mongoAction, bool hasCheckSum) const
{
    return kSizeOpMsgBlock
           // + ThorsAnvil::Serialize::bsonGetPrintSize(mongoAction)
           + mongoAction.getSizeBson()
           // Op_Msg Optional Checksum
           + (hasCheckSum ? sizeof(std::int32_t) : 0);
}

THORS_MONGO_HEADER_ONLY_INCLUDE
bool MessageHandler::sendMessage(MongoActionWriteInterface const& mongoAction, MessageId& messageId, OP_MsgFlag flags, PrinterConfig const& config, bool handleChecksum)
{
    OpMsgBlock      wireMessage;
    wireMessage.messageSize         = calcSize(mongoAction, flags & OP_MsgFlag::checksumPresent);
    wireMessage.messageId           = Internal::getNextMessageId();
    wireMessage.messageResponseId   = 0;
    wireMessage.opCode              = OpCode::OP_MSG;
    wireMessage.flags               = flags;
    wireMessage.kind                = 0;

    if (!handleChecksum)
    {
        // If we can't handle checksum then don't allow the flag to be set.
        wireMessage.flags &= ~OP_MsgFlag::checksumPresent;
    }

    std::iostream&    stream = getStream();

    stream.write(reinterpret_cast<char*>(&wireMessage), kSizeOpMsgBlock);
    mongoAction.writeBson(stream, config);

    if (flags & OP_MsgFlag::checksumPresent)
    {
        // That actual checksum creation is done in ConnectionBufferMongo::sync()
        // The sync happens when we flush the stream below.
        std::uint32_t   checksum = 0;
        stream.write(reinterpret_cast<char*>(&checksum), sizeof(checksum));
    }

    if (stream)
    {
        messageId = wireMessage.messageId;
    }
    else
    {
        ThorsLogCritical("ThorsAnvil::DB::Mongo::MessageHandler", "sendMessage", "Serialization of message failed.");
    }
    stream.flush();
    return static_cast<bool>(stream);
}

THORS_MONGO_HEADER_ONLY_INCLUDE
bool MessageHandler::recvMessage(MongoActionReadInterface& mongoAction, MessageId messageId, ParserConfig const& config, bool handleChecksum)
{
    OpMsgBlock          wireMessage;
    std::iostream&      stream = getStream();

    std::streampos start    = stream.tellg();
    stream.read(reinterpret_cast<char*>(&wireMessage), kSizeOpMsgBlock);
    mongoAction.readBson(stream, config);
    //stream >> ThorsAnvil::Serialize::bsonImporter(mongoAction, config);

    if (stream)
    {
        if (wireMessage.flags & OP_MsgFlag::checksumPresent)
        {
            // That actual checksum validation is done in ConnectionBufferMongo::underflow()
            std::uint32_t   checksum;
            stream.read(reinterpret_cast<char*>(&checksum), sizeof(checksum));

            // If validation worked correctly. Then it XOR the calculated value on the stream.
            // Thus if everything worked it will result in this checksum being zero.
            // If we don't want to handle checksum then simply ignore it.
            if (handleChecksum && checksum != 0)
            {
                stream.setstate(std::ios::failbit);
                ThorsLogCritical("ThorsAnvil::DB::Mongo::MessageHandler", "recvMessage", "Deserialization checksum validation failed");
            }
        }

        std::streampos end      = stream.tellg();
        std::size_t size        = end - start;
        if (size != wireMessage.messageSize)
        {
            stream.setstate(std::ios::failbit);
            ThorsLogCritical("ThorsAnvil::DB::Mongo::MessageHandler", "recvMessage", "Deserialization of message failed. Invalid Size: Bytes read: ", size, " Wire Message Size: ", wireMessage.messageSize);
        }
        if (wireMessage.opCode != OpCode::OP_MSG)
        {
            stream.setstate(std::ios::failbit);
            ThorsLogCritical("ThorsAnvil::DB::Mongo::MessageHandler", "recvMessage", "Unexpected Message Type. Expected: OP_MSG(", static_cast<int>(OpCode::OP_MSG), ") Got(", static_cast<int>(wireMessage.opCode));
        }
        if (wireMessage.messageResponseId != messageId)
        {
            stream.setstate(std::ios::failbit);
            ThorsLogCritical("ThorsAnvil::DB::Mongo::MessageHandler", "recvMessage", "This message is not the expected reply: Expected: ", messageId, " Got: ", wireMessage.messageResponseId);
        }
    }
    return static_cast<bool>(stream);
}

THORS_MONGO_HEADER_ONLY_INCLUDE
void MessageHandler::setStreamBad()
{
    getStream().setstate(std::ios::failbit);
}

THORS_MONGO_HEADER_ONLY_INCLUDE
void MessageHandler::setCompression(Compression compression)
{
    ConnectionBufferMongo* buffer = dynamic_cast<ConnectionBufferMongo*>(getStream().rdbuf());
    if (buffer) {
        buffer->setCompressionOnWrite(compression);
    }
}
