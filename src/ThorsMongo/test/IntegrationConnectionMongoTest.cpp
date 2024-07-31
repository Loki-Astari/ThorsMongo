#include "gtest/gtest.h"

#include "ConnectionMongo.h"
#include "ThorsMongo.h"
#include "Authenticate.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "AuthenticateScramSha.h"
#include "ThorSerialize/Traits.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::ConnectionMongo;
using ThorsAnvil::DB::Mongo::MongoMessageHandler;
using ThorsAnvil::DB::Mongo::Authenticate;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Client;
using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::InsertResult;

TEST(IntegrationConnectionMongoTest, connectToMongoWithAuthenticator)
{
    ConnectionMongo     mongo({"localhost", 27017});
    MongoMessageHandler sender(mongo);
    Authenticate        authenticate;

    authenticate.addAuthenticator("SCRAM-SHA-256", ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate);
    authenticate.handShake(sender, {"test", "testPassword", "test"}, Compression::None, Client{"IntegrationConnectionMongoTest::connectToMongo"});
}

TEST(IntegrationConnectionMongoTest, connectToMongoWithAuthenticatorUsingSnappy)
{
    ConnectionMongo     mongo({"localhost", 27017});
    MongoMessageHandler sender(mongo);
    Authenticate        authenticate;

    authenticate.addAuthenticator("SCRAM-SHA-256", ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate);
    authenticate.handShake(sender, {"root", "underMongo", "admin"}, Compression::Snappy, Client{"IntegrationConnectionMongoTest::connectToMongo"});
}

TEST(IntegrationConnectionMongoTest, insertData)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}};

    InsertResult        result = mongo["test"]["People"].insert(people);

    EXPECT_EQ(1, result.ok);
    EXPECT_EQ("", result.errmsg);
    EXPECT_EQ("", result.codeName);
    EXPECT_EQ(2, result.n);
    EXPECT_EQ(2, result.inserted.size());
    EXPECT_EQ(0, result.writeErrors.size());
}

