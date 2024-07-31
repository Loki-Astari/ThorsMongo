#include "gtest/gtest.h"

#include "MongoUtil.h"

using ThorsAnvil::DB::Mongo::OP_MsgFlag;
using ThorsAnvil::DB::Mongo::OpCode;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::kSizeUInt32;
using ThorsAnvil::DB::Mongo::kSizeOpMsgHeaderSize;
using ThorsAnvil::DB::Mongo::kSizeOpMsgBlock;
using ThorsAnvil::DB::Mongo::kSizeOpCompressedBlock;
using ThorsAnvil::DB::Mongo::kSizeOpCompressedMessage;
using ThorsAnvil::DB::Mongo::kSizeSmallestPossibleMessage;

TEST(MongoUtilTest, OP_MsgFlagEmpty)
{
    OP_MsgFlag      test = OP_MsgFlag::empty;
    EXPECT_EQ(static_cast<OP_MsgFlag>(0), test);
}

TEST(MongoUtilTest, OP_MsgFlagChecksumPresent)
{
    OP_MsgFlag      test = OP_MsgFlag::checksumPresent;
    EXPECT_EQ(static_cast<OP_MsgFlag>(1), test);
}

TEST(MongoUtilTest, OP_MsgFlagMoreToCome)
{
    OP_MsgFlag      test = OP_MsgFlag::moreToCome;
    EXPECT_EQ(static_cast<OP_MsgFlag>(2), test);
}

TEST(MongoUtilTest, OP_MsgFlagExhaustAllowed)
{
    OP_MsgFlag      test = OP_MsgFlag::exhaustAllowed;
    EXPECT_EQ(static_cast<OP_MsgFlag>(0x10000), test);
}

TEST(MongoUtilTest, OP_MsgFlagOr)
{
    OP_MsgFlag      test = OP_MsgFlag::checksumPresent | OP_MsgFlag::exhaustAllowed;
    EXPECT_EQ(static_cast<OP_MsgFlag>(0x00001 | 0x10000), test);
}

TEST(MongoUtilTest, OP_MsgFlagAnd)
{
    OP_MsgFlag      test = OP_MsgFlag::checksumPresent | OP_MsgFlag::exhaustAllowed;
    EXPECT_TRUE(test & OP_MsgFlag::exhaustAllowed);
}

TEST(MongoUtilTest, OP_MsgFlagNot)
{
    OP_MsgFlag      test = ~OP_MsgFlag::checksumPresent;
    EXPECT_EQ(static_cast<OP_MsgFlag>(~0x00001), test);
}

TEST(MongoUtilTest, OP_MsgFlagAndEuqal)
{
    OP_MsgFlag      test = OP_MsgFlag::checksumPresent | OP_MsgFlag::exhaustAllowed;
    test &= ~OP_MsgFlag::checksumPresent;
    EXPECT_EQ(OP_MsgFlag::exhaustAllowed, test);
}

TEST(MongoUtilTest, OpCodeOPCompressed)
{
    OpCode      test = OpCode::OP_COMPRESSED;
    EXPECT_EQ(static_cast<OpCode>(2012), test);
}

TEST(MongoUtilTest, OpCodeOPMsg)
{
    OpCode      test = OpCode::OP_MSG;
    EXPECT_EQ(static_cast<OpCode>(2013), test);
}

TEST(MongoUtilTest, CompressionSnappy)
{
    Compression      test = Compression::Snappy;
    EXPECT_EQ(static_cast<Compression>(1), test);
}

TEST(MongoUtilTest, ConstantTest)
{
    EXPECT_EQ(4,  kSizeUInt32);
    EXPECT_EQ(16, kSizeOpMsgHeaderSize);
    EXPECT_EQ(21, kSizeOpMsgBlock);
    EXPECT_EQ(25, kSizeOpCompressedBlock);
    EXPECT_EQ(9,  kSizeOpCompressedMessage);
    EXPECT_EQ(26, kSizeSmallestPossibleMessage);
}
#if 0

static constexpr std::uint32_t  kSizeUInt32                     = 4;
static constexpr std::uint32_t  kSizeOpMsgHeaderSize            = 16;
static constexpr std::uint32_t  kSizeOpMsgBlock                 = 21;
static constexpr std::uint32_t  kSizeOpCompressedBlock          = 25;
static constexpr std::uint32_t  kSizeOpCompressedMessage        = 9;
static constexpr std::uint32_t  kSizeSmallestPossibleMessage    = 26;

#endif

