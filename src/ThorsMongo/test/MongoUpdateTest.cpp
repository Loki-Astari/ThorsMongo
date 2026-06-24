#include "gtest/gtest.h"

#include "MongoQuery.h"
#include "MongoUpdate.h"
#include "test/Action.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include <sstream>

ThorsMongo_CreateFieldAccess(People, data);
ThorsMongo_CreateFieldAccess(People, age);
ThorsMongo_CreateFieldAccess(People, address, city);
ThorsMongo_CreateFieldAccess(Family, mother, address, code);

TEST(MongoUpdateTest, CreateFilterOneLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using PushToPersonData = ThorsMongo_UpdateFromAccess(Push, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(PushToPersonData{12}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$push":{"data":12}})", s.str());
}

TEST(MongoUpdateTest, CreateFilterTwoLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetPersonAddressCity = ThorsMongo_UpdateFromAccess(Set, People, address, city);

    s << ThorsAnvil::Serialize::jsonExporter(SetPersonAddressCity{"London"}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$set":{"address.city":"London"}})", s.str());
}

TEST(MongoUpdateTest, CreateFilterThreeLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetOnInsertFamilyPersonAddressCode = ThorsMongo_UpdateFromAccess(SetOnInsert, Family, mother, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(SetOnInsertFamilyPersonAddressCode{12}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$setOnInsert":{"mother.address.code":12}})", s.str());
}

TEST(MongoUpdateTest, SetCityAndCode)
{
    std::stringstream   s;

    using namespace std::literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetPersonAddressCity = ThorsMongo_ActionSpec(Set, People, address, city);
    using SetPersonData        = ThorsMongo_ActionSpec(Set, People, age);

    using MySet = ThorsMongo_UpdateFromAccessSpec(Set, SetPersonAddressCity, SetPersonData);

    s << ThorsAnvil::Serialize::jsonExporter(MySet{"Paris"s, 35}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ(R"({"$set":{"address.city":"Paris","age":35}})", s.str());
    EXPECT_EQ(42, ThorsAnvil::Serialize::jsonStreanSize(MySet{"Paris"s, 35}));
}
TEST(MongoUpdateTest, SetCityAndCodeBSON)
{
    std::stringstream   s;

    using namespace std::literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetPersonAddressCity = ThorsMongo_ActionSpec(Set, People, address, city);
    using SetPersonData        = ThorsMongo_ActionSpec(Set, People, age);

    using MySet = ThorsMongo_UpdateFromAccessSpec(Set, SetPersonAddressCity, SetPersonData);

    s << ThorsAnvil::Serialize::bsonExporter(MySet{"Paris"s, 35}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ("\x31\x00\x00\x00"
                    "\x03"  "$set\0"    "\x26\x00\x00\x00" // => 38
                                            "\x02" "address.city\0"  "\x06\x00\x00\x00" "Paris\0"   // 1 + 13 + 4 + 6 => 24  : 29 - 7
                                            "\x10" "age\0"           "\x23\x00\x00\x00"             // 1 +  4 + 4     => 9   : 14 - 7
                                        "\x00"
              "\x00"s,

            s.str());

    EXPECT_EQ(49, ThorsAnvil::Serialize::bsonGetPrintSize(MySet{"Paris"s, 35}));
}
