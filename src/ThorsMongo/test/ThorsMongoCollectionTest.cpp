#include "gtest/gtest.h"

#include "test/Action.h"

#include "ThorsMongo.h"

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::DB;
using ThorsAnvil::DB::Mongo::Collection;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::OptReadConcern;
using ThorsAnvil::DB::Mongo::OptWriteConcern;
using ThorsAnvil::DB::Mongo::Level;
using ThorsAnvil::DB::Mongo::W;
using ThorsAnvil::DB::Mongo::ThorUT;

TEST(ThorsMongoCollectionTest, CreateNormal)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    DB              db(server, "test");
    Collection      c(db, "People");
}

TEST(ThorsMongoCollectionTest, CreateWithSquareBracket)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];
}

TEST(ThorsMongoCollectionTest, GetReadConcern)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    OptReadConcern  r = c.getReadConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoCollectionTest, SetReadConcernWithLevelMajor)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    ReadConcern     newRead{Level::Majority};
    OptReadConcern  r = c.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, c.getReadConcern());
}
TEST(ThorsMongoCollectionTest, SetReadConcernWithLevelSnapshot)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    ReadConcern     newRead{Level::Snapshot};
    OptReadConcern  r = c.setReadConcern(newRead);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newRead, c.getReadConcern());
}
TEST(ThorsMongoCollectionTest, ReadConcernPersist)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    ReadConcern     newRead{Level::Snapshot};

    // DB Object 1
    {
        Collection      c  = server["test"]["People"];
        c.setReadConcern(newRead);
    }

    // DB Object 2
    Collection      c  = server["test"]["People"];
    EXPECT_EQ(newRead, c.getReadConcern());
}

TEST(ThorsMongoCollectionTest, GetWriteConcern)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    OptWriteConcern r = c.getWriteConcern();
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
}
TEST(ThorsMongoCollectionTest, SetWriteConcernWithJFalse)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    WriteConcern    newWrite{{}, false, {}};
    OptWriteConcern r = c.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, c.getWriteConcern());
}
TEST(ThorsMongoCollectionTest, SetWriteConcernWithJTrue)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    WriteConcern    newWrite{{}, true, {}};
    OptWriteConcern r = c.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, c.getWriteConcern());
}
TEST(ThorsMongoCollectionTest, SetWriteConcernWithMajoritySet)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    WriteConcern    newWrite{W::Majority, true, {}};
    OptWriteConcern r = c.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, c.getWriteConcern());
}
TEST(ThorsMongoCollectionTest, SetWriteConcernWithW1AndTimeout)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    Collection      c  = server["test"]["People"];

    WriteConcern    newWrite{1, true, 12};
    OptWriteConcern r = c.setWriteConcern(newWrite);
    EXPECT_FALSE(r.has_value());        // Should be just a default read concern
    EXPECT_EQ(newWrite, c.getWriteConcern());
}
TEST(ThorsMongoCollectionTest, WriteConcernPersist)
{
    ThorsMongo      server({"localhost"}, {ThorUT::Build});
    WriteConcern    newWrite{1, true, 12};

    // DB Object 1
    {
        Collection      c  = server["test"]["People"];
        c.setWriteConcern(newWrite);
    }

    // DB Object 2
    Collection      c  = server["test"]["People"];
    EXPECT_EQ(newWrite, c.getWriteConcern());
}


