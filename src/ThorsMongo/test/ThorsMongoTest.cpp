#include "gtest/gtest.h"
#include "ReadConcern.h"

#include "ThorsMongo.h"

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Certificate;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::OptReadConcern;
using ThorsAnvil::DB::Mongo::OptWriteConcern;
using ThorsAnvil::DB::Mongo::Level;
using ThorsAnvil::DB::Mongo::W;

TEST(ThorsMongoTest, Create)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});
}

TEST(ThorsMongoTest, CreateCertificate)
{
    auto action = []()
    {
        ThorsMongo      server({"localhost"}, Certificate{});
    };

    EXPECT_THROW(action(), std::runtime_error);
}

TEST(ThorsMongoTest, CreateNoCompression)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"}, Compression::None);
}
TEST(ThorsMongoTest, CreateExplicitSnappy)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"}, Compression::Snappy);
}
TEST(ThorsMongoTest, CreateExplicicZLib)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"}, Compression::ZLib);
}
TEST(ThorsMongoTest, CreateExplicitZStd)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"}, Compression::ZStd);
}

TEST(ThorsMongoTest, CreateWithClientName)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"}, Compression::Snappy, {"MyApplication"});
}
TEST(ThorsMongoTest, CreateWithClientNamePlatform)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"}, Compression::Snappy, {"MyApplication", "Platform"});
}

TEST(ThorsMongoTest, GetReadConcern)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    OptReadConcern  r = server.getReadConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoTest, SetReadConcernWithLevelMajor)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    ReadConcern     newRead{Level::Majority};
    OptReadConcern  r = server.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, server.getReadConcern());
}
TEST(ThorsMongoTest, SetReadConcernWithLevelSnapshot)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    ReadConcern     newRead{Level::Snapshot};
    OptReadConcern  r = server.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, server.getReadConcern());
}

TEST(ThorsMongoTest, GetWriteConcern)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    OptWriteConcern r = server.getWriteConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoTest, SetWriteConcernWithJFalse)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    WriteConcern    newWrite{{}, false, {}};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}
TEST(ThorsMongoTest, SetWriteConcernWithJTrue)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    WriteConcern    newWrite{{}, true, {}};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}
TEST(ThorsMongoTest, SetWriteConcernWithMajoritySet)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    WriteConcern    newWrite{W::Majority, true, {}};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}
TEST(ThorsMongoTest, SetWriteConcernWithW1AndTimeout)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});

    WriteConcern    newWrite{1, true, 12};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}

