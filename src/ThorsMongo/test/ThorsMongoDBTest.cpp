#include "gtest/gtest.h"

#include "ThorsMongo.h"

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::DB;

TEST(ThorsMongoDBTest, CreateNormal)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});
    DB              db(server, "test");
}

TEST(ThorsMongoDBTest, CreateWithSquareBracket)
{
    ThorsMongo      server({"localhost"}, {"test", "testPassword", "test"});
    DB              db  = server["test"];
}

