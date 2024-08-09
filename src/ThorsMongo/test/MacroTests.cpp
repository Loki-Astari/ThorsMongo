#include <gtest/gtest.h>

#include "test/Action.h"
#include "MongoQuery.h"
#include "MongoUpdate.h"


using ThorsAnvil::Serialize::PrinterInterface;


ThorsMongo_CreateFieldAccess(Funky, mark);
ThorsMongo_CreateFieldAccess(Splotch, length);
ThorsMongo_CreateFieldAccess(Splotch, width);
ThorsMongo_CreateFieldAccess(Splotch, color);

ThorsMongo_CreateFieldAccess(Address, street);
ThorsMongo_CreateFieldAccess(Address, city);
ThorsMongo_CreateFieldAccess(Address, code);

ThorsMongo_CreateFieldAccess(People, name);
ThorsMongo_CreateFieldAccess(People, age);
ThorsMongo_CreateFieldAccess(People, address);
ThorsMongo_CreateFieldAccess(People, data);

ThorsMongo_CreateFieldAccess(People, address, street);
ThorsMongo_CreateFieldAccess(People, address, city);
ThorsMongo_CreateFieldAccess(People, address, code);

ThorsMongo_CreateFieldAccess(Family, mother);
ThorsMongo_CreateFieldAccess(Family, mother, name);
ThorsMongo_CreateFieldAccess(Family, mother, age);
ThorsMongo_CreateFieldAccess(Family, mother, address);
ThorsMongo_CreateFieldAccess(Family, mother, data);
ThorsMongo_CreateFieldAccess(Family, mother, address, street);
ThorsMongo_CreateFieldAccess(Family, mother, address, city);
ThorsMongo_CreateFieldAccess(Family, mother, address, code);
ThorsMongo_CreateFieldAccess(Family, father);
ThorsMongo_CreateFieldAccess(Family, father, name);
ThorsMongo_CreateFieldAccess(Family, father, age);
ThorsMongo_CreateFieldAccess(Family, father, address);
ThorsMongo_CreateFieldAccess(Family, father, data);
ThorsMongo_CreateFieldAccess(Family, father, address, street);
ThorsMongo_CreateFieldAccess(Family, father, address, city);
ThorsMongo_CreateFieldAccess(Family, father, address, code);

ThorsMongo_CreateFieldAccess(PeopleWithTime, dateTime);
ThorsMongo_CreateFieldAccess(PeopleWithTime, timeStamp);

ThorsMongo_CreateFieldAccess(PeopleWithRecordID, $recordId);

ThorsMongo_CreateFieldAccess(PeopleWithFood, food);

ThorsMongo_CreateFieldAccess(AddressCode, code);

ThorsMongo_CreateFieldAccess(PeopleWithAddressCode, name);
ThorsMongo_CreateFieldAccess(PeopleWithAddressCode, address);
ThorsMongo_CreateFieldAccess(PeopleWithAddressCode, address, code);

TEST(MacroTests, EqString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Eq, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"small"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$eq":"small"}})", s.str());
}
TEST(MacroTests, EqInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Eq, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$eq":12}})", s.str());
}
TEST(MacroTests, EqAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Eq, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Lane", "London", 123}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$eq":{"street":"Lane","city":"London","code":123}}})", s.str());
}
TEST(MacroTests, NeString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Ne, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"small"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$ne":"small"}})", s.str());
}
TEST(MacroTests, NeInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Ne, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$ne":12}})", s.str());
}
TEST(MacroTests, NeAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Ne, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Lane", "London", 123}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$ne":{"street":"Lane","city":"London","code":123}}})", s.str());
}
TEST(MacroTests, LtString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Lt, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"small"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$lt":"small"}})", s.str());
}
TEST(MacroTests, LtInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Lt, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$lt":12}})", s.str());
}
TEST(MacroTests, LtAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Lt, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Lane", "London", 123}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$lt":{"street":"Lane","city":"London","code":123}}})", s.str());
}
TEST(MacroTests, LteString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Lte, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"small"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$lte":"small"}})", s.str());
}
TEST(MacroTests, LteInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Lte, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$lte":12}})", s.str());
}
TEST(MacroTests, LteAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Lte, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Lane", "London", 123}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$lte":{"street":"Lane","city":"London","code":123}}})", s.str());
}
TEST(MacroTests, GtString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Gt, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"small"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$gt":"small"}})", s.str());
}
TEST(MacroTests, GtInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Gt, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$gt":12}})", s.str());
}
TEST(MacroTests, GtAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Gt, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Lane", "London", 123}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$gt":{"street":"Lane","city":"London","code":123}}})", s.str());
}
TEST(MacroTests, GteString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Gte, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"small"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$gte":"small"}})", s.str());
}
TEST(MacroTests, GteInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Gte, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$gte":12}})", s.str());
}
TEST(MacroTests, GteAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Gte, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Lane", "London", 123}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$gte":{"street":"Lane","city":"London","code":123}}})", s.str());
}
TEST(MacroTests, InString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, In, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"First","second","third"}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$in":["First","second","third"]}})", s.str());
}
TEST(MacroTests, InInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, In, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{1,2,3}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$in":[1,2,3]}})", s.str());
}
TEST(MacroTests, InAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, In, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{{"Lane", "London", 123},{},{}}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$in":[{"street":"Lane","city":"London","code":123},{"street":"","city":"","code":0},{"street":"","city":"","code":0}]}})", s.str());
}
TEST(MacroTests, NinString)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Nin, Family, father, name);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"First","second","third"}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.name":{"$nin":["First","second","third"]}})", s.str());
}
TEST(MacroTests, NinInt)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Nin, Family, father, age);      // std::string

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{1,2,3}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"father.age":{"$nin":[1,2,3]}})", s.str());
}
TEST(MacroTests, NinAddress)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Nin, Family, mother, address);      // structure

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{{"Lane", "London", 123},{},{}}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address":{"$nin":[{"street":"Lane","city":"London","code":123},{"street":"","city":"","code":0},{"street":"","city":"","code":0}]}})", s.str());
}
TEST(MacroTests, BinaryAnd)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(FilterLHS, Lt, Family, mother, name);
    ThorsMongo_FilterFromAccess(FilterRHS, Gt, Family, mother, address, code);
    using Filter = ThorsAnvil::DB::Mongo::QueryOp::And<FilterLHS, FilterRHS>;

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"Tim",386}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$and":[{"mother.name":{"$lt":"Tim"}},{"mother.address.code":{"$gt":386}}]})", s.str());
}
TEST(MacroTests, BinaryOr)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(FilterLHS, Lt, Family, mother, name);
    ThorsMongo_FilterFromAccess(FilterRHS, Gt, Family, mother, address, code);
    using Filter = ThorsAnvil::DB::Mongo::QueryOp::Or<FilterLHS, FilterRHS>;

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"Tim",386}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$or":[{"mother.name":{"$lt":"Tim"}},{"mother.address.code":{"$gt":386}}]})", s.str());
}
TEST(MacroTests, BinaryNor)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(FilterLHS, Lt, Family, mother, name);
    ThorsMongo_FilterFromAccess(FilterRHS, Gt, Family, mother, address, code);
    using Filter = ThorsAnvil::DB::Mongo::QueryOp::Nor<FilterLHS, FilterRHS>;

    s << ThorsAnvil::Serialize::jsonExporter(Filter{"Tim",386}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$nor":[{"mother.name":{"$lt":"Tim"}},{"mother.address.code":{"$gt":386}}]})", s.str());
}
TEST(MacroTests, BinaryNot)
{
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using ThorsAnvil::DB::Mongo::QueryOp::Not;

    std::stringstream   s;
    ThorsMongo_FilterFromAccess(FilterLHS, Lt, Family, mother, name);
    ThorsMongo_FilterFromAccess(FilterRHS, Gt, Family, mother, address, code);
    using Filter = Not<And<FilterLHS, FilterRHS>>;

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"Tim",386}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$not":{"$and":[{"mother.name":{"$lt":"Tim"}},{"mother.address.code":{"$gt":386}}]}})", s.str());
}
TEST(MacroTests, ElementExists)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Exists, Family, mother, name);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{true}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.name":{"$exists":true}})", s.str());
}
TEST(MacroTests, ElementType)
{
    using ThorsAnvil::DB::Mongo::QueryOp::BsonType;
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Type, Family, mother, name);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{BsonType::Array}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.name":{"$type":4}})", s.str());
}
TEST(MacroTests, ExprMod)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Mod, Family, mother, age);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{12,2}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.age":{"$mod":[12,2]}})", s.str());
}
TEST(MacroTests, ExprRegEx)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, RegEx, Family, mother, address, city);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"*don","g"}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"mother.address.city":{"$regex":"*don","$options":"g"}})", s.str());
}
TEST(MacroTests, ArrayAll)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, All, PeopleWithFood, food);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"chicken","beans"}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"food":{"$all":["chicken","beans"]}})", s.str());
}
TEST(MacroTests, ArrayElemMatch)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, ElemMatch, PeopleWithFood, food);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{{"chicken",{},{},{},"Food",{}}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"food":{"$elemMatch":{"$eq":"chicken","$lt":"Food"}}})", s.str());
}
TEST(MacroTests, ArraySize)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, Size, PeopleWithFood, food);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{6}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"food":{"$size":6}})", s.str());
}
TEST(MacroTests, BitAllSet)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, AllSet, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{0b11011011}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"address.code":{"$bitsAllSet":219}})", s.str());
}
TEST(MacroTests, BitAllClear)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, AllClear, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{0b11011011}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"address.code":{"$bitsAllClear":219}})", s.str());
}
TEST(MacroTests, BitAnySet)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, AnySet, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{0b11011011}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"address.code":{"$bitsAnySet":219}})", s.str());
}
TEST(MacroTests, BitAnyClear)
{
    std::stringstream   s;
    ThorsMongo_FilterFromAccess(Filter, AnyClear, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Filter{0b11011011}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"address.code":{"$bitsAnyClear":219}})", s.str());
}

/* Update Macro Test */
TEST(MacroTests, UpdateInc)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Inc, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$inc":{"address.code":12}})", s.str());
}
TEST(MacroTests, UpdateMin)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Min, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$min":{"address.code":12}})", s.str());
}
TEST(MacroTests, UpdateMax)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Max, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$max":{"address.code":12}})", s.str());
}
TEST(MacroTests, UpdateMul)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Mul, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$mul":{"address.code":12}})", s.str());
}
TEST(MacroTests, UpdateReName)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Rename, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Update{"oldCode"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$rename":{"address.code":"oldCode"}})", s.str());
}
TEST(MacroTests, UpdateSet)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Set, People, address, city);

    s << ThorsAnvil::Serialize::jsonExporter(Update{"London"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$set":{"address.city":"London"}})", s.str());
}
TEST(MacroTests, UpdateSetOnInsert)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, SetOnInsert, People, address, street);

    s << ThorsAnvil::Serialize::jsonExporter(Update{"Gin Alley"}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$setOnInsert":{"address.street":"Gin Alley"}})", s.str());
}
TEST(MacroTests, UpdateUnset)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Unset, People, address, code);

    s << ThorsAnvil::Serialize::jsonExporter(Update{}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$unset":{"address.code":""}})", s.str());
}
TEST(MacroTests, UpdateAddToSet)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, AddToSet, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$addToSet":{"data":12}})", s.str());
}
TEST(MacroTests, UpdatePopFront)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, PopFront, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$pop":{"data":-1}})", s.str());
}
TEST(MacroTests, UpdatePopBack)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, PopBack, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$pop":{"data":1}})", s.str());
}
TEST(MacroTests, UpdatePull)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Pull, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$pull":{"data":12}})", s.str());
}
TEST(MacroTests, UpdatePush)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, Push, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{12}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$push":{"data":12}})", s.str());
}
TEST(MacroTests, UpdatePullAll)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, PullAll, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{{12,13,14}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$pullAll":{"data":[12,13,14]}})", s.str());
}

TEST(MacroTests, UpdateEach)
{
    std::stringstream   s;
    ThorsMongo_UpdateExtendFromAccess(Update, Push, Each, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{{12,13,14}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$push":{"data":{"$each":[12,13,14]}}})", s.str());
}
TEST(MacroTests, UpdatePosition)
{
    std::stringstream   s;
    ThorsMongo_UpdateExtendFromAccess(Update, AddToSet, Position, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{{{12,13,14}, 2}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$addToSet":{"data":{"$each":[12,13,14],"$position":2}}})", s.str());
}
TEST(MacroTests, UpdateSlice)
{
    std::stringstream   s;
    ThorsMongo_UpdateExtendFromAccess(Update, Push, Slice, People, data);

    s << ThorsAnvil::Serialize::jsonExporter(Update{{{12,13,14}, 0}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$push":{"data":{"$each":[12,13,14],"$slice":0}}})", s.str());
}
TEST(MacroTests, UpdateSort)
{
    std::stringstream   s;
    ThorsMongo_UpdateExtendFromAccess(Update, AddToSet, Sort, People, data);
    using ThorsAnvil::DB::Mongo::SortOrder;

    s << ThorsAnvil::Serialize::jsonExporter(Update{{{12,13,14}, SortOrder::Ascending}}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$addToSet":{"data":{"$each":[12,13,14],"$sort":1}}})", s.str());
}
TEST(MacroTests, UpdateCurrentDateSetDate)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, CurrentDateSetDate, People, age);
    using ThorsAnvil::DB::Mongo::SortOrder;

    s << ThorsAnvil::Serialize::jsonExporter(Update{}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$currentDate":{"age":{"$type":"date"}}})", s.str());
}
TEST(MacroTests, UpdateCurrentDateSetTimeStamp)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, CurrentDateSetTimeStamp, People, age);
    using ThorsAnvil::DB::Mongo::SortOrder;

    s << ThorsAnvil::Serialize::jsonExporter(Update{}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$currentDate":{"age":{"$type":"timestamp"}}})", s.str());
}
TEST(MacroTests, UpdateCurrentDate)
{
    std::stringstream   s;
    ThorsMongo_UpdateFromAccess(Update, CurrentDate, People, age);
    using ThorsAnvil::DB::Mongo::SortOrder;

    s << ThorsAnvil::Serialize::jsonExporter(Update{}, PrinterInterface::PrinterConfig{PrinterInterface::OutputType::Stream});
    EXPECT_EQ(R"({"$currentDate":{"age":true}})", s.str());
}
/*
struct CurrentDate
using CurrentDateSetDate = CurrentDate<T>;
using CurrentDateSetTimeStamp = CurrentDate<T>;
*/
