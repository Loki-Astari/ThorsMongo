#include "gtest/gtest.h"

#include "test/TestMessageHandler.h"
#include "ThorSerialize/Traits.h"

#include <sstream>
#include <algorithm>
#include <iomanip>


using ThorsAnvil::DB::Mongo::OP_MsgFlag;
using ThorsAnvil::DB::Mongo::MessageId;
using ThorsAnvil::DB::Mongo::ParserConfig;
using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::MongoActionWriteInterfaceTrivialImpl;
using ThorsAnvil::DB::Mongo::MongoActionReadInterfaceTrivialImpl;

class Empty: public MongoActionReadInterfaceTrivialImpl<Empty>, public MongoActionWriteInterfaceTrivialImpl<Empty>
{};
ThorsAnvil_MakeTrait(Empty);


TEST(ThorsMongActionTest, OutputEmptyMessage)
{
    TestMessageHandler   messageSender;

    MessageId           id;
    messageSender.sendMessage(Empty{}, id, OP_MsgFlag::empty, PrinterConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));

    std::string     result = messageSender.stream.str();
    EXPECT_EQ(26, result.size());


    // Assumages native int is little endian
    std::int32_t    messageSize;
    std::copy(std::begin(result) + 00, std::begin(result) + 04, reinterpret_cast<char*>(&messageSize));
    // Should be Message Header (4 * std::int32_t) + Flags (std::int32_t) + Kind (char) + Document
    // Empty document is int32_t + char (0)
    // 16 + 4 + 1 + 4 + 1 => 26
    EXPECT_EQ(26, messageSize);
    EXPECT_EQ(26, result.size());

    std::int32_t    messageOp;
    std::copy(std::begin(result) + 12, std::begin(result) + 16, reinterpret_cast<char*>(&messageOp));
    EXPECT_EQ(2013, messageOp); // OP_MSG

    OP_MsgFlag    messageFlag;
    std::copy(std::begin(result) + 16, std::begin(result) + 20, reinterpret_cast<char*>(&messageFlag));
    EXPECT_EQ(OP_MsgFlag::empty, messageFlag);

    char            kind = result[20];
    EXPECT_EQ(0, kind);
}

TEST(ThorsMongActionTest, OutputMessageExplicitEmptyFlag)
{
    TestMessageHandler   messageSender;

    MessageId           id;
    messageSender.sendMessage(Empty{}, id, OP_MsgFlag::empty, PrinterConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));

    std::string     result = messageSender.stream.str();
    EXPECT_EQ(26, result.size());


    OP_MsgFlag    messageFlag;
    std::copy(std::begin(result) + 16, std::begin(result) + 20, reinterpret_cast<char*>(&messageFlag));
    EXPECT_EQ(OP_MsgFlag::empty, messageFlag);
}
TEST(ThorsMongActionTest, OutputMessageExplicitChecksumPresentFlag)
{
    TestMessageHandler   messageSender;

    MessageId           id;
    messageSender.sendMessage(Empty{}, id, OP_MsgFlag::checksumPresent, PrinterConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));

    std::string     result = messageSender.stream.str();
    EXPECT_EQ(30, result.size());   // Note 4 more bytes


    OP_MsgFlag    messageFlag;
    std::copy(std::begin(result) + 16, std::begin(result) + 20, reinterpret_cast<char*>(&messageFlag));
    EXPECT_EQ(OP_MsgFlag::checksumPresent, messageFlag);
}
TEST(ThorsMongActionTest, OutputMessageExplicitMoreToComeFlag)
{
    TestMessageHandler   messageSender;

    MessageId           id;
    messageSender.sendMessage(Empty{}, id, OP_MsgFlag::moreToCome, PrinterConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));

    std::string     result = messageSender.stream.str();
    EXPECT_EQ(26, result.size());


    OP_MsgFlag    messageFlag;
    std::copy(std::begin(result) + 16, std::begin(result) + 20, reinterpret_cast<char*>(&messageFlag));
    EXPECT_EQ(OP_MsgFlag::moreToCome, messageFlag);
}
TEST(ThorsMongActionTest, OutputMessageExplicitExhaustAllowedFlag)
{
    TestMessageHandler   messageSender;

    MessageId           id;
    messageSender.sendMessage(Empty{}, id, OP_MsgFlag::exhaustAllowed, PrinterConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));

    std::string     result = messageSender.stream.str();
    EXPECT_EQ(26, result.size());


    OP_MsgFlag    messageFlag;
    std::copy(std::begin(result) + 16, std::begin(result) + 20, reinterpret_cast<char*>(&messageFlag));
    EXPECT_EQ(OP_MsgFlag::exhaustAllowed, messageFlag);
}
TEST(ThorsMongActionTest, OutputMessageAndIgnoreChecksum)
{
    TestMessageHandler   messageSender;

    MessageId           id;
    messageSender.sendMessage(Empty{}, id, OP_MsgFlag::exhaustAllowed, PrinterConfig{}, false);
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));

    std::string     result = messageSender.stream.str();
    EXPECT_EQ(26, result.size());


    OP_MsgFlag    messageFlag;
    std::copy(std::begin(result) + 16, std::begin(result) + 20, reinterpret_cast<char*>(&messageFlag));
    EXPECT_EQ(OP_MsgFlag::exhaustAllowed, messageFlag);
}

//===

TEST(ThorsMongActionTest, InputEmptyMessage)
{
    using namespace std::string_literals;
    TestMessageHandler   messageSender;
    messageSender.stream <<     "\x1a\x00\x00\x00"      // Size
                                "\x0e\x00\x00\x00"      // MessageId
                                "\x20\x00\x00\x00"      // ResponseId
                                "\xdd\x07\x00\x00"      // OP_MSG
                                "\x00\x00\x00\x00"      // Flags: empty
                                "\x00"                  // Kind 0
                                "\x05\x00\x00\x00"      // BSON size
                                "\x00"s;                // BSON terminator

    Empty   data;
    messageSender.recvMessage(data, 0x20, ParserConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));
}

TEST(ThorsMongActionTest, InputMessageChecksumPresent)
{
    using namespace std::string_literals;
    TestMessageHandler   messageSender;
    messageSender.stream <<     "\x1e\x00\x00\x00"      // Size
                                "\x0e\x00\x00\x00"      // MessageId
                                "\x20\x00\x00\x00"      // ResponseId
                                "\xdd\x07\x00\x00"      // OP_MSG
                                "\x01\x00\x00\x00"      // Flags: Checksum
                                "\x00"                  // Kind 0
                                "\x05\x00\x00\x00"      // BSON size
                                "\x00"                  // BSON terminator
                                "\x44\x11\x37\xb8"s;    // Checksum

    Empty   data;
    messageSender.recvMessage(data, 0x20, ParserConfig{});
    EXPECT_FALSE(static_cast<bool>(messageSender.stream));    // Checksum failure.
                                                // As the stringstream does not use ConnectionBufferMongo
    messageSender.stream.clear();
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));
    EXPECT_EQ(30, messageSender.stream.tellg());              // Even though checksum failed.
                                                // We should have read the whole message.
}

TEST(ThorsMongActionTest, InputMessageChecksumPresentIgnoreChecksum)
{
    using namespace std::string_literals;
    TestMessageHandler   messageSender;
    messageSender.stream <<     "\x1e\x00\x00\x00"      // Size
                                "\x0e\x00\x00\x00"      // MessageId
                                "\x20\x00\x00\x00"      // ResponseId
                                "\xdd\x07\x00\x00"      // OP_MSG
                                "\x01\x00\x00\x00"      // Flags: Checksum
                                "\x00"                  // Kind 0
                                "\x05\x00\x00\x00"      // BSON size
                                "\x00"                  // BSON terminator
                                "\x44\x11\x37\xb8"s;    // Checksum

    Empty   data;
    messageSender.recvMessage(data, 0x20, ParserConfig{}, false);
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));
}

TEST(ThorsMongActionTest, InputMessageMoreToCome)
{
    using namespace std::string_literals;
    TestMessageHandler   messageSender;
    messageSender.stream <<     "\x1a\x00\x00\x00"      // Size
                                "\x0e\x00\x00\x00"      // MessageId
                                "\x20\x00\x00\x00"      // ResponseId
                                "\xdd\x07\x00\x00"      // OP_MSG
                                "\x02\x00\x00\x00"      // Flags: moreToCome
                                "\x00"                  // Kind 0
                                "\x05\x00\x00\x00"      // BSON size
                                "\x00"s;                // BSON terminator

    Empty   data;
    messageSender.recvMessage(data, 0x20, ParserConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));
}

TEST(ThorsMongActionTest, InputMessageExhaustAllowed)
{
    using namespace std::string_literals;
    TestMessageHandler   messageSender;
    messageSender.stream <<     "\x1a\x00\x00\x00"      // Size
                                "\x0e\x00\x00\x00"      // MessageId
                                "\x20\x00\x00\x00"      // ResponseId
                                "\xdd\x07\x00\x00"      // OP_MSG
                                "\x00\x00\x01\x00"      // Flags: exhaustAllowed
                                "\x00"                  // Kind 0
                                "\x05\x00\x00\x00"      // BSON size
                                "\x00"s;                // BSON terminator

    Empty   data;
    messageSender.recvMessage(data, 0x20, ParserConfig{});
    EXPECT_TRUE(static_cast<bool>(messageSender.stream));
}

TEST(ThorsMongActionTest, InputMessageInvalidMessageId)
{
    using namespace std::string_literals;
    TestMessageHandler   messageSender;
    messageSender.stream <<     "\x1a\x00\x00\x00"      // Size
                                "\x0e\x00\x00\x00"      // MessageId
                                "\x20\x00\x00\x00"      // ResponseId
                                "\xdd\x07\x00\x00"      // OP_MSG
                                "\x00\x00\x01\x00"      // Flags: exhaustAllowed
                                "\x00"                  // Kind 0
                                "\x05\x00\x00\x00"      // BSON size
                                "\x00"s;                // BSON terminator

    Empty   data;
    messageSender.recvMessage(data, 0x21, ParserConfig{});
    EXPECT_FALSE(static_cast<bool>(messageSender.stream));
}
