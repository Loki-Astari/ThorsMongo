#include "gtest/gtest.h"

#include "ConnectionBufferMongo.h"
#include "MongoUtil.h"
#include "test/Action.h"

#include <unistd.h>
#include <iostream>
#include <fstream>


using ThorsAnvil::DB::Mongo::ConnectionBufferMongo;
using ThorsAnvil::DB::Mongo::OpMsgHeader;;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::OP_MsgFlag;
using ThorsAnvil::DB::Mongo::OpCode;
using ThorsAnvil::ThorsSocket::Open;
using ThorsAnvil::ThorsSocket::Mode;

TEST(ConnectionBufferMongoTest, Build)
{
    SocketSetUp     winSocketInit;
    char data[10000] = {0};
    getcwd(data, 10000);
    ConnectionBufferMongo   buffer({"test/data/emptyMessage", Open::Append});
    EXPECT_NE(0, buffer.getSocket().socketId(Mode::Read));
}

TEST(ConnectionBufferMongoTest, ReadHeader)
{
    SocketSetUp     winSocketInit;
    OpMsgHeader data{0, 0, 0, OpCode::OP_NOOP};
    ConnectionBufferMongo   buffer({"test/data/emptyMessage", ThorsAnvil::ThorsSocket::Open::Append});
    auto val = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));

    EXPECT_EQ(26,               data.messageSize);
    EXPECT_EQ(15,               data.messageId);
    EXPECT_EQ(32,               data.messageResponseId);
    EXPECT_EQ(OpCode::OP_MSG,   data.opCode);
    EXPECT_EQ(sizeof(data),     val);
}

TEST(ConnectionBufferMongoTestTest, BadMessageHeader)
{
    SocketSetUp     winSocketInit;
    OpMsgHeader data{0, 0, 0, OpCode::OP_NOOP};
    ConnectionBufferMongo   buffer({"test/data/badMessageSize", ThorsAnvil::ThorsSocket::Open::Append});
    auto val = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));

    EXPECT_EQ(0, val);
}

TEST(ConnectionBufferMongoTest, ReadMoreMessagesThanIsAvailable)
{
    SocketSetUp     winSocketInit;
    char data[50];
    ConnectionBufferMongo   buffer({"test/data/emptyMessages", ThorsAnvil::ThorsSocket::Open::Append});
    auto val = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));

    EXPECT_EQ(0, val);
}

TEST(ConnectionBufferMongoTest, ReadTwoMessagesSoPubSeekIsIncremented)
{
    SocketSetUp     winSocketInit;
    char dataIgnore[26];
    ConnectionBufferMongo   buffer({"test/data/twoEmptyMessages", ThorsAnvil::ThorsSocket::Open::Append});
    auto val1 = buffer.sgetn(reinterpret_cast<char*>(&dataIgnore), sizeof(dataIgnore));

    EXPECT_EQ(26, val1);
    EXPECT_EQ(26, buffer.pubseekoff(0, std::ios::cur,std::ios::in));

    OpMsgHeader data{0, 0, 0, OpCode::OP_NOOP};
    auto val2 = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, val2);

    EXPECT_EQ(26,               data.messageSize);
    EXPECT_EQ(15,               data.messageId);
    EXPECT_EQ(32,               data.messageResponseId);
    EXPECT_EQ(OpCode::OP_MSG,   data.opCode);
    EXPECT_EQ(42, buffer.pubseekoff(0, std::ios::cur,std::ios::in));
}

TEST(ConnectionBufferMongoTest, ReadMoreThanBufferHasButWithinRange)
{
    SocketSetUp     winSocketInit;
    OpMsgHeader data{0, 0, 0, OpCode::OP_NOOP};
    ConnectionBufferMongo   buffer({"test/data/messagesHeaderAndFlag", ThorsAnvil::ThorsSocket::Open::Append});
    auto val = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, val);

    // There are 4 bytes in the buffer.
    // There should be 10 bytes in the object but the file will fail to read more than 4.

    char dataIngore[10];
    auto val2 = buffer.sgetn(reinterpret_cast<char*>(&dataIngore), sizeof(dataIngore));
    EXPECT_EQ(4, val2);
}

TEST(ConnectionBufferMongoTest, WriteMessage)
{
    SocketSetUp     winSocketInit;
    OpMsgHeader data{26, 15, 32, OpCode::OP_MSG};
    ConnectionBufferMongo   buffer({"test/data/WriteMessage", ThorsAnvil::ThorsSocket::Open::Truncate});

    auto val = buffer.sputn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, val);

    unlink("test/data/WriteMessage");
}

TEST(ConnectionBufferMongoTest, WriteMessageFlushBeforeMessageComplete)
{
    SocketSetUp     winSocketInit;
    OpMsgHeader data{26, 15, 32, OpCode::OP_MSG};
    ConnectionBufferMongo   buffer({"test/data/WriteMessage", ThorsAnvil::ThorsSocket::Open::Truncate});

    auto val = buffer.sputn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, val);
    auto s = buffer.pubsync();
    EXPECT_EQ(-1, s);

    unlink("test/data/WriteMessage");
}
TEST(ConnectionBufferMongoTest, WriteMessageFlush)
{
    SocketSetUp     winSocketInit;
    OpMsgHeader data{26, 15, 32, OpCode::OP_MSG};
    ConnectionBufferMongo   buffer({"test/data/WriteMessage", ThorsAnvil::ThorsSocket::Open::Truncate});

    auto val = buffer.sputn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, val);

    OP_MsgFlag    flags = OP_MsgFlag::empty;
    val = buffer.sputn(reinterpret_cast<char*>(&flags), sizeof(flags));
    EXPECT_EQ(4, val);

    std::uint8_t    kind = 0;
    val = buffer.sputn(reinterpret_cast<char*>(&kind), sizeof(kind));
    EXPECT_EQ(1, val);

    std::int32_t    bsonSize = 5;
    val = buffer.sputn(reinterpret_cast<char*>(&bsonSize), sizeof(bsonSize));
    EXPECT_EQ(4, val);

    std::uint8_t    bsonTerm = 0;
    val = buffer.sputn(reinterpret_cast<char*>(&bsonTerm), sizeof(bsonTerm));
    EXPECT_EQ(1, val);

    auto s = buffer.pubsync();
    EXPECT_EQ(0, s);

    unlink("test/data/WriteMessage");
}

struct LocalOpMsg: public OpMsgHeader
{
    char    buffer[1000];
};
TEST(ConnectionBufferMongoTest, WriteMessageMoreThanBuffer)
{
    SocketSetUp     winSocketInit;
    LocalOpMsg data{26, 15, 32, OpCode::OP_MSG, {}};
    ConnectionBufferMongo   buffer({"test/data/WriteMessage", ThorsAnvil::ThorsSocket::Open::Truncate});

    auto val = buffer.sputn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(0, val);

    unlink("test/data/WriteMessage");
}

TEST(ConnectionBufferMongoTest, CheckThatMoveWorks)
{
    SocketSetUp     winSocketInit;
    LocalOpMsg data{26, 15, 32, OpCode::OP_MSG, {}};
    ConnectionBufferMongo   buffer({"test/data/WriteMessage", ThorsAnvil::ThorsSocket::Open::Truncate});
    auto val = buffer.sputn(reinterpret_cast<char*>(&data), 26);   // Note: Message size is set to 26 above
    auto tel = buffer.pubseekoff(0, std::ios::cur,std::ios::in);

    ConnectionBufferMongo   buffer2(std::move(buffer));
    EXPECT_EQ(tel, buffer2.pubseekoff(0, std::ios::cur,std::ios::in));
    EXPECT_EQ(26, val);

    unlink("test/data/WriteMessage");
}

/*
 Use this test to build data for the test: ReadCompressData
*/
TEST(ConnectionBufferMongoTest, WriteCompressData)
{
    SocketSetUp     winSocketInit;
    {
        ConnectionBufferMongo   buffer({"test/data/compressedData1", ThorsAnvil::ThorsSocket::Open::Truncate});
        buffer.setCompressionOnWrite(Compression::Snappy);

        OpMsgHeader data{26, 15, 32, OpCode::OP_MSG};
        buffer.sputn(reinterpret_cast<char*>(&data), sizeof(data));

        OP_MsgFlag    flags = OP_MsgFlag::empty;
        buffer.sputn(reinterpret_cast<char*>(&flags), sizeof(flags));

        std::uint8_t    kind = 0;
        buffer.sputn(reinterpret_cast<char*>(&kind), sizeof(kind));

        std::int32_t    bsonSize = 5;
        buffer.sputn(reinterpret_cast<char*>(&bsonSize), sizeof(bsonSize));

        std::uint8_t    bsonTerm = 0;
        buffer.sputn(reinterpret_cast<char*>(&bsonTerm), sizeof(bsonTerm));

        buffer.pubsync();
    }
    std::ifstream       dataFile("test/data/compressedData1", std::ios::binary);
    std::vector<char>   data{std::istreambuf_iterator<char>{dataFile}, std::istreambuf_iterator<char>{}};

    std::vector<char>   expected{
        '\x25', '\x00', '\x00', '\x00',     // Message Size
        '\x0f', '\x00', '\x00', '\x00',     // Message Id
        '\x20', '\x00', '\x00', '\x00',     // Response Id
        '\xdc', '\x07', '\x00', '\x00',     // Op Code (OP_COMPRESSED)
        '\xdd', '\x07', '\x00', '\x00',     // Original OpCode (OP_MSG)
        '\x0a', '\x00', '\x00', '\x00',     // Original Message Size
        '\x01',                             // Compression Type (Snappy)
        '\x0a', '\x24', '\x00', '\x00', '\x00', '\x00', '\x00', '\x05', '\x00', '\x00', '\x00', '\x00'};
    EXPECT_EQ(expected, data);
    unlink("test/data/compressedData1");
}

TEST(ConnectionBufferMongoTest, ReadCompressData)
{
    SocketSetUp     winSocketInit;
    ConnectionBufferMongo   buffer({"test/data/compressedData", ThorsAnvil::ThorsSocket::Open::Append});

    OpMsgHeader data{0, 0, 0, OpCode::OP_NOOP};
    auto r = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, r);

    EXPECT_EQ(26,               data.messageSize);
    EXPECT_EQ(15,               data.messageId);
    EXPECT_EQ(32,               data.messageResponseId);
    EXPECT_EQ(OpCode::OP_MSG,   data.opCode);

    OP_MsgFlag    flags = static_cast<OP_MsgFlag>(255);
    r = buffer.sgetn(reinterpret_cast<char*>(&flags), sizeof(flags));
    EXPECT_EQ(4, r);
    EXPECT_EQ(OP_MsgFlag::empty, flags);

    std::uint8_t    kind = 12;
    r = buffer.sgetn(reinterpret_cast<char*>(&kind), sizeof(kind));
    EXPECT_EQ(1, r);
    EXPECT_EQ(0, kind);

    std::int32_t    bsonSize = 255;
    r = buffer.sgetn(reinterpret_cast<char*>(&bsonSize), sizeof(bsonSize));
    EXPECT_EQ(4, r);
    EXPECT_EQ(5, bsonSize);

    std::uint8_t    bsonTerm = 1;
    r = buffer.sgetn(reinterpret_cast<char*>(&bsonTerm), sizeof(bsonTerm));
    EXPECT_EQ(1, r);
    EXPECT_EQ(0, bsonTerm);
}

TEST(ConnectionBufferMongoTest, ReadDataWithChecksum)
{
    SocketSetUp     winSocketInit;
    ConnectionBufferMongo   buffer({"test/data/emptyMessageWithChecksum", ThorsAnvil::ThorsSocket::Open::Append});

    OpMsgHeader data{0, 0, 0, OpCode::OP_NOOP};
    auto r = buffer.sgetn(reinterpret_cast<char*>(&data), sizeof(data));
    EXPECT_EQ(16, r);

    EXPECT_EQ(30,               data.messageSize);
    EXPECT_EQ(15,               data.messageId);
    EXPECT_EQ(32,               data.messageResponseId);
    EXPECT_EQ(OpCode::OP_MSG,   data.opCode);

    OP_MsgFlag    flags = static_cast<OP_MsgFlag>(255);
    r = buffer.sgetn(reinterpret_cast<char*>(&flags), sizeof(flags));
    EXPECT_EQ(4, r);
    EXPECT_EQ(OP_MsgFlag::checksumPresent, flags);

    std::uint8_t    kind = 12;
    r = buffer.sgetn(reinterpret_cast<char*>(&kind), sizeof(kind));
    EXPECT_EQ(1, r);
    EXPECT_EQ(0, kind);

    std::int32_t    bsonSize = 255;
    r = buffer.sgetn(reinterpret_cast<char*>(&bsonSize), sizeof(bsonSize));
    EXPECT_EQ(4, r);
    EXPECT_EQ(5, bsonSize);

    std::uint8_t    bsonTerm = 1;
    r = buffer.sgetn(reinterpret_cast<char*>(&bsonTerm), sizeof(bsonTerm));
    EXPECT_EQ(1, r);
    EXPECT_EQ(0, bsonTerm);

    std::uint32_t   checksum  = 1;
    r = buffer.sgetn(reinterpret_cast<char*>(&checksum), sizeof(checksum));
    EXPECT_EQ(4, r);
    EXPECT_EQ(0, checksum);
}

/*
 Use this test to build data for the test: ReadDataWithChecksum
*/
TEST(ConnectionBufferMongoTest, WriteDataWithChecksum)
{
    SocketSetUp     winSocketInit;
    {
        ConnectionBufferMongo   buffer({"test/data/checksumData", ThorsAnvil::ThorsSocket::Open::Truncate});

        OpMsgHeader data{30, 15, 32, OpCode::OP_MSG};
        buffer.sputn(reinterpret_cast<char*>(&data), sizeof(data));

        OP_MsgFlag    flags = OP_MsgFlag::checksumPresent;
        buffer.sputn(reinterpret_cast<char*>(&flags), sizeof(flags));

        std::uint8_t    kind = 0;
        buffer.sputn(reinterpret_cast<char*>(&kind), sizeof(kind));

        std::int32_t    bsonSize = 5;
        buffer.sputn(reinterpret_cast<char*>(&bsonSize), sizeof(bsonSize));

        std::uint8_t    bsonTerm = 0;
        buffer.sputn(reinterpret_cast<char*>(&bsonTerm), sizeof(bsonTerm));

        std::uint32_t   checksum  = 0;
        buffer.sputn(reinterpret_cast<char*>(&checksum), sizeof(checksum));

        buffer.pubsync();
    }
    std::ifstream       dataFile("test/data/checksumData", std::ios::binary);
    std::vector<char>   data{std::istreambuf_iterator<char>{dataFile}, std::istreambuf_iterator<char>{}};

    std::vector<char>   expected{
        '\x1E', '\x00', '\x00', '\x00',     // Message Size
        '\x0f', '\x00', '\x00', '\x00',     // Message Id
        '\x20', '\x00', '\x00', '\x00',     // Response Id
        '\xdd', '\x07', '\x00', '\x00',     // Op Code (OP_MSG)
        '\x01', '\x00', '\x00', '\x00',     // Flag => Checksum bit set
        '\x00',                             // Kind 0
        '\x05', '\x00', '\x00', '\x00',     // BSON object size.
        '\x00',                             // BSON object terminator
        '\x44', '\x11', '\x37', '\xb8'      // Checksum
    };
    // Checksum generated on this page: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
    // Using CRC32_C (Default parameters)
    // Places the following byetes in the CRC Input Box (Select Bytes)
    // 0x1E 0x00 0x00 0x00 0x0f 0x00 0x00 0x00 0x20 0x00 0x00 0x00 0xdd 0x07 0x00 0x00 0x01 0x00 0x00 0x00 0x00 0x05 0x00 0x00 0x00 0x00
    // Click "Calculate CRC": Result => 0xB8371144
    EXPECT_EQ(expected, data);

    unlink("test/data/checksumData");
}

