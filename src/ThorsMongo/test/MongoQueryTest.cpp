#include "gtest/gtest.h"

#include "MongoQuery.h"
#include "test/Action.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include <sstream>

ThorsMongo_CreateFilter(FindByPersonNameEq, Eq, People, name);
ThorsMongo_CreateFilter(FindByPersonAddressCCEq, Eq, People, address, city);
ThorsMongo_CreateFilter(FindByFamilyMotherAddress, Eq, Family, mother, address, code);

TEST(MongoQueryTest, CreateFilterOneLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterInterface;
    s << ThorsAnvil::Serialize::jsonExporter(FindByPersonNameEq{"Tim"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(R"({"name":{"$eq":"Tim"}})", s.str());
}

TEST(MongoQueryTest, CreateFilterTwoLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterInterface;
    s << ThorsAnvil::Serialize::jsonExporter(FindByPersonAddressCCEq{"London"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(R"({"address.city":{"$eq":"London"}})", s.str());
}

TEST(MongoQueryTest, CreateFilterThreeLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterInterface;
    s << ThorsAnvil::Serialize::jsonExporter(FindByFamilyMotherAddress{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(R"({"mother.address.code":{"$eq":12}})", s.str());
}

