#include "gtest/gtest.h"
#include "ReadConcern.h"

#include "ThorsMongo.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Certificate;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::OptReadConcern;
using ThorsAnvil::DB::Mongo::OptWriteConcern;
using ThorsAnvil::DB::Mongo::Level;
using ThorsAnvil::DB::Mongo::W;
using ThorsAnvil::DB::Mongo::ThorUT;

TEST(ThorsMongoTest, Create)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
}

TEST(ThorsMongoTest, CreateCertificate)
{
    SocketSetUp     winSocketInit;
    auto action = []()
    {
        ThorsMongo      server({"localhost"}, Certificate{});
    };

    EXPECT_THROW(action(), std::runtime_error);
}

TEST(ThorsMongoTest, CreateNoCompression)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build}, Compression::None);
}
TEST(ThorsMongoTest, CreateExplicitSnappy)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build}, Compression::Snappy);
}
TEST(ThorsMongoTest, CreateExplicicZLib)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build}, Compression::ZLib);
}
TEST(ThorsMongoTest, CreateExplicitZStd)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build}, Compression::ZStd);
}

TEST(ThorsMongoTest, CreateWithClientName)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build}, Compression::Snappy, {"MyApplication"});
}
TEST(ThorsMongoTest, CreateWithClientNamePlatform)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build}, Compression::Snappy, {"MyApplication", "Platform"});
}

TEST(ThorsMongoTest, GetReadConcern)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    OptReadConcern  r = server.getReadConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoTest, SetReadConcernWithLevelMajor)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    ReadConcern     newRead{Level::Majority};
    OptReadConcern  r = server.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, server.getReadConcern());
}
TEST(ThorsMongoTest, SetReadConcernWithLevelSnapshot)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    ReadConcern     newRead{Level::Snapshot};
    OptReadConcern  r = server.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, server.getReadConcern());
}

TEST(ThorsMongoTest, GetWriteConcern)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    OptWriteConcern r = server.getWriteConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoTest, SetWriteConcernWithJFalse)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    WriteConcern    newWrite{{}, false, {}};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}
TEST(ThorsMongoTest, SetWriteConcernWithJTrue)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    WriteConcern    newWrite{{}, true, {}};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}
TEST(ThorsMongoTest, SetWriteConcernWithMajoritySet)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    WriteConcern    newWrite{W::Majority, true, {}};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}
TEST(ThorsMongoTest, SetWriteConcernWithW1AndTimeout)
{
    SocketSetUp     winSocketInit;
    ThorsMongo      server({"localhost"}, {ThorUT::Build});

    WriteConcern    newWrite{1, true, 12};
    OptWriteConcern r = server.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, server.getWriteConcern());
}

