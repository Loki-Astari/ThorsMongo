#include "gtest/gtest.h"

#include "WriteConcern.h"

#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/BsonThor.h"

#include <string>

using ThorsAnvil::Serialize::bsonExporter;
using ThorsAnvil::Serialize::PrinterInterface;
using PrinterConfig = ThorsAnvil::Serialize::PrinterInterface::PrinterConfig;

using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::W;

using namespace std::string_literals;

TEST(WriteConcernTest, FieldJ)
{
    WriteConcern        a{{}, true, {}};
    std::stringstream   output;
    output << bsonExporter(a, PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(  "\x09\x00\x00\x00"
                "\x08" "j\x00" "\x01"
                "\x00"s,
                output.str());
}

TEST(WriteConcernTest, FieldWIntegerAndJ)
{
    WriteConcern        a{7, false, {}};
    std::stringstream   output;
    output << bsonExporter(a, PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(  "\x10\x00\x00\x00"
                "\x10" "w\x00" "\x07\x00\x00\x00"
                "\x08" "j\x00" "\x00"
                "\x00"s,
                output.str());
}

TEST(WriteConcernTest, FieldWMajorityAndJ)
{
    WriteConcern        a{W::Majority, false, {}};
    std::stringstream   output;
    output << bsonExporter(a, PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(  "\x19\x00\x00\x00"
                "\x02" "w\x00" "\x09\x00\x00\x00" "majority\x00"
                "\x08" "j\x00" "\x00"
                "\x00"s,
                output.str());
}
TEST(WriteConcernTest, FieldWTimeoutAndJ)
{
    WriteConcern        a{{}, false, 256};
    std::stringstream   output;
    output << bsonExporter(a, PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(  "\x17\x00\x00\x00"
                "\x08" "j\x00" "\x00"
                "\x10" "wtimeout\x00" "\x00\x01\x00\x00"
                "\x00"s,
                output.str());
}
TEST(WriteConcernTest, FieldWTimeoutWMajorityAndJ)
{
    WriteConcern        a{W::Majority, false, 257};
    std::stringstream   output;
    output << bsonExporter(a, PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(  "\x27\x00\x00\x00"
                "\x02" "w\x00" "\x09\x00\x00\x00" "majority\x00"
                "\x08" "j\x00" "\x00"
                "\x10" "wtimeout\x00" "\x01\x01\x00\x00"
                "\x00"s,
                output.str());
}
TEST(WriteConcernTest, FieldWTimeoutWIntAndJ)
{
    WriteConcern        a{15, false, 258};
    std::stringstream   output;
    output << bsonExporter(a, PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(  "\x1E\x00\x00\x00"
                "\x10" "w\x00" "\x0f\x00\x00\x00"
                "\x08" "j\x00" "\x00"
                "\x10" "wtimeout\x00" "\x02\x01\x00\x00"
                "\x00"s,
                output.str());
}

