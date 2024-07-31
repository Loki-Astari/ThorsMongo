#include "gtest/gtest.h"

#include "ThorsMongo.h"

using ThorsAnvil::DB::Mongo::ObjectID;
using ThorsAnvil::DB::Mongo::Action::HasID;

struct NoID
{
    int         value;
};
struct YesID
{
    ObjectID    _id;
    int         value;
};

ThorsAnvil_MakeTrait(NoID, value);
ThorsAnvil_MakeTrait(YesID, _id, value);


TEST(HasIDTest, NoID)
{
    EXPECT_FALSE(HasID<NoID>::val);
}

TEST(HasIDTest, YesID)
{
    EXPECT_TRUE(HasID<YesID>::val);
}
