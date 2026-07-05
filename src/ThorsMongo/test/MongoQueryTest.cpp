#include "gtest/gtest.h"

#include "MongoQuery.h"
#include "test/Action.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include <sstream>

ThorsMongo_CreateFieldAccess(People, name);
using FindByPersonNameEq = ThorsMongo_FilterFromAccess(Eq, People, name);
ThorsMongo_CreateFieldAccess(People, address, city);
using FindByPersonAddressCCEq = ThorsMongo_FilterFromAccess(Eq, People, address, city);
ThorsMongo_CreateFieldAccess(Family, mother, address, code);
using FindByFamilyMotherAddress = ThorsMongo_FilterFromAccess(Eq, Family, mother, address, code);

TEST(MongoQueryTest, CreateFilterOneLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    s << ThorsAnvil::Serialize::jsonExporter(FindByPersonNameEq{"Tim"}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"name":{"$eq":"Tim"}})", s.str());
}

TEST(MongoQueryTest, CreateFilterTwoLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    s << ThorsAnvil::Serialize::jsonExporter(FindByPersonAddressCCEq{"London"}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"address.city":{"$eq":"London"}})", s.str());
}

TEST(MongoQueryTest, CreateFilterThreeLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    s << ThorsAnvil::Serialize::jsonExporter(FindByFamilyMotherAddress{12}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"mother.address.code":{"$eq":12}})", s.str());
}

TEST(MongoQueryTest, And2ValuesJson)
{
    using namespace std::string_literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using TwoAnd = And<FindByPersonNameEq, FindByFamilyMotherAddress>;

    std::stringstream   s;

    s << ThorsAnvil::Serialize::jsonExporter(TwoAnd{"Hi"s, 12}, PrinterConfig{OutputType::Stream});
    EXPECT_EQ(R"({"$and":[{"name":{"$eq":"Hi"}},{"mother.address.code":{"$eq":12}}]})", s.str());
}

TEST(MongoQueryTest, And2ValuesBson)
{
    using namespace std::string_literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using TwoAnd = And<FindByPersonNameEq, FindByFamilyMotherAddress>;

    std::stringstream   s;

    s << ThorsAnvil::Serialize::bsonExporter(TwoAnd{"Hi"s, 12}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(
        "\x5a\x00\x00\x00"
            "\x04"     "$and\0" "\x4f\x00\x00\x00"
                    "\x03" "0\0"    "\x1c\x00\x00\x00"          // 31
                        "\x03"  "name\0"  "\x11\x00\x00\x00"
                            "\x02"  "$eq\0"     "\x03\x00\x00\x00"  "Hi\0"
                            "\x00"
                        "\x00"
                    "\x03" "1\0"    "\x28\x00\x00\x00"          // 43
                        "\x03"  "mother.address.code\0" "\x0e\x00\x00\x00"
                            "\x10"  "$eq\0"     "\x0c\x00\x00\x00"
                            "\x00"
                        "\x00"
                    "\x00"
            "\x00"s
            , s.str());
#if 0
    {
        "$and":[
                {"name":{"$eq":"Hi"}},
                {"mother.address.code":{"$eq":12}}
            ]
    })"
#endif
}


TEST(MongoQueryTest, And3ValuesJson)
{
    using namespace std::string_literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using TwoAnd = And<FindByPersonNameEq, FindByFamilyMotherAddress,FindByPersonAddressCCEq>;

    std::stringstream   s;

    s << ThorsAnvil::Serialize::jsonExporter(TwoAnd{"Hi"s, 12, "London"s}, PrinterConfig{OutputType::Stream});
    EXPECT_EQ(R"({"$and":[{"name":{"$eq":"Hi"}},{"mother.address.code":{"$eq":12}},{"address.city":{"$eq":"London"}}]})", s.str());
}

TEST(MongoQueryTest, And3ValuesBson)
{
    using namespace std::string_literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using TwoAnd = And<FindByPersonNameEq, FindByFamilyMotherAddress,FindByPersonAddressCCEq>;

    std::stringstream   s;

    s << ThorsAnvil::Serialize::bsonExporter(TwoAnd{"Hi"s, 12, "London"s}, PrinterConfig{OutputType::Stream});
    EXPECT_EQ(
        "\x85\x00\x00\x00"
                "\x04" "$and\0" "\x7a\x00\x00\x00"
                    "\x03" "0\0"    "\x1c\x00\x00\x00"      // 31
                        "\x03" "name\0"   "\x11\x00\x00\x00"
                            "\x02"  "$eq\0"  "\x03\x00\x00\x00"  "Hi\0"
                            "\x00"
                        "\00"
                    "\x03" "1\0"    "\x28\x00\x00\x00"      // 43
                        "\x03" "mother.address.code\0"   "\x0e\x00\x00\x00"
                            "\x10"  "$eq\0"  "\x0c\x00\x00\x00"
                            "\x00"
                        "\00"
                    "\x03" "2\0"    "\x28\x00\x00\x00"      // 43
                        "\x03" "address.city\0"   "\x15\x00\x00\x00"
                            "\x02"  "$eq\0"  "\x07\x00\x00\x00"  "London\0"
                            "\x00"
                        "\00"
                    "\x00"
                "\x00"s
        , s.str());
#if 0
            {
                "$and":[
                    {"name":{"$eq":"Hi"}},
                    {"mother.address.code":{"$eq":12}},
                    {"address.city":{"$eq":"London"}}
                ]
            }
#endif
}

