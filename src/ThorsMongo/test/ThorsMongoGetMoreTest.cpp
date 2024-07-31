#include "gtest/gtest.h"

#include "ThorsMongo.h"
#include "test/Action.h"


using ThorsAnvil::DB::Mongo::GetMoreConfig;
using ThorsAnvil::DB::Mongo::Action::GetMore;
using ThorsAnvil::DB::Mongo::CursorFirst;
using ThorsAnvil::DB::Mongo::PrinterConfig;

TEST(ThorsMongGetMoreTest, GetMoreConfigDefault)
{
    GetMoreConfig    config;

    EXPECT_FALSE(config.getBatchSize().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongGetMoreTest, GetMoreConfigSetBatchSize)
{
    GetMoreConfig    config;

    config.setBatchSize(12);
    EXPECT_TRUE(config.getBatchSize().has_value());

    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongGetMoreTest, GetMoreConfigSetMaxTimeMS)
{
    GetMoreConfig    config;

    EXPECT_FALSE(config.getBatchSize().has_value());
    config.setMaxTimeMS(23);
    EXPECT_TRUE(config.getMaxTimeMS().has_value());

    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongGetMoreTest, GetMoreConfigSetComment)
{
    GetMoreConfig    config;

    EXPECT_FALSE(config.getBatchSize().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    config.setComment("Hi There");
    EXPECT_TRUE(config.getComment().has_value());
}

TEST(ThorsMongGetMoreTest, MessageGetMore)
{
    std::string             collection("col");
    std::string             db("db");
    CursorFirst<People>     cursor;

    GetMore<People>         getMore(collection, db, GetMoreConfig{}, cursor);

    EXPECT_EQ(0x36, getMore.getSizeBson());

    std::stringstream   output;
    getMore.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x36\x00\x00\x00"
            "\x12" "getMore\x00"    "\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x02" "collection\x00" "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"        "\x03\x00\x00\x00"  "db\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongGetMoreTest, MessageGetMoreWithBatchSize)
{
    std::string             collection("col");
    std::string             db("db");
    CursorFirst<People>     cursor;

    GetMore<People>         getMore(collection, db, GetMoreConfig{}.setBatchSize(12), cursor);

    EXPECT_EQ(0x45, getMore.getSizeBson());

    std::stringstream   output;
    getMore.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x45\x00\x00\x00"
            "\x12" "getMore\x00"    "\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x02" "collection\x00" "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"        "\x03\x00\x00\x00"  "db\x00"
            "\x10" "batchSize\x00"  "\x0C\x00\x00\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongGetMoreTest, MessageGetMoreWithMaxTimeMS)
{
    std::string             collection("col");
    std::string             db("db");
    CursorFirst<People>     cursor;

    GetMore<People>         getMore(collection, db, GetMoreConfig{}.setMaxTimeMS(256), cursor);

    EXPECT_EQ(0x45, getMore.getSizeBson());

    std::stringstream   output;
    getMore.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x45\x00\x00\x00"
            "\x12" "getMore\x00"    "\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x02" "collection\x00" "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"        "\x03\x00\x00\x00"  "db\x00"
            "\x10" "maxTimeMS\x00"  "\x00\x01\x00\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongGetMoreTest, MessageGetMoreWithComment)
{
    std::string             collection("col");
    std::string             db("db");
    CursorFirst<People>     cursor;

    GetMore<People>         getMore(collection, db, GetMoreConfig{}.setComment("Long Shadows"), cursor);

    EXPECT_EQ(0x50, getMore.getSizeBson());

    std::stringstream   output;
    getMore.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x50\x00\x00\x00"
            "\x12" "getMore\x00"    "\x00\x00\x00\x00\x00\x00\x00\x00"
            "\x02" "collection\x00" "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"        "\x03\x00\x00\x00"  "db\x00"
            "\x02" "comment\x00"    "\x0D\x00\x00\x00"  "Long Shadows\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

