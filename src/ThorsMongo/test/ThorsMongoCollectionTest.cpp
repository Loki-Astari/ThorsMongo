#include "gtest/gtest.h"

#include "ThorsMongo.h"

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::DB;
using ThorsAnvil::DB::Mongo::Collection;

TEST(ThorsMongoCollectionTest, CreateNormal)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});
    DB              db(server, "test");
    Collection      c(db, "People");
}

TEST(ThorsMongoCollectionTest, CreateWithSquareBracket)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});
    Collection      c  = server["test"]["People"];
}
