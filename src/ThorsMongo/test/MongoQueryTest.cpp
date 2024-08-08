#include "gtest/gtest.h"

#include "MongoQuery.h"
#include "test/Action.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include <sstream>

ThorsMongo_CreateFilter(FindByPersonNameEq, Eq, People, name);

TEST(MongoQueryTest, CreateFilterOneLevel)
{
    std::stringstream   s;

    using ThorsAnvil::Serialize::PrinterInterface;
    s << ThorsAnvil::Serialize::jsonExporter(FindByPersonNameEq{"Tim"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});

    EXPECT_EQ(R"({"name":{"$eq":"Tim"}})", s.str());

}
