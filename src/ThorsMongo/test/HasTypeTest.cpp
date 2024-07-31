#include "gtest/gtest.h"

#include "MongoQuery.h"

using ThorsAnvil::DB::Mongo::HasCType;

struct NoType
{
    using V = int;
};
struct YesType
{
    using CType = int;
};

TEST(HasTypeTest, NoID)
{
    EXPECT_FALSE(HasCType<NoType>::val);
}

TEST(HasTypeTest, YesType)
{
    EXPECT_TRUE(HasCType<YesType>::val);
}
