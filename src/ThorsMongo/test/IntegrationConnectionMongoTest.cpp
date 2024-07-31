#include "gtest/gtest.h"

#include "ConnectionMongo.h"
#include "ThorsMongo.h"
#include "Authenticate.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "AuthenticateScramSha.h"
#include "ThorSerialize/Traits.h"

using ThorsAnvil::DB::Mongo::ConnectionMongo;
using ThorsAnvil::DB::Mongo::MongoMessageHandler;
using ThorsAnvil::DB::Mongo::Authenticate;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Client;

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

