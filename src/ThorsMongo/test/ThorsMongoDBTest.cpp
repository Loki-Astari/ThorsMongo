#include "gtest/gtest.h"

#include "ThorsMongo.h"

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::DB;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::OptReadConcern;
using ThorsAnvil::DB::Mongo::OptWriteConcern;
using ThorsAnvil::DB::Mongo::Level;
using ThorsAnvil::DB::Mongo::W;
using ThorsAnvil::DB::Mongo::ThorUT;

TEST(ThorsMongoDBTest, CreateNormal)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");
}

TEST(ThorsMongoDBTest, CreateWithSquareBracket)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db  = server["test"];
}

TEST(ThorsMongoDBTest, GetReadConcern)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    OptReadConcern  r = db.getReadConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoDBTest, SetReadConcernWithLevelMajor)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    ReadConcern     newRead{Level::Majority};
    OptReadConcern  r = db.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, db.getReadConcern());
}
TEST(ThorsMongoDBTest, SetReadConcernWithLevelSnapshot)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    ReadConcern     newRead{Level::Snapshot};
    OptReadConcern  r = db.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, db.getReadConcern());
}
TEST(ThorsMongoDBTest, ReadConcernPersist)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    ReadConcern     newRead{Level::Snapshot};

    // DB Object 1
    {
        DB              db(server, "test");
        db.setReadConcern(newRead);
    }

    // DB Object 2
    DB              db(server, "test");
    EXPECT_EQ(newRead, db.getReadConcern());
}

TEST(ThorsMongoDBTest, GetWriteConcern)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    OptWriteConcern r = db.getWriteConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoDBTest, SetWriteConcernWithJFalse)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    WriteConcern    newWrite{{}, false, {}};
    OptWriteConcern r = db.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, db.getWriteConcern());
}
TEST(ThorsMongoDBTest, SetWriteConcernWithJTrue)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    WriteConcern    newWrite{{}, true, {}};
    OptWriteConcern r = db.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, db.getWriteConcern());
}
TEST(ThorsMongoDBTest, SetWriteConcernWithMajoritySet)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    WriteConcern    newWrite{W::Majority, true, {}};
    OptWriteConcern r = db.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, db.getWriteConcern());
}
TEST(ThorsMongoDBTest, SetWriteConcernWithW1AndTimeout)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");

    WriteConcern    newWrite{1, true, 12};
    OptWriteConcern r = db.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, db.getWriteConcern());
}
TEST(ThorsMongoDBTest, WriteConcernPersist)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    WriteConcern    newWrite{1, true, 12};

    // DB Object 1
    {
        DB              db(server, "test");
        db.setWriteConcern(newWrite);
    }

    // DB Object 2
    DB              db(server, "test");
    EXPECT_EQ(newWrite, db.getWriteConcern());
}


