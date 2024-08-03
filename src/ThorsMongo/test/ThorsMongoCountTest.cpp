#include "gtest/gtest.h"
#include "test/Action.h"

#include "ThorsMongo.h"
#include "MongoQuery.h"

using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::CountConfig;
using ThorsAnvil::DB::Mongo::Action::Counter;
using ThorsAnvil::DB::Mongo::Collation;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::Level;
using namespace ThorsAnvil::DB::Mongo::QueryOp;


TEST(ThorsMongoCountTest, ConstructCountConfig)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    EXPECT_FALSE(countConfig.getSkip().has_value());
    EXPECT_FALSE(countConfig.getHint().has_value());
    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(countConfig.getCollation().has_value());
    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetLimit)
{
    CountConfig      countConfig;

    countConfig.setLimit(10);
    EXPECT_TRUE(countConfig.getLimit().has_value());

    EXPECT_FALSE(countConfig.getSkip().has_value());
    EXPECT_FALSE(countConfig.getHint().has_value());
    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(countConfig.getCollation().has_value());
    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetSkip)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    countConfig.setSkip(10);
    EXPECT_TRUE(countConfig.getSkip().has_value());

    EXPECT_FALSE(countConfig.getHint().has_value());
    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(countConfig.getCollation().has_value());
    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetHint)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    EXPECT_FALSE(countConfig.getSkip().has_value());
    countConfig.setHint("Hi");
    EXPECT_TRUE(countConfig.getHint().has_value());

    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(countConfig.getCollation().has_value());
    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetReadConcern)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    EXPECT_FALSE(countConfig.getSkip().has_value());
    EXPECT_FALSE(countConfig.getHint().has_value());
    countConfig.setReadConcern({Level::Local});
    EXPECT_TRUE(countConfig.getReadConcern().has_value());

    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(countConfig.getCollation().has_value());
    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetMaxTimeMS)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    EXPECT_FALSE(countConfig.getSkip().has_value());
    EXPECT_FALSE(countConfig.getHint().has_value());
    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    countConfig.setMaxTimeMS(255);
    EXPECT_TRUE(countConfig.getMaxTimeMS().has_value());

    EXPECT_FALSE(countConfig.getCollation().has_value());
    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetCollation)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    EXPECT_FALSE(countConfig.getSkip().has_value());
    EXPECT_FALSE(countConfig.getHint().has_value());
    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    countConfig.setCollation(Collation{"gb", {}, {}, {}, {}, {}, {}, {}});
    EXPECT_TRUE(countConfig.getCollation().has_value());

    EXPECT_FALSE(countConfig.getComment().has_value());
}

TEST(ThorsMongoCountTest, ConstructCountConfigSetComment)
{
    CountConfig      countConfig;

    EXPECT_FALSE(countConfig.getLimit().has_value());
    EXPECT_FALSE(countConfig.getSkip().has_value());
    EXPECT_FALSE(countConfig.getHint().has_value());
    EXPECT_FALSE(countConfig.getReadConcern().has_value());
    EXPECT_FALSE(countConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(countConfig.getCollation().has_value());
    countConfig.setComment("Comment again");
    EXPECT_TRUE(countConfig.getComment().has_value());
}

TEST(ThorsMongCountTest, SerializeCount)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}, find};

    EXPECT_EQ(0x45, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x45\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
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

TEST(ThorsMongCountTest, SerializeCountWithLimit)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setLimit(10), find};

    EXPECT_EQ(0x50, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x50\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "limit\x00" "\x0A\x00\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongCountTest, SerializeCountWithSkip)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setSkip(255), find};

    EXPECT_EQ(0x4F, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x4F\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "skip\x00"      "\xFF\x00\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongCountTest, SerializeCountWithHint)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setHint("A hint"), find};

    EXPECT_EQ(0x56, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x56\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x02"  "hint\x00"  "\x07\x00\x00\x00"  "A hint\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongCountTest, SerializeCountReadConcern)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setReadConcern({}), find};

    EXPECT_EQ(0x68, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x68\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
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

TEST(ThorsMongCountTest, SerializeCountMaxTimeMS)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setMaxTimeMS(254), find};

    EXPECT_EQ(0x54, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x54\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "query\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00" "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "maxTimeMS\x00" "\xFE\x00\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongCountTest, SerializeCountCollation)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setCollation({"gb", true, "Up", 10, false, "Min", "Loop", false}), find};

    EXPECT_EQ(0xD7, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xD7\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
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

TEST(ThorsMongCountTest, SerializeCountWithComment)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Counter<Search>         counter{collection, db, CountConfig{}.setComment("Plop"), find};

    EXPECT_EQ(0x57, counter.getSizeBson());

    std::stringstream   output;
    counter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x57\x00\x00\x00"
                    "\x02"  "count\x00"     "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
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

