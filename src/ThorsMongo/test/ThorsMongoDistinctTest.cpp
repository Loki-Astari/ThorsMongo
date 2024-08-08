#include "gtest/gtest.h"
#include "test/Action.h"

#include "ThorsMongo.h"
#include "MongoQuery.h"

using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::DistinctConfig;
using ThorsAnvil::DB::Mongo::Action::Distinct;
using ThorsAnvil::DB::Mongo::Collation;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::Level;
using namespace ThorsAnvil::DB::Mongo::QueryOp;


TEST(ThorsMongoDistinctTest, ConstructDistinctConfig)
{
    DistinctConfig      distinctConfig;

    EXPECT_FALSE(distinctConfig.getReadConcern().has_value());
    EXPECT_FALSE(distinctConfig.getCollation().has_value());
    EXPECT_FALSE(distinctConfig.getComment().has_value());
}

TEST(ThorsMongoDistinctTest, ConstructDistinctConfigSetReadConcern)
{
    DistinctConfig      distinctConfig;

    distinctConfig.setReadConcern({Level::Local});
    EXPECT_TRUE(distinctConfig.getReadConcern().has_value());

    EXPECT_FALSE(distinctConfig.getCollation().has_value());
    EXPECT_FALSE(distinctConfig.getComment().has_value());
}

TEST(ThorsMongoDistinctTest, ConstructDistinctConfigSetCollation)
{
    DistinctConfig      distinctConfig;

    EXPECT_FALSE(distinctConfig.getReadConcern().has_value());
    distinctConfig.setCollation(Collation{"gb", {}, {}, {}, {}, {}, {}, {}});
    EXPECT_TRUE(distinctConfig.getCollation().has_value());

    EXPECT_FALSE(distinctConfig.getComment().has_value());
}

TEST(ThorsMongoDistinctTest, ConstructDistinctConfigSetComment)
{
    DistinctConfig      distinctConfig;

    EXPECT_FALSE(distinctConfig.getReadConcern().has_value());
    EXPECT_FALSE(distinctConfig.getCollation().has_value());
    distinctConfig.setComment("Comment again");
    EXPECT_TRUE(distinctConfig.getComment().has_value());
}

TEST(ThorsMongoDistinctTest, SerializeCount)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Distinct<Search>        distinct{collection, db, DistinctConfig{}, "name", find};

    EXPECT_EQ(0x56, distinct.getSizeBson());

    std::stringstream   output;
    distinct.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x56\x00\x00\x00"
                    "\x02"  "distinct\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x02"  "key\x00"       "\x05\x00\x00\x00"  "name\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoDistinctTest, SerializeCountReadConcern)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Distinct<Search>        distinct{collection, db, DistinctConfig{}.setReadConcern({}), "name", find};

    EXPECT_EQ(0x79, distinct.getSizeBson());

    std::stringstream   output;
    distinct.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x79\x00\x00\x00"
                    "\x02"  "distinct\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x02"  "key\x00"       "\x05\x00\x00\x00"  "name\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "readConcern\x00"
                        "\x16\x00\x00\x00"
                            "\x02"  "level\x00"     "\x06\x00\x00\x00"  "local\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoDistinctTest, SerializeCountCollation)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Distinct<Search>        distinct{collection, db, DistinctConfig{}.setCollation({"gb", true, "Up", 10, false, "Min", "Loop", false}), "name", find};

    EXPECT_EQ(0xE8, distinct.getSizeBson());

    std::stringstream   output;
    distinct.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xE8\x00\x00\x00"
                    "\x02"  "distinct\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x02"  "key\x00"       "\x05\x00\x00\x00"  "name\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "collation\x00"
                        "\x87\x00\x00\x00"
                            "\x02"  "locale\x00"            "\x03\x00\x00\x00" "gb\x00"
                            "\x08"  "caseLevel\x00"         "\x01"
                            "\x02"  "caseFirst\x00"         "\x03\x00\x00\x00" "Up\x00"
                            "\x10"  "strength\x00"          "\x0A\x00\x00\x00"
                            "\x08"  "numericOrdering\x00"   "\x00"
                            "\x02"  "alternate\x00"         "\x04\x00\x00\x00" "Min\x00"
                            "\x02"  "maxVariable\x00"       "\x05\x00\x00\x00" "Loop\x00"
                            "\x08"  "backwards\x00"         "\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoDistinctTest, SerializeCountWithComment)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Distinct<Search>        distinct{collection, db, DistinctConfig{}.setComment("Plop"), "name", find};

    EXPECT_EQ(0x68, distinct.getSizeBson());

    std::stringstream   output;
    distinct.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x68\x00\x00\x00"
                    "\x02"  "distinct\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x02"  "key\x00"       "\x05\x00\x00\x00"  "name\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x02"  "comment\x00"   "\x05\x00\x00\x00"  "Plop\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

