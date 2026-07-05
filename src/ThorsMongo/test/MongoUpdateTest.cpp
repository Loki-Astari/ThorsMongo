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

#if 0
TEST(MongoUpdateTest, SetCityAndMasterEnum)
{
    std::stringstream   s;

    using namespace std::literals;
    using ThorsAnvil::Serialize::PrinterConfig;
    using ThorsAnvil::Serialize::OutputType;
    using SetPersonAddressCity = ThorsMongo_ActionSpec(Set, People, address, city);
    using SetMaster            = ThorsMongo_ActionSpec(Set, People, master);

    using MySet = ThorsMongo_UpdateFromAccessSpec(Set, SetPersonAddressCity, SetMaster);

    s << ThorsAnvil::Serialize::bsonExporter(MySet{"Paris"s, Master::Active}, PrinterConfig{OutputType::Stream});

    EXPECT_EQ("\x3B\x00\x00\x00" // 4 + 1 + 5 + 48 + 1
                    "\x03"  "$set\0"    "\x30\x00\x00\x00" // => 4 + 24 + 19 + 1 => 48
                                            "\x02" "address.city\0"  "\x06\x00\x00\x00" "Paris\0"   // 1 + 13 + 4 + 6 => 24
                                            "\x02" "master\0"        "\x07\x00\x00\x00" "Active\0"  // 1 + 7 + 4 + 7 => 19
                                        "\x00"
              "\x00"s,

            s.str());

    EXPECT_EQ(59, ThorsAnvil::Serialize::bsonGetPrintSize(MySet{"Paris"s, Master::Active}));
}
#endif
