#include "gtest/gtest.h"
#include "MongoCursor.h"
#include "ThorsMongoFind.h"
#include "test/Action.h"

using namespace ThorsAnvil::DB::Mongo;

TEST(RangeTest, Construct)
{
    Range<FindResult<PersonNoId>>       range;
}

TEST(RangeTest, Copy)
{
    Range<FindResult<PersonNoId>>       range;
    Range<FindResult<PersonNoId>>       another{std::move(range)};
}

