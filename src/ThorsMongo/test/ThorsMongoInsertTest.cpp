#include "gtest/gtest.h"

#include "ThorsMongo.h"
#include "test/Action.h"

#include <vector>

using ThorsAnvil::DB::Mongo::InsertConfig;
using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::Action::Inserter;
using ThorsAnvil::DB::Mongo::WriteConcern;
using ThorsAnvil::DB::Mongo::InsertResult;
using ThorsAnvil::DB::Mongo::ObjectID;
using ThorsAnvil::DB::Mongo::W;
using ThorsAnvil::DB::Mongo::ParserConfig;
using ThorsAnvil::Serialize::bsonExporter;

TEST(ThorsMongInsertTest, InsertConfigDefault)
{
    InsertConfig    config;

    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongInsertTest, InsertConfigSetOrdered)
{
    InsertConfig    config = InsertConfig{}.setOrdered(true);

    EXPECT_TRUE(config.getOrdered().has_value());
    EXPECT_EQ(true, config.getOrdered().value());

    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongInsertTest, InsertConfigSetMaxTime)
{
    InsertConfig    config = InsertConfig{}.setMaxTimeMS(12);

    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_TRUE(config.getMaxTimeMS().has_value());
    EXPECT_EQ(12, config.getMaxTimeMS().value());

    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongInsertTest, InsertConfigSetWriteConcern)
{
    InsertConfig    config = InsertConfig{}.setWriteConcern({{}, true, {}});

    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_TRUE(config.getWriteConcern().has_value());
    WriteConcern expected{{}, true, {}};
    EXPECT_EQ(expected, config.getWriteConcern().value());

    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongInsertTest, InsertConfigSetByPass)
{
    InsertConfig    config = InsertConfig{}.setBypassDocumentValidation(true);

    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_TRUE(config.getBypassDocumentValidation().has_value());
    EXPECT_EQ(true, config.getBypassDocumentValidation().value());

    EXPECT_FALSE(config.getComment().has_value());
}

TEST(ThorsMongInsertTest, InsertConfigSetComment)
{
    InsertConfig    config = InsertConfig{}.setComment("Hi There");

    EXPECT_FALSE(config.getOrdered().has_value());
    EXPECT_FALSE(config.getMaxTimeMS().has_value());
    EXPECT_FALSE(config.getWriteConcern().has_value());
    EXPECT_FALSE(config.getBypassDocumentValidation().has_value());
    EXPECT_TRUE(config.getComment().has_value());
    EXPECT_EQ("Hi There", config.getComment().value());
}

TEST(ThorsMongInsertTest, InsertConfigSetAllValues)
{
    InsertConfig    config = InsertConfig{}.setOrdered(true)
                                           .setMaxTimeMS(12)
                                           .setWriteConcern({{}, true, {}})
                                           .setBypassDocumentValidation(true)
                                           .setComment("Hi There");

    EXPECT_TRUE(config.getOrdered().has_value());
    EXPECT_EQ(true, config.getOrdered().value());
    EXPECT_TRUE(config.getMaxTimeMS().has_value());
    EXPECT_EQ(12, config.getMaxTimeMS().value());
    EXPECT_TRUE(config.getWriteConcern().has_value());
    WriteConcern expected{{}, true, {}};
    EXPECT_EQ(expected, config.getWriteConcern().value());
    EXPECT_TRUE(config.getBypassDocumentValidation().has_value());
    EXPECT_EQ(true, config.getBypassDocumentValidation().value());
    EXPECT_TRUE(config.getComment().has_value());
    EXPECT_EQ("Hi There", config.getComment().value());
}

TEST(ThorsMongInsertTest, InsertTupleNoIdFunky)
{
    std::string             collection("col");
    std::string             db("db");
    auto                    documents{std::make_tuple(PersonNoId{"Polly", 89}, Funky{"car"}, Splotch{8, 5, 6})};
    InsertResult            result;

    Inserter<decltype(documents)>    inserter(collection, db, InsertConfig{}, documents, result);
    EXPECT_EQ(0xC5, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
        "\xC5\x00\x00\x00"                                                                                  //  4       4                                                                               //  4       4                                                                               //  4       4                                                                               //  4       4
            "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"                                           //  16      20
            "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"                                            //  12      32
            "\x04"  "documents\x00"                                                                         //  11      43
                "\x99\x00\x00\x00"                                                                          //  4       47      4
                    "\x03"  "0\x00"                                                                         //  3       50      7
                        "\x2F\x00\x00\x00"                                                                  //  4       54      11  4
                            "\x07" "_id\x00"    "\x00\x00\x00\x00"  "\x00\x00\x00\x00\x00"  "\x00\x00\x00"  //  17      71      28  21
                            "\x02" "name\x00"   "\x06\x00\x00\x00" "Polly\x00"                              //  16   X  87      44  37
                            "\x10" "age\x00"    "\x59\x00\x00\x00"                                          //  9       96      53  46
                            "\x00"                                                                          //  1       97      54  47
                    "\x03"  "1\x00"                                                                         //  3       100     57
                        "\x24\x00\x00\x00"                                                                  //  4       104     61  4
                            "\x07" "_id\x00"    "\x00\x00\x00\x00"  "\x00\x00\x00\x00\x00"  "\x00\x00\x00"  //  17      121     78  21
                            "\x02" "mark\x00"   "\x04\x00\x00\x00"  "car\x00"                               //  14      135     92  35
                            "\x00"                                                                          //  1       136     93  36
                    "\x03"  "2\x00"                                                                         //  3       139     96
                        "\x38\x00\x00\x00"                                                                  //  4       143     100 4
                            "\x07" "_id\x00"    "\x00\x00\x00\x00"  "\x00\x00\x00\x00\x00"  "\x00\x00\x00"  //  17      160     117 21
                            "\x10" "length\x00" "\x08\x00\x00\x00"                                          //  12      172     129 33
                            "\x10" "width\x00"  "\x05\x00\x00\x00"                                          //  11      183     140 44
                            "\x10" "color\x00"  "\x06\x00\x00\x00"                                          //  11      194     151 55
                            "\x00"                                                                          //  1       195     152 56
                    "\x00"                                                                                  //  1       196     153
            "\x00"s;                                                                                        //  1       197


    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);
    std::stringstream id2;
    id2 << bsonExporter(result.inserted[2]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
        expected[148 + loop] = id2.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertCreateNoId)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}, documents, result);
    // Size:
    //      Size:               4                                   => 4        4
    //          insert: col     1 + 7 + 4 + 4                       => 16       20
    //          $db: db         1 + 4 + 4 + 3                       => 12       32
    //          documents:      1 + 10                              => 11       43
    //              size            4                               => 4        47          4
    //                  0:              1 + 2                       => 3        50          7
    //                      {Polly}     4                           => 4        54          11      4
    //                          _id:        1 + 4 + 12              => 17       71          28      21      // Note this value is inserted by serializer.
    //                          name:       1 + 5 + 4 + 6           => 16       87          44      37
    //                          age:        1 + 4 + 4               => 9        96          54      46
    //                          term:       1                       => 1        97          55      47
    //                  1:              1 + 2                       => 3        100         58
    //                      {Pam}       4                           => 4        104         62      4
    //                          _id:        1 + 4 + 12              => 17       121         79      21      // Note this value is inserted by serializer.
    //                          name:       1 + 5 + 4 + 4           => 14       135         93      35
    //                          age:        1 + 4 + 4               => 9        144         102     44
    //                          term:       1                       => 1        145         103     45
    //                  term:   1                                   => 1        146         104
    //              term:       1                                   => 1        147
    EXPECT_EQ(147, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x93\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertCreateYesId)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonYesId> documents{{{}, "Polly", 89}, {{}, "Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonYesId>>    inserter(collection, db, InsertConfig{}, documents, result);
    // Size:
    //      Size:               4                                   => 4        4
    //          insert: col     1 + 7 + 4 + 4                       => 16       20
    //          $db: db         1 + 4 + 4 + 3                       => 12       32
    //          documents:      1 + 10                              => 11       43
    //              size            4                               => 4        47          4
    //                  0:              1 + 2                       => 3        50          7
    //                      {Polly}     4                           => 4        54          11      4
    //                          _id:        1 + 4 + 12              => 17       71          28      21
    //                          name:       1 + 5 + 4 + 6           => 16       87          44      37
    //                          age:        1 + 4 + 4               => 9        96          54      46
    //                          term:       1                       => 1        97          55      47
    //                  1:              1 + 2                       => 3        100         58
    //                      {Pam}       4                           => 4        104         62      4
    //                          _id:        1 + 4 + 12              => 17       121         79      21
    //                          name:       1 + 5 + 4 + 4           => 14       135         93      35
    //                          age:        1 + 4 + 4               => 9        144         102     44
    //                          term:       1                       => 1        145         103     45
    //                  term:   1                                   => 1        146         104
    //              term:       1                                   => 1        147
    EXPECT_EQ(147, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x93\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertWithOrderTrue)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}.setOrdered(true), documents, result);
    EXPECT_EQ(0x9D, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x9D\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "ordered\x00" "\x01"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertWithOrderFalse)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}.setOrdered(false), documents, result);
    EXPECT_EQ(0x9D, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x9D\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "ordered\x00" "\x00"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertWithMaxTime)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}.setMaxTimeMS(12), documents, result);
    EXPECT_EQ(0xA2, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xA2\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "maxTimeMS\x00" "\x0C\x00\x00\x00"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertWithWriteConcern)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}.setWriteConcern({W::Majority, true, 34}), documents, result);
    EXPECT_EQ(0xC8, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xC8\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "writeConcern\x00"                                                  // 14       14
                            "\x27\x00\x00\x00"                                                  // 4        18      4
                                "\x02"  "w\x00"         "\x09\x00\x00\x00" "majority\x00"       // 16       34      20
                                "\x08"  "j\x00"         "\x01"                                  // 4        38      24
                                "\x10"  "wtimeout\x00"  "\x22\x00\x00\x00"                      // 14       52      38
                                "\x00"                                                          // 1        53      39
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertWithBypassDocumentValidation)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}.setBypassDocumentValidation(true), documents, result);
    EXPECT_EQ(0xAE, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xAE\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "bypassDocumentValidation\x00" "\x01"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongInsertTest, InsertWithComment)
{
    std::string             collection("col");
    std::string             db("db");
    std::vector<PersonNoId> documents{{"Polly", 89}, {"Pam", 34}};
    InsertResult            result;

    Inserter<std::vector<PersonNoId>>    inserter(collection, db, InsertConfig{}.setComment("A value to check"), documents, result);
    EXPECT_EQ(0xB1, inserter.getSizeBson());

    std::stringstream   output;
    inserter.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\xB1\x00\x00\x00"
                    "\x02"  "insert\x00"    "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"       "\x03\x00\x00\x00"  "db\x00"
                    "\x04"  "documents\x00"
                        "\x67\x00\x00\x00"
                            "\x03"  "0\x00"
                                "\x2F\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x06\x00\x00\x00"  "Polly\x00"
                                    "\x10" "age\x00"    "\x59\x00\x00\x00"
                                    "\x00"
                            "\x03" "1\x00"
                                "\x2D\x00\x00\x00"
                                    "\x07" "_id\x00"    "\x00\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                                    "\x02" "name\x00"   "\x04\x00\x00\x00"  "Pam\x00"
                                    "\x10" "age\x00"    "\x22\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x02"  "comment\x00" "\x11\x00\x00\x00" "A value to check\x00"
                    "\x00"s;

    std::stringstream id0;
    id0 << bsonExporter(result.inserted[0]); // Note: Creates an Array with one value. So ObjectID starts at 7th character.
    std::stringstream id1;
    id1 << bsonExporter(result.inserted[1]);

    // Copy generated ID into the expected string.
    for (int loop = 0; loop < 12; ++loop) {
        expected[59 + loop] = id0.str()[7 + loop];
        expected[109 + loop] = id1.str()[7 + loop];
    }
    EXPECT_EQ(expected, output.str());
}


