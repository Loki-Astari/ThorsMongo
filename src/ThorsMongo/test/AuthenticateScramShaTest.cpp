#include "gtest/gtest.h"

#include "AuthenticateScramSha.h"
#include "test/TestMessageHandler.h"

using ThorsAnvil::DB::Mongo::Auth::ScramSha256::AuthInit;
using ThorsAnvil::DB::Mongo::Auth::ScramSha256::AuthCont;
using ThorsAnvil::DB::Mongo::Auth::ScramSha256::AuthReply;
using ThorsAnvil::DB::Mongo::MessageId;
using ThorsAnvil::DB::Mongo::Internal::getMessageId;
using ThorsAnvil::DB::Mongo::ParserConfig;

TEST(AuthenticateScramShaTest, AuthInit)
{
    AuthInit        authInit("db", "mechanism", "DLDLLDDLDLDLDLDLDL");

    EXPECT_EQ(1, authInit.saslStart);
    EXPECT_EQ("mechanism", authInit.mechanism);
    EXPECT_EQ("db", authInit.$db);
    EXPECT_EQ(0, authInit.payload.getType());
    EXPECT_EQ("DLDLLDDLDLDLDLDLDL", authInit.payload.getData());
}

TEST(AuthenticateScramShaTest, AuthCont)
{
    AuthCont          authCont(21, "db", "GHHFHHFHFHFFHHF");

    EXPECT_EQ(1, authCont.saslContinue);
    EXPECT_EQ(21, authCont.conversationId);
    EXPECT_EQ("db", authCont.$db);
    EXPECT_EQ(0, authCont.payload.getType());
    EXPECT_EQ("GHHFHHFHFHFFHHF", authCont.payload.getData());
}

TEST(AuthenticateScramShaTest, AuthReply)
{
    AuthReply         authReply;

    EXPECT_EQ(0, authReply.payload.getType());
}

TEST(AuthenticateScramShaTest, AuthInitFailBadReply)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                      // Remove CodeName to cause Error
                      //   "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateScramShaTest, AuthInitFail)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                    // Set the OK to false.
                    // This will force an exception to be thrown.
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);


    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateScramShaTest, AuthContFailBadmessage)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha256.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 2 to AuthCont
                           "\xa3\x00\x00\x00"           // Size
                           "\x04\x00\x00\x00"           // Id
                           "\x01\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flags
                           "\x00"                       // Kind 0
                           "\x8e\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                    // Remove "payload" section to cause
                    // parsing to fail
                    //     "\x05"   "payload\x00"               "\x2e\x00\x00\x00"  "\x00"  // Binary
                                "\x76\x3d\x6d\x43\x56\x68\x4f\x62\x66\x37\x57\x71\x63\x4e\x72\x38"  // 1.0
                                "\x69\x50\x6c\x78\x39\x34\x66\x73\x49\x48\x52\x36\x62\x73\x54\x4c"  // 2.0
                                "\x5a\x4f\x43\x66\x39\x58\x64\x68\x4b\x45\x72\x2b\x41\x3d"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}
TEST(AuthenticateScramShaTest, AuthContFail)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 2 to AuthCont
                           "\xa3\x00\x00\x00"           // Size
                           "\x04\x00\x00\x00"           // Id
                           "\x01\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flags
                           "\x00"                       // Kind 0
                           "\x8e\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x2e\x00\x00\x00"  "\x00"  // Binary
                                "\x76\x3d\x6d\x43\x56\x68\x4f\x62\x66\x37\x57\x71\x63\x4e\x72\x38"  // 1.0
                                "\x69\x50\x6c\x78\x39\x34\x66\x73\x49\x48\x52\x36\x62\x73\x54\x4c"  // 2.0
                                "\x5a\x4f\x43\x66\x39\x58\x64\x68\x4b\x45\x72\x2b\x41\x3d"
                // Set OK to false.
                // This will force an exception.
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateScramShaTest, AuthCont2FailBadMessage)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 2 to AuthCont
                           "\xa3\x00\x00\x00"           // Size
                           "\x04\x00\x00\x00"           // Id
                           "\x01\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flags
                           "\x00"                       // Kind 0
                           "\x8e\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x2e\x00\x00\x00"  "\x00"  // Binary
                                "\x76\x3d\x6d\x43\x56\x68\x4f\x62\x66\x37\x57\x71\x63\x4e\x72\x38"  // 1.0
                                "\x69\x50\x6c\x78\x39\x34\x66\x73\x49\x48\x52\x36\x62\x73\x54\x4c"  // 2.0
                                "\x5a\x4f\x43\x66\x39\x58\x64\x68\x4b\x45\x72\x2b\x41\x3d"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 3 to AuthCont2
                           "\x75\x00\x00\x00"           // Size
                           "\x06\x00\x00\x00"           // Id
                           "\x02\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                    // Remove the BSON SIZE
                    // This will cause parsing to fail
                    //     "\x60\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x01"
                           "\x05"   "payload\x00"               "\x00\x00\x00\x00"      "\x00"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"      "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"      "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);


    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateScramShaTest, AuthCont2Fail)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 2 to AuthCont
                           "\xa3\x00\x00\x00"           // Size
                           "\x04\x00\x00\x00"           // Id
                           "\x01\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flags
                           "\x00"                       // Kind 0
                           "\x8e\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x2e\x00\x00\x00"  "\x00"  // Binary
                                "\x76\x3d\x6d\x43\x56\x68\x4f\x62\x66\x37\x57\x71\x63\x4e\x72\x38"  // 1.0
                                "\x69\x50\x6c\x78\x39\x34\x66\x73\x49\x48\x52\x36\x62\x73\x54\x4c"  // 2.0
                                "\x5a\x4f\x43\x66\x39\x58\x64\x68\x4b\x45\x72\x2b\x41\x3d"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 3 to AuthCont2
                           "\x75\x00\x00\x00"           // Size
                           "\x06\x00\x00\x00"           // Id
                           "\x02\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\x60\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x01"
                           "\x05"   "payload\x00"               "\x00\x00\x00\x00"      "\x00"
                    // Set OK to false
                    // This will force an exception.
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"      "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"      "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);


    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateScramShaTest, AuthCont2OKNotDone)
{
    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 2 to AuthCont
                           "\xa3\x00\x00\x00"           // Size
                           "\x04\x00\x00\x00"           // Id
                           "\x01\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flags
                           "\x00"                       // Kind 0
                           "\x8e\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x2e\x00\x00\x00"  "\x00"  // Binary
                                "\x76\x3d\x6d\x43\x56\x68\x4f\x62\x66\x37\x57\x71\x63\x4e\x72\x38"  // 1.0
                                "\x69\x50\x6c\x78\x39\x34\x66\x73\x49\x48\x52\x36\x62\x73\x54\x4c"  // 2.0
                                "\x5a\x4f\x43\x66\x39\x58\x64\x68\x4b\x45\x72\x2b\x41\x3d"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 3 to AuthCont2
                           "\x75\x00\x00\x00"           // Size
                           "\x06\x00\x00\x00"           // Id
                           "\x02\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\x60\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                // Set Done to false
                // This will generate an exception.
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x00\x00\x00\x00"      "\x00"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"      "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"      "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateScramShaTest, AuthCont2OK)
{
    //std::int32_t        conversationId;     // Seen
    //bool                done;               // Seen
    //Binary              payload;            // Seen

    using namespace std::string_literals;
    // OP_MSG (with truncated AuthReply (see AuthenticateScramSha.h)
    std::string mongoData(
                           // Response 1 to AuthInit
                           "\xe2\x00\x00\x00"           // Size
                           "\x02\x00\x00\x00"           // Id
                           "\x00\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\xcd\x00\x00\x00"           // BSON Size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x6d\x00\x00\x00"  "\x00"  // Binary
                                "\x72\x3d\x66\x79\x6b\x6f\x2b\x64\x32\x6c\x62\x62\x46\x67\x4f\x4e"  // 1.0
                                "\x52\x76\x39\x71\x6b\x78\x64\x61\x77\x4c\x39\x42\x69\x76\x71\x64"  // 2.0
                                "\x6d\x61\x54\x72\x6e\x4d\x77\x63\x6e\x61\x73\x70\x5a\x64\x61\x72"  // 3.0
                                "\x59\x75\x51\x78\x76\x62\x7a\x4e\x2f\x61\x2c\x73\x3d\x49\x38\x71"  // 4.0
                                "\x64\x61\x2f\x77\x57\x31\x78\x39\x4e\x63\x58\x6a\x78\x45\x43\x69"  // 5.0
                                "\x72\x39\x57\x75\x72\x57\x74\x54\x35\x2f\x38\x62\x53\x52\x65\x45"  // 6.0
                                "\x52\x78\x41\x3d\x3d\x2c\x69\x3d\x31\x35\x30\x30\x30"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 2 to AuthCont
                           "\xa3\x00\x00\x00"           // Size
                           "\x04\x00\x00\x00"           // Id
                           "\x01\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flags
                           "\x00"                       // Kind 0
                           "\x8e\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x00"
                           "\x05"   "payload\x00"               "\x2e\x00\x00\x00"  "\x00"  // Binary
                                "\x76\x3d\x6d\x43\x56\x68\x4f\x62\x66\x37\x57\x71\x63\x4e\x72\x38"  // 1.0
                                "\x69\x50\x6c\x78\x39\x34\x66\x73\x49\x48\x52\x36\x62\x73\x54\x4c"  // 2.0
                                "\x5a\x4f\x43\x66\x39\x58\x64\x68\x4b\x45\x72\x2b\x41\x3d"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"  "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"  "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"

                           // Response 3 to AuthCont2
                           "\x75\x00\x00\x00"           // Size
                           "\x06\x00\x00\x00"           // Id
                           "\x02\x00\x00\x00"           // Response To
                           "\xdd\x07\x00\x00"           // OP_MSG
                           "\x00\x00\x00\x00"           // Flag
                           "\x00"                       // Kind 0
                           "\x60\x00\x00\x00"           // BSON size
                           "\x10"   "conversationId\x00"        "\x01\x00\x00\x00"
                           "\x08"   "done\x00"                  "\x01"
                           "\x05"   "payload\x00"               "\x00\x00\x00\x00"      "\x00"
                           "\x01"   "ok\x00"                    "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                           "\x02"   "errmsg\x00"                "\x01\x00\x00\x00"      "\x00"
                           "\x02"   "codeName\x00"              "\x01\x00\x00\x00"      "\x00"
                           "\x10"   "code\x00"                  "\x00\x00\x00\x00"
                           "\x00"s);


    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    getMessageId()  = 0;
    auto action = [&](){ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate(messageSender, {"user", "pass", "db"});};

    EXPECT_NO_THROW(
        action()
    );
}

