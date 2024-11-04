#include "gtest/gtest.h"

#include "ThorsMongo.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::FAModifyConfig;
using ThorsAnvil::DB::Mongo::FARemoveConfig;
using ThorsAnvil::DB::Mongo::Action::FindAndModify;
using ThorsAnvil::DB::Mongo::SortOrder;
using ThorsAnvil::DB::Mongo::Projection;
using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::WriteConcern;
using namespace ThorsAnvil::DB::Mongo::QueryOp;


TEST(ThorsMongoFindAndModifyTest, UpdateEach)
{
    using Search = NameField<Eq<std::string>>;
    using Update = AddToSet<FoodField<Each<std::vector<std::string>>>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{{"Jam", "Butter"}};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0x9A, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x9A\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x45\x00\x00\x00"
                        "\x03" "$addToSet\x00"
                            "\x35\x00\x00\x00"
                                "\x03" "food\x00"
                                    "\x2A\x00\x00\x00"
                                        "\x04" "$each\x00"
                                            "\x1E\x00\x00\x00"
                                                "\x02" "0\x00" "\x04\x00\x00\x00" "Jam\x00"
                                                "\x02" "1\x00" "\x07\x00\x00\x00" "Butter\x00"
                                                "\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateSlice)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Push<FoodField<Slice<std::vector<std::string>>>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{{{"long", "small"},3}};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0xA2, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xA2\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x03" "$push\x00"
                            "\x41\x00\x00\x00"
                                "\x03" "food\x00"
                                    "\x36\x00\x00\x00"
                                        "\x04" "$each\x00"
                                            "\x1E\x00\x00\x00"
                                                "\x02" "0\x00" "\x05\x00\x00\x00" "long\x00"
                                                "\x02" "1\x00" "\x06\x00\x00\x00" "small\x00"
                                                "\x00"
                                        "\x10" "$slice\x00" "\x03\x00\x00\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdatePosition)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Push<FoodField<Position<std::vector<std::string>>>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{{{"long", "small"},3}};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0xA5, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xA5\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x50\x00\x00\x00"
                        "\x03" "$push\x00"
                            "\x44\x00\x00\x00"
                                "\x03" "food\x00"
                                    "\x39\x00\x00\x00"
                                        "\x04" "$each\x00"
                                            "\x1E\x00\x00\x00"
                                                "\x02" "0\x00" "\x05\x00\x00\x00" "long\x00"
                                                "\x02" "1\x00" "\x06\x00\x00\x00" "small\x00"
                                                "\x00"
                                        "\x10" "$position\x00" "\x03\x00\x00\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, UpdateSort)
{
    using Search = NameField<Eq<std::string>>;
    using Update = Push<FoodField<Sort<std::vector<std::string>>>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    Update                          update{{{"long", "small"}, SortOrder::Ascending}};
    FindAndModify<Search, Update>   findAndModify(collection, db, FAModifyConfig{}, find, update);

    EXPECT_EQ(0xA1, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xA1\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4C\x00\x00\x00"
                        "\x03" "$push\x00"
                            "\x40\x00\x00\x00"
                                "\x03" "food\x00"
                                    "\x35\x00\x00\x00"
                                        "\x04" "$each\x00"
                                            "\x1E\x00\x00\x00"
                                                "\x02" "0\x00" "\x05\x00\x00\x00" "long\x00"
                                                "\x02" "1\x00" "\x06\x00\x00\x00" "small\x00"
                                                "\x00"
                                        "\x10" "$sort\x00" "\x01\x00\x00\x00"
                                        "\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, Replace)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FindAndModify<Search, People>   findAndModify(collection, db, FAModifyConfig{}, find, mark);

    EXPECT_EQ(0xA2, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xA2\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());

}

TEST(ThorsMongoFindAndModifyTest, ReplaceSetSortAscending)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setSort({{"name", SortOrder::Ascending}});
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xB7, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xB7\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x03" "sort\x00"
                    "\x0F\x00\x00\x00"
                        "\x10"  "name\x00"  "\x01\x00\x00\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());

}

TEST(ThorsMongoFindAndModifyTest, ReplaceSetSortDescending)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setSort({{"name", SortOrder::Descending}});
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xB7, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xB7\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x03" "sort\x00"
                    "\x0F\x00\x00\x00"
                        "\x10"  "name\x00"  "\xFF\xFF\xFF\xFF"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceSetNew)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setReturnNew(true);
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xA8, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xA8\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x08" "new\x00" "\x01"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceSetFields)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setFields(Projection::create<PeopleWithAddressCode>());
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xD1, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xD1\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x03" "fields\x00"
                    "\x27\x00\x00\x00"
                        "\x10"  "name\x00"  "\x01\x00\x00\x00"
                        "\x03"  "address\x00"
                            "\x0F\x00\x00\x00"
                                "\x10" "code\x00" "\x01\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceUpsert)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setUpsert(true);
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xAB, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xAB\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x08" "upsert\x00" "\x01"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceByPassDocumenValidation)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setBypassDocumentValidation(true);
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xBD, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xBD\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x08" "bypassDocumentValidation\x00" "\x01"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceWriteConcern)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setWriteConcern({});
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xB9, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xB9\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x03" "writeConcern\x00"
                    "\x09\x00\x00\x00"
                        "\x08" "j\x00"  "\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceMaxTimeMS)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setMaxTimeMS(255);
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xB1, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xB1\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x10" "maxTimeMS\x00" "\xFF\x00\x00\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceCollation)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setCollation({"US", {}, {}, 48, {}, {}, {}, {}});
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xCF, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xCF\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x03" "collation\x00"
                    "\x22\x00\x00\x00"
                        "\x02"  "locale\x00"    "\x03\x00\x00\x00"  "US\x00"
                        "\x10"  "strength\x00"  "\x30\x00\x00\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceHint)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setHint("North is Good");
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xBA, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xBA\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x02" "hint\x00" "\x0E\x00\x00\x00" "North is Good\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceComment)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setComment("Commong Code");
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xBC, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xBC\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x02" "comment\x00" "\x0D\x00\x00\x00" "Commong Code\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, ReplaceLet)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    People                          mark{"Mark", 24, {}, {}};
    FAModifyConfig                  config;

    config.setLet({{"x", "1"}, {"y", "32"}});
    FindAndModify<Search, People>   findAndModify(collection, db, config, find, mark);

    EXPECT_EQ(0xBF, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xBF\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x03" "update\x00"
                    "\x4D\x00\x00\x00"
                        "\x02" "name\x00"   "\x05\x00\x00\x00" "Mark\x00"
                        "\x10" "age\x00"    "\x18\x00\x00\x00"
                        "\x03" "address\x00"
                            "\x27\x00\x00\x00"
                                "\x02" "street\x00"     "\x01\x00\x00\x00"  "\x00"
                                "\x02" "city\x00"       "\x01\x00\x00\x00"  "\x00"
                                "\x10" "code\x00"       "\x00\x00\x00\x00"
                                "\x00"
                        "\x00"
            "\x03" "let\x00"
                    "\x18\x00\x00\x00"
                        "\x02" "x\x00" "\x02\x00\x00\x00" "1\x00"
                        "\x02" "y\x00" "\x03\x00\x00\x00" "32\x00"
                        "\x00"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongoFindAndModifyTest, Remove)
{
    using Search = NameField<Eq<std::string>>;

    std::string                     collection("col");
    std::string                     db("db");
    Search                          find{"John"};
    FindAndModify<Search, bool>     findAndModify(collection, db, FARemoveConfig{}, find);

    EXPECT_EQ(0x56, findAndModify.getSizeBson());

    std::stringstream   output;
    findAndModify.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\x56\x00\x00\x00"
            "\x02" "findAndModify\x00"  "\x04\x00\x00\x00"  "col\x00"
            "\x02" "$db\x00"            "\x03\x00\x00\x00"  "db\x00"
            "\x03" "query\x00"
                    "\x1E\x00\x00\x00"
                        "\x03" "name\x00"
                            "\x13\x00\x00\x00"
                                "\x02" "$eq\x00" "\x05\x00\x00\x00" "John\x00"
                                "\x00"
                        "\x00"
            "\x08" "remove\x00" "\x01"
            "\x00"s;

    EXPECT_EQ(expected, output.str());
}


