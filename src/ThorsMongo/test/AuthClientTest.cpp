#include "gtest/gtest.h"

#include "AuthClient.h"
#include "ThorSerialize/JsonThor.h"

using ThorsAnvil::DB::Mongo::Auth::Driver;
using ThorsAnvil::DB::Mongo::Auth::OS;
using ThorsAnvil::DB::Mongo::Auth::Application;
using ThorsAnvil::DB::Mongo::Auth::Client;
using ThorsAnvil::Serialize::jsonExporter;

TEST(AuthClientTest, CreateDriver)
{
    Driver      driver;
    EXPECT_EQ("ThorsAnvil::Mongo::Driver", driver.name);
    EXPECT_EQ("v2.0", driver.version);
}

TEST(AuthClientTest, CreateOS)
{
    OS          os;
    EXPECT_NE("", os.type);
    EXPECT_TRUE(os.name.has_value());
    EXPECT_TRUE(os.architecture.has_value());
    EXPECT_TRUE(os.version.has_value());
}

TEST(AuthClientTest, CreateApplication)
{
    Application application("AppName");
    EXPECT_EQ("AppName", application.name);
}

TEST(AuthClientTest, CreateClientWithName)
{
    Client      client("AppNameClient");
    EXPECT_EQ("AppNameClient", client.application.name);
    EXPECT_NE("", client.os.type);
    EXPECT_TRUE(client.os.name.has_value());
    EXPECT_TRUE(client.os.architecture.has_value());
    EXPECT_TRUE(client.os.version.has_value());
    EXPECT_FALSE(client.platform.has_value());
}

TEST(AuthClientTest, CreateClientWithNamePlatform)
{
    Client      client("AppNameClient", "MAC");
    EXPECT_EQ("AppNameClient", client.application.name);
    EXPECT_NE("", client.driver.name);
    EXPECT_NE("", client.driver.version);
    EXPECT_NE("", client.os.type);
    EXPECT_TRUE(client.os.name.has_value());
    EXPECT_TRUE(client.os.architecture.has_value());
    EXPECT_TRUE(client.os.version.has_value());
    EXPECT_TRUE(client.platform.has_value());
    EXPECT_EQ("MAC", client.platform);
}

TEST(AuthClientTest, SerizlzeClient)
{
    std::stringstream output;

    Client      client("AppNameClient", "MAC");
    output << jsonExporter(client);

    EXPECT_TRUE(static_cast<bool>(output));
}
