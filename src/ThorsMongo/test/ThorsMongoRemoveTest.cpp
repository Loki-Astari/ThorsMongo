#include "gtest/gtest.h"

#include "ThorsMongo.h"
#include "MongoQuery.h"
#include "ThorSerialize/Traits.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::RemoveConfig;
using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::Action::Remover;
using ThorsAnvil::DB::Mongo::RemoveConfig;
using ThorsAnvil::DB::Mongo::Query;
using ThorsAnvil::DB::Mongo::Collation;
using ThorsAnvil::DB::Mongo::Remove;


TEST(ThorsMongRemoveTest, Construct)
{
    RemoveConfig        config;

    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
}

TEST(ThorsMongRemoveTest, ConstructAddComment)
{
    RemoveConfig        config = RemoveConfig{}.setComment("Comment on a line");

    EXPECT_TRUE(config.getComment().has_value());
    EXPECT_EQ("Comment on a line", config.getComment().value());

    EXPECT_FALSE(config.getLet().has_value());
    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
}

TEST(ThorsMongRemoveTest, ConstructAddLet)
{
    RemoveConfig        config = RemoveConfig{}.setLet({{"x", "2*y"}, {"z", "3 + 4"}});

    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_TRUE(config.getLet().has_value());
    EXPECT_EQ(2, config.getLet().value().size());

    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
}

TEST(ThorsMongRemoveTest, ConstructAddOrdered)
{
    RemoveConfig        config = RemoveConfig{}.setOrdered(true);

    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
    EXPECT_TRUE(config.getOrdered().has_value());
    EXPECT_EQ(true, config.getOrdered().value());

    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
}

TEST(ThorsMongRemoveTest, ConstructAddWriteConcern)
{
    RemoveConfig        config = RemoveConfig{}.setWriteConcern({{}, false, 567});

    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_TRUE(config.getWriteConcern().has_value());
    WriteConcern expected{{}, false, 567};
    EXPECT_EQ(expected, config.getWriteConcern().value());

    EXPECT_FALSE(config.getMaxTimeMS().has_value());
}

TEST(ThorsMongRemoveTest, ConstructAddMaxTimeMS)
{
    RemoveConfig        config = RemoveConfig{}.setMaxTimeMS(5567);

    EXPECT_FALSE(config.getComment().has_value());
    EXPECT_FALSE(config.getLet().has_value());
    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_TRUE(config.getMaxTimeMS().has_value());
    EXPECT_EQ(5567, config.getMaxTimeMS().value());
}

TEST(ThorsMongRemoveTest, ConstructAddAll)
{
    RemoveConfig        config = RemoveConfig{}
                                    .setComment("Comment on a line")
                                    .setLet({{"x", "2*y"}, {"z", "3 + 4"}})
                                    .setOrdered(true)
                                    .setWriteConcern({{}, false, 567})
                                    .setMaxTimeMS(5567);

    EXPECT_TRUE(config.getComment().has_value());
    EXPECT_EQ("Comment on a line", config.getComment().value());
    EXPECT_TRUE(config.getLet().has_value());
    EXPECT_EQ(2, config.getLet().value().size());
    EXPECT_TRUE(config.getOrdered().has_value());
    EXPECT_EQ(true, config.getOrdered().value());
    EXPECT_TRUE(config.getWriteConcern().has_value());
    WriteConcern expected{{}, false, 567};
    EXPECT_EQ(expected, config.getWriteConcern().value());
    EXPECT_TRUE(config.getMaxTimeMS().has_value());
    EXPECT_EQ(5567, config.getMaxTimeMS().value());
}

TEST(ThorsMongRemoveTest, RemoveTuple)
{
    // This test Assumes
    std::string                         collection("col");
    std::string                         db("db");
    auto                                documents{std::make_tuple(Query<NameField<std::string>>{"Sam", Remove::One}, Query<AgeField<int>>{45}, Query<Funky>{"twin"})};

    Remover<decltype(documents)>        remover(collection, db, RemoveConfig{}, documents);
    EXPECT_EQ(0xA6, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xA6\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x7C\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x03"  "1\x00"                                                     //  3   89      48
                                "\x21\x00\x00\x00"                                              //  4   93      52      4
                                    "\x03"  "q\x00"                                             //  3   96      55      7
                                        "\x0E\x00\x00\x00"                                      //  4   100     59      11      4
                                            "\x10"  "age\x00"   "\x2D\x00\x00\x00"              //  9   109     68      20      13
                                            "\x00"                                              //  1   110     69      21      14
                                    "\x10"  "limit\x00" "\x00\x00\x00\x00"                      //  11  121     80      32
                                    "\x00"                                                      //  1   122     81      33
                            "\x03"  "2\x00"                                                     //  3   125     84
                                "\x27\x00\x00\x00"                                              //  4   129     88      4
                                    "\x03"  "q\x00"                                             //  3   132     91      7
                                        "\x14\x00\x00\x00"                                      //  4   136     95      11      4
                                            "\x02"  "mark\x00"  "\x05\x00\x00\x00" "twin\x00"   //  15  151     110     26      19
                                            "\x00"                                              //  1   152     111     27      20
                                    "\x10"  "limit\x00" "\x00\x00\x00\x00"                      //  11  163     122     38
                                    "\x00"                                                      //  1   164     123     39
                            "\x00"                                                              //  1   165     124
                    "\x00"s;                                                                    //  1   166

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveBasic)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}, documents);
    EXPECT_EQ(0x58, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x58\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x00"s;                                                                    //  1   88

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveWithComment)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}.setComment("A famous delete"), documents);
    EXPECT_EQ(0x75, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x75\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x02" "comment\x00" "\x10\x00\x00\x00" "A famous delete\x00"               //  29  116
                    "\x00"s;                                                                    //  1   117

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveWithLet)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}.setLet({{"x", "12"}}), documents);
    EXPECT_EQ(0x6C, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x6C\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x03" "let\x00"                                                            //  5   92
                        "\x0F\x00\x00\x00"                                                      //  4   96      4
                            "\x02"  "x\00"  "\x03\x00\x00\x00" "12\x00"                         //  10  106     14
                            "\x00"                                                              //  1   107     15
                    "\x00"s;                                                                    //  1   108

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveWithOrdered)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}.setOrdered(false), documents);
    EXPECT_EQ(0x62, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x62\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x08"  "ordered\x00" "\x00"                                                //  10  97
                    "\x00"s;                                                                    //  1   98

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveWithWriteConcern)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}.setWriteConcern({13, true, 12}), documents);
    EXPECT_EQ(0x84, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x84\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x03"  "writeConcern\x00"                                                  //  14  101
                        "\x1E\x00\x00\x00"                                                      //  4   105     4
                            "\x10"  "w\x00"         "\x0D\x00\x00\x00"                          //  7   112     11
                            "\x08"  "j\x00"         "\x01"                                      //  4   116     15
                            "\x10"  "wtimeout\x00"  "\x0C\x00\x00\x00"                          //  14  130     29
                            "\x00"                                                              //  1   131     30
                    "\x00"s;                                                                    //  1   132

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveWithMaxTimeMS)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}.setMaxTimeMS(18), documents);
    EXPECT_EQ(0x67, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x67\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x10" "maxTimeMS\x00"  "\x12\x00\x00\x00"                                  //  15  102
                    "\x00"s;                                                                    //  1   103

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveQueryDefaultLimit)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam"}}; // Don't specify limit => Should be 0

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}, documents);
    EXPECT_EQ(0x58, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x58\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x2E\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x26\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x00\x00\x00\x00"                      //  11  85      44      37
                                    "\x00"                                                      //  1   86      45      38
                            "\x00"                                                              //  1   87      46
                    "\x00"s;                                                                    //  1   88

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveQueryHint)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One, {}, "Hint"}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}, documents);
    EXPECT_EQ(0x67, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x67\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x3D\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x35\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x02"  "hint\x00"  "\x05\x00\x00\x00" "Hint\x00"           //  15  100     59      52
                                    "\x00"                                                      //  1   101     60      53
                            "\x00"                                                              //  1   102     61
                    "\x00"s;                                                                    //  1   103

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongRemoveTest, RemoveDeleteWithCollation)
{
    std::string                         collection("col");
    std::string                         db("db");
    std::vector<Query<NameField<std::string>>>      documents{{"Sam", Remove::One, Collation{"Bob", true, {}, 32, {}, {}, {}, {}}}};

    Remover<std::vector<Query<NameField<std::string>>>>    remover(collection, db, RemoveConfig{}, documents);
    EXPECT_EQ(0x92, remover.getSizeBson());

    std::stringstream   output;
    remover.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x92\x00\x00\x00"                                                              //  4   4
                    "\x02" "delete\x00" "\x04\x00\x00\x00" "col\x00"                            //  16  20
                    "\x02" "$db\x00"    "\x03\x00\x00\x00" "db\x00"                             //  12  32
                    "\x04" "deletes\x00"                                                        //  9   41
                        "\x68\x00\x00\x00"                                                      //  4   45      4
                            "\x03"  "0\x00"                                                     //  3   48      7
                                "\x60\x00\x00\x00"                                              //  4   52      11      4
                                    "\x03"  "q\x00"                                             //  3   55      14      7
                                        "\x13\x00\x00\x00"                                      //  4   59      18      11      4
                                            "\x02"  "name\x00"  "\x04\x00\x00\x00"  "Sam\x00"   //  14  73      32      25      18
                                            "\x00"                                              //  1   74      33      26      19
                                    "\x10"  "limit\x00" "\x01\x00\x00\x00"                      //  11  85      44      37
                                    "\x03"  "collation\x00"                                     //  11  96      55      48
                                        "\x2F\x00\x00\x00"                                      //  4   100     59      52      4
                                            "\x02" "locale\x00"    "\x04\x00\x00\x00" "Bob\x00" //  16  116     75      68      20
                                            "\x08" "caseLevel\x00" "\x01"                       //  12  128     87      80      32
                                            "\x10" "strength\x00"  "\x20\x00\x00\x00"           //  14  142     101     94      46
                                            "\x00"                                              //  1   143     102     95      47
                                    "\x00"                                                      //  1   144     103     96
                            "\x00"                                                              //  1   145     104
                    "\x00"s;                                                                    //  1   146

    EXPECT_EQ(expected, output.str());
}


