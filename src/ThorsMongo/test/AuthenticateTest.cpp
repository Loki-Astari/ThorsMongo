#include "gtest/gtest.h"

#include "Authenticate.h"
#include "test/TestMessageHandler.h"

#include <sstream>

namespace Auth = ThorsAnvil::DB::Mongo::Auth;
using ThorsAnvil::DB::Mongo::Authenticate;
using ThorsAnvil::DB::Mongo::MessageId;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Client;
using ThorsAnvil::DB::Mongo::Internal::getMessageId;
using ThorsAnvil::DB::Mongo::MessageHandler;

TEST(AuthenticateTest, CreateDriver)
{
    // Should be default constructable.
    Auth::Driver    driver;

    EXPECT_EQ("ThorsAnvil::Mongo::Driver", driver.name);
    EXPECT_EQ("v2.0", driver.version);
}

TEST(AuthenticateTest, CreateOS)
{
    // Should be default constructable.
    Auth::OS        os;

    EXPECT_NE("", os.type);
    if (os.name) {
        EXPECT_NE("", os.name.value());
    }
    if (os.architecture) {
        EXPECT_NE("", os.architecture.value());
    }
    if (os.version) {
        EXPECT_NE("", os.version.value());
    }
}

TEST(AuthenticateTest, CreateApplication)
{
    Auth::Application   application("CreateApplication");

    EXPECT_EQ("CreateApplication", application.name);
}

TEST(AuthenticateTest, CreateClientIgnorePlatform)
{
    Auth::Client        client("CreateClient", {});

    EXPECT_EQ("CreateClient", client.application.name);

    EXPECT_EQ("ThorsAnvil::Mongo::Driver", client.driver.name);
    EXPECT_EQ("v2.0", client.driver.version);

    EXPECT_NE("", client.os.type);
    if (client.os.name) {
        EXPECT_NE("", client.os.name.value());
    }
    if (client.os.architecture) {
        EXPECT_NE("", client.os.architecture.value());
    }
    if (client.os.version) {
        EXPECT_NE("", client.os.version.value());
    }
}

TEST(AuthenticateTest, CreateClientNoPlatform)
{
    Auth::Client        client("CreateClient", {});
    EXPECT_FALSE(client.platform);
}
TEST(AuthenticateTest, CreateClientEmptyPlatform)
{
    Auth::Client        client("CreateClient", {""});
    EXPECT_FALSE(client.platform);
}
TEST(AuthenticateTest, CreateClientWithPlatform)
{
    Auth::Client        client("CreateClient", {"Thor"});
    EXPECT_TRUE(client.platform);
    EXPECT_EQ("Thor", client.platform);
}

TEST(AuthenticateTest, HandShake)
{
    Auth::HandShake     handshake("user", "db", Client{"application", {"Platform"}}, {});

    EXPECT_EQ(1, handshake.hello);
    EXPECT_EQ("db.user", handshake.saslSupportedMechs);
    EXPECT_EQ("db", handshake.$db);
    // TODO: hostInfo

    EXPECT_EQ("application", handshake.client.application.name);

    EXPECT_EQ("ThorsAnvil::Mongo::Driver", handshake.client.driver.name);
    EXPECT_EQ("v2.0", handshake.client.driver.version);

    EXPECT_NE("", handshake.client.os.type);
    if (handshake.client.os.name) {
        EXPECT_NE("", handshake.client.os.name.value());
    }
    if (handshake.client.os.architecture) {
        EXPECT_NE("", handshake.client.os.architecture.value());
    }
    if (handshake.client.os.version) {
        EXPECT_NE("", handshake.client.os.version.value());
    }
    EXPECT_TRUE(handshake.client.platform);
    EXPECT_EQ("Platform", handshake.client.platform.value());

    EXPECT_EQ(0, handshake.compression.size());
}

TEST(AuthenticateTest, HandShakeMultipleCompression)
{
    Auth::HandShake     handshake("user", "db", Client{"application", {"Platform"}}, {"Snappy", "LZH"});

    EXPECT_EQ(1, handshake.hello);
    EXPECT_EQ("db.user", handshake.saslSupportedMechs);
    EXPECT_EQ("db", handshake.$db);
    // TODO: hostInfo

    ASSERT_EQ(2, handshake.compression.size());
    EXPECT_EQ("Snappy", handshake.compression[0]);
    EXPECT_EQ("LZH", handshake.compression[1]);
}

TEST(AuthenticateTest, HandShakeFail)
{
    using namespace std::string_literals;
    std::string mongoData("\x26\x00\x00\x00"      // Message Size
                          "\x00\x00\x00\x00"      // Message ID
                          "\x00\x00\x00\x00"      // Response ID
                          "\xdd\x07\x00\x00"      // OP_MSG
                          "\x00\x00\x00\x00"      // Flags
                          "\x00"                  // Kind
                          "\x11\x00\x00\x00"      // BSON Size
                          "\x01"  "ok\x00"   "\x00\x00\x00\x00\x00\x00\x00\x00"      // ok = 0
                          "\x00"s);               // BSON terminator

    MessageId replyId = getMessageId();
    char      update  = static_cast<char>(replyId);
    mongoData[8]      = update;

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    Authenticate    authenticate;
    auto action = [&](){authenticate.handShake(messageSender, {"user", "password", "database"}, Compression::None, Auth::Client{"appName"});};
    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateTest, HandShakeFailBadReply)
{
    using namespace std::string_literals;
    std::string mongoData("\x26\x00\x00\x00"      // Message Size
                          "\x00\x00\x00\x00"      // Message ID
                          "\x00\x00\x00\x00"      // Response ID
                          "\xdd\x07\x00\x00"s);   // OP_MSG

    MessageId replyId = getMessageId();
    char      update  = static_cast<char>(replyId);
    mongoData[8]      = update;

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    Authenticate    authenticate;
    auto action = [&](){authenticate.handShake(messageSender, {"user", "password", "database"}, Compression::None, Auth::Client{"appName"});};
    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateTest, NoAuthMechanism)
{
    using namespace std::string_literals;
    std::string mongoData("\x4A\x00\x00\x00"      // Message Size
                          "\x00\x00\x00\x00"      // Message ID
                          "\x00\x00\x00\x00"      // Response ID
                          "\xdd\x07\x00\x00"      // OP_MSG
                          "\x00\x00\x00\x00"      // Flags
                          "\x00"                  // Kind
                          "\x35\x00\x00\x00"      // BSON Size                                4           =>  4
                          "\x01" "ok\x00" "\x00\x00\x00\x00\x00\x00\xf0\x3f"  //              1 + 3 + 8   => 12   16
                          "\x04" "saslSupportedMechs\x00"                     //              1 + 19      => 20   36
                              "\x10\x00\x00\x00"      // ARRAY Size                           4           =>  4   40
                              "\x02" "0\x00" "\x04\x00\x00\x00" "Bob\x00"     //              1 + 2 + 4+4 => 11   51
                              "\x00"                  // ARRAY Terminator                     1           =>  1   52
                          "\x00"s);                // BSON terminator                         1           =>  1   53 => 53 + 21 => 74

    MessageId replyId = getMessageId();
    char      update  = static_cast<char>(replyId);
    mongoData[8]      = update;

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    Authenticate    authenticate;
    auto action = [&](){authenticate.handShake(messageSender, {"user", "password", "database"}, Compression::None, Auth::Client{"appName"});};
    EXPECT_THROW(
        action(),
        std::runtime_error
    );
}

TEST(AuthenticateTest, CallAuthMechanism)
{
    using namespace std::string_literals;
    std::string mongoData("\x4A\x00\x00\x00"      // Message Size
                          "\x00\x00\x00\x00"      // Message ID
                          "\x00\x00\x00\x00"      // Response ID
                          "\xdd\x07\x00\x00"      // OP_MSG
                          "\x00\x00\x00\x00"      // Flags
                          "\x00"                  // Kind
                          "\x35\x00\x00\x00"      // BSON Size
                          "\x01" "ok\x00" "\x00\x00\x00\x00\x00\x00\xf0\x3f"
                          "\x04" "saslSupportedMechs\x00" 
                              "\x10\x00\x00\x00"      // ARRAY Size
                              "\x02" "0\x00" "\x04\x00\x00\x00" "Bob\x00"
                              "\x00"                  // ARRAY Terminator
                          "\x00"s);               // BSON terminator

    MessageId replyId = getMessageId();
    char      update  = static_cast<char>(replyId);
    mongoData[8]      = update;

    TestMessageHandler  messageSender;;
    messageSender.stream << mongoData;

    bool            called = false;
    Authenticate    authenticate;
    authenticate.addAuthenticator("Bob", [&](MessageHandler&, Auth::UserNamePassword const&)
                                         {
                                            called = true;
                                         });

    auto action = [&](){authenticate.handShake(messageSender, {"user", "password", "database"}, Compression::None, Auth::Client{"appName"});};
    EXPECT_NO_THROW(action());
    EXPECT_TRUE(called);
}
