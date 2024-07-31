#include "gtest/gtest.h"

#include "ReadConcern.h"

#include "ThorSerialize/BsonThor.h"

#include <string>

using ThorsAnvil::Serialize::bsonExporter;
using ThorsAnvil::DB::Mongo::Level;
using ThorsAnvil::DB::Mongo::ReadConcern;

using namespace std::string_literals;

// enum class ReadConcern {Local, Available, Majority, Linearizable, Snapshot};
TEST(ReadConcernTest, ReadConcernLevelLocal)
{
    ReadConcern        a{Level::Local};
    std::stringstream   output;
    output << bsonExporter(a);

    EXPECT_EQ(  "\x16\x00\x00\x00"
                "\x02" "level\x00" "\x06\x00\x00\x00" "local\x00"
                "\x00"s,
                output.str());
}

// enum class ReadConcern {Local, Available, Majority, Linearizable, Snapshot};
TEST(ReadConcernTest, ReadConcernLevelAvailable)
{
    ReadConcern        a{Level::Available};
    std::stringstream   output;
    output << bsonExporter(a);

    EXPECT_EQ(  "\x1A\x00\x00\x00"
                "\x02" "level\x00" "\x0a\x00\x00\x00" "available\x00"
                "\x00"s,
                output.str());
}
// enum class ReadConcern {Local, Available, Majority, Linearizable, Snapshot};
TEST(ReadConcernTest, ReadConcernLevelMajority)
{
    ReadConcern        a{Level::Majority};
    std::stringstream   output;
    output << bsonExporter(a);

    EXPECT_EQ(  "\x19\x00\x00\x00"
                "\x02" "level\x00" "\x09\x00\x00\x00" "majority\x00"
                "\x00"s,
                output.str());
}
// enum class ReadConcern {Local, Available, Majority, Linearizable, Snapshot};
TEST(ReadConcernTest, ReadConcernLevelLinearizable)
{
    ReadConcern        a{Level::Linearizable};
    std::stringstream   output;
    output << bsonExporter(a);

    EXPECT_EQ(  "\x1d\x00\x00\x00"
                "\x02" "level\x00" "\x0d\x00\x00\x00" "linearizable\x00"
                "\x00"s,
                output.str());
}
// enum class ReadConcern {Local, Available, Majority, Linearizable, Snapshot};
TEST(ReadConcernTest, ReadConcernLevelSnapshot)
{
    ReadConcern        a{Level::Snapshot};
    std::stringstream   output;
    output << bsonExporter(a);

    EXPECT_EQ(  "\x19\x00\x00\x00"
                "\x02" "level\x00" "\x09\x00\x00\x00" "snapshot\x00"
                "\x00"s,
                output.str());
}


