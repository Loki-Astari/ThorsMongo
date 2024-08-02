#include "gtest/gtest.h"

#include "ConnectionMongo.h"
#include "ThorsMongo.h"
#include "Authenticate.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "MongoQuery.h"
#include "MongoCursor.h"
#include "AuthenticateScramSha.h"
#include "ThorSerialize/Traits.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::ConnectionMongo;
using ThorsAnvil::DB::Mongo::MongoMessageHandler;

using ThorsAnvil::DB::Mongo::Authenticate;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Client;

using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::Query;
using ThorsAnvil::DB::Mongo::Projection;
using ThorsAnvil::DB::Mongo::SortOrder;
using ThorsAnvil::DB::Mongo::CollectionInfo;

using ThorsAnvil::DB::Mongo::FindConfig;
using ThorsAnvil::DB::Mongo::FAModifyConfig;
using ThorsAnvil::DB::Mongo::FARemoveConfig;
using ThorsAnvil::DB::Mongo::CountConfig;
using ThorsAnvil::DB::Mongo::DistinctConfig;

using ThorsAnvil::DB::Mongo::InsertResult;
using ThorsAnvil::DB::Mongo::RemoveResult;
using ThorsAnvil::DB::Mongo::FindResult;
using ThorsAnvil::DB::Mongo::FAModifyResult;
using ThorsAnvil::DB::Mongo::CountResult;
using ThorsAnvil::DB::Mongo::DistinctResult;
using ThorsAnvil::DB::Mongo::ListCollectionResult;
using ThorsAnvil::DB::Mongo::ListDatabaseResult;
using ThorsAnvil::DB::Mongo::AdminResult;

using ThorsAnvil::DB::Mongo::FindRange;
using ThorsAnvil::DB::Mongo::LCRange;
using ThorsAnvil::DB::Mongo::DBRange;

using namespace ThorsAnvil::DB::Mongo::QueryOp;

namespace ThorsAnvil::DB::Mongo
{
template<typename R>
struct TestFindResult
{
    using T = typename R::ValueType;

    ThorsAnvil::DB::Mongo::CursorFirst<T>&              cursor;
    std::size_t&                                        index;
    double&                                             ok;
    R&                                                  result;
    TestFindResult(Range<R>& range)
        : cursor(range.getResult().cursor)
        , index(range.getResult().index)
        , ok(range.getResult().ok)
        , result(range.getResult())
    {}
};
}
using ThorsAnvil::DB::Mongo::TestFindResult;


TEST(IntegrationConnectionMongoTest, connectToMongoWithAuthenticator)
{
    ConnectionMongo     mongo({"localhost", 27017});
    MongoMessageHandler sender(mongo);
    Authenticate        authenticate;

    authenticate.addAuthenticator("SCRAM-SHA-256", ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate);
    authenticate.handShake(sender, {"test", "testPassword", "test"}, Compression::None, Client{"IntegrationConnectionMongoTest::connectToMongo"});
}

TEST(IntegrationConnectionMongoTest, connectToMongoWithAuthenticatorUsingSnappy)
{
    ConnectionMongo     mongo({"localhost", 27017});
    MongoMessageHandler sender(mongo);
    Authenticate        authenticate;

    authenticate.addAuthenticator("SCRAM-SHA-256", ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate);
    authenticate.handShake(sender, {"root", "underMongo", "admin"}, Compression::Snappy, Client{"IntegrationConnectionMongoTest::connectToMongo"});
}

TEST(IntegrationConnectionMongoTest, insertData)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}};

    InsertResult        result = mongo["test"]["People"].insert(people);

    EXPECT_EQ(1, result.ok);
    EXPECT_EQ("", result.errmsg);
    EXPECT_EQ("", result.codeName);
    EXPECT_EQ(2, result.n);
    EXPECT_EQ(2, result.inserted.size());
    EXPECT_EQ(0, result.writeErrors.size());
}

TEST(IntegrationConnectionMongoTest, removeData)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(Query<NameField<std::string>>{"Sam", 1}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::vector<Query<NameField<std::string>>>{{"Sam", 0}});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(1, r1Result.n);   // Should only delete one Sam
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_GE(r2Result.n, 2);   // Delete any remaining "Sam"
                                // There is 2 from this test.
                                // But we also get rid of the Sam from any previous tests

}

TEST(IntegrationConnectionMongoTest, insertRemoveTuple)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    auto                add{std::make_tuple(People{"Pam", 46, {"Lane", "WA", 65}}, Funky{"time"}, Splotch{1,2,3})};
    auto                rem{std::make_tuple(Query<NameField<std::string>>{"Pam"}, Query<Funky>{"time"}, Query<LengthField<int>>{1})};

    InsertResult        iResult = mongo["test"]["People"].insert(add);
    RemoveResult        rResult = mongo["test"]["People"].remove(rem);

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, rResult.ok);
    EXPECT_EQ(3, rResult.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryEq)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameEQ       = Query<NameField<Eq<std::string>>>;
    using VEQ           = std::vector<QNameEQ>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VEQ{{"Tom"}});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VEQ{{"Sam"}});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryNe)
{
    using namespace std::string_literals;
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameNE       = Query<NameField<Ne<std::string>>>;
    using VNE           = std::vector<QNameNE>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VNE{"Sam"s});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VNE{"Tom"s});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryGt)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameGT       = Query<AgeField<Gt<int>>>;
    using VGT           = std::vector<QNameGT>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VGT{56});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VGT{20});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryGte)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameGTE      = Query<AgeField<Gte<int>>>;
    using VGTE          = std::vector<QNameGTE>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VGTE{57});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VGTE{22});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryLt)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameLT       = Query<AgeField<Lt<int>>>;
    using VLT           = std::vector<QNameLT>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VLT{22});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VLT{58});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryLte)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameLTE      = Query<AgeField<Lte<int>>>;
    using VLTE          = std::vector<QNameLTE>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VLTE{21});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VLTE{56});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryIn)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameIN       = Query<AgeField<In<int>>>;
    using VIN           = std::vector<QNameIN>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VIN{{25,26,48}});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VIN{{23,22,45}});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryNin)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameNIN      = Query<AgeField<Nin<int>>>;
    using VNIN          = std::vector<QNameNIN>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(VNIN{{22,23,45}});
    RemoveResult        r2Result = mongo["test"]["People"].remove(VNIN{{25,26,48}});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryAnd)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameAND      = Query<And<NameField<std::string>, AgeField<Lt<int>>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameAND{{"Sam", 23}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameAND{{"Sam", 58}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(1, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(2, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryOr)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameOR       = Query<Or<NameField<std::string>, AgeField<Lt<int>>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameOR{{"Tom", 23}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameOR{{"Sam", 18}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(1, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(2, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryNor)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameNOR      = Query<Nor<NameField<std::string>, AgeField<Lt<int>>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameNOR{{"Sam", 22}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameNOR{{"Tom", 30}}));
    RemoveResult        r3Result = mongo["test"]["People"].remove(std::make_tuple(QNameNOR{{"Tom", 12}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(1, r2Result.n);
    EXPECT_EQ(1, r3Result.ok);
    EXPECT_EQ(2, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryNot)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameNOT       = Query<NameField<Not<Eq<std::string>>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameNOT{"Sam"}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameNOT{"Tom"}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryExists)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameEXISTS   = Query<NameField<Exists>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameEXISTS{false}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameEXISTS{true}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryType)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 22, {"Court", "NY", 12}}, {"Sam", 23, {"Jester", "FW", 23}}, {"Sam", 45, {"Limbo", "FG", 56}}};
    using QNameEXISTS   = Query<NameField<Type>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameEXISTS{BsonType::Double}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameEXISTS{BsonType::String}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryMod)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 25, {"Court", "NY", 12}}, {"Sam", 37, {"Jester", "FW", 23}}, {"Sam", 49, {"Limbo", "FG", 56}}};
    using QNameMod      = Query<AgeField<Mod>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameMod{{12, 0}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameMod{{12, 1}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryRegEx)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Samual", 25, {"Court", "NY", 12}}, {"Samantha", 37, {"Jester", "FW", 23}}, {"Samtra", 49, {"Limbo", "FG", 56}}};
    using QNameRegEx    = Query<NameField<RegEx>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QNameRegEx{{"Samp.*", "i"}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QNameRegEx{{"Sam.*", "i"}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryText)
{
    // TODO
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, removeQueryAll)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
        {"Sam", 25, {"Cour terror",  "NY", 12}, {{10, 13, 14, 15}}},
        {"Sam", 37, {"Jes terror",   "FW", 23}, {{13, 14, 15}}},
        {"Sam", 49, {"Limbo terror", "FG", 56}, {{10, 14, 15}}}
    };
    using QDataAll      = Query<DataField<All<int>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QDataAll{{10, 12, 13, 14}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QDataAll{{14, 15}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryElemMatch)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
        {"Sam", 25, {"Cour terror",  "NY", 12}, {{10, 13, 14, 15}}},
        {"Sam", 37, {"Jes terror",   "FW", 23}, {{13, 14, 15}}},
        {"Sam", 49, {"Limbo terror", "FG", 56}, {{10, 14, 15}}}
    };
    using QDataElemMatch      = Query<DataField<ElemMatch<int>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QDataElemMatch{{25, 14, {}, {}, {}, {}}}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QDataElemMatch{{14, 15, {}, {}, {}, {}}}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryElemSize)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
        {"Sam", 25, {"Cour terror",  "NY", 12}, {{10, 13, 14}}},
        {"Sam", 37, {"Jes terror",   "FW", 23}, {{13, 14, 15}}},
        {"Sam", 49, {"Limbo terror", "FG", 56}, {{10, 14, 15}}}
    };
    using QDataSize      = Query<DataField<Size>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QDataSize{2}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QDataSize{3}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryAllClear)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 17, {"Cour terror",  "NY", 12}}, {"Sam", 25, {"Jes terror",   "FW", 23}}, {"Sam", 33, {"Limbo terror", "FG", 56}}};
    using QAgeAllClear  = Query<AgeField<AllClear>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAllClear{0b11000111}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAllClear{0b11000110}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryAllSet)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 17, {"Cour terror",  "NY", 12}}, {"Sam", 25, {"Jes terror",   "FW", 23}}, {"Sam", 33, {"Limbo terror", "FG", 56}}};
    using QAgeAllSet    = Query<AgeField<AllSet>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAllSet{0b00111001}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAllSet{0b00000001}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}



TEST(IntegrationConnectionMongoTest, removeQueryAnyClear)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 17, {"Cour terror",  "NY", 12}}, {"Sam", 25, {"Jes terror",   "FW", 23}}, {"Sam", 33, {"Limbo terror", "FG", 56}}};
    using QAgeAnyClear  = Query<AgeField<AnyClear>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAnyClear{0b00000001}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAnyClear{0b11111110}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, removeQueryAnySet)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 17, {"Cour terror",  "NY", 12}}, {"Sam", 25, {"Jes terror",   "FW", 23}}, {"Sam", 33, {"Limbo terror", "FG", 56}}};
    using QAgeAnySet    = Query<AgeField<AnySet>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    RemoveResult        r1Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAnySet{0b10000110}));
    RemoveResult        r2Result = mongo["test"]["People"].remove(std::make_tuple(QAgeAnySet{0b11111111}));

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(0, r1Result.n);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, findQueryEq)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 45, {"Jes terror",   "FW", 23}}, {"John", 45, {"Limbo terror", "FG", 56}}};
    using FindName      = NameField<Eq<std::string>>;
    using FindAge       = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindName{"John"});
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{45});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(2, findResult.cursor.data().size());
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, ListCollections)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});

    LCRange                   r1Result = mongo["test"].listCollections();
    TestFindResult<ListCollectionResult>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(0, findResult.cursor.getId());

    int count = 0;
    for (auto const& v: r1Result) {
        ++count;
    }
    EXPECT_NE(0, count);
}

TEST(IntegrationConnectionMongoTest, ListDatabases)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});

    DBRange                   r1Result = mongo.listDatabases();
    EXPECT_EQ(1, r1Result.rangeData->ok);

    int count = 0;
    for (auto const& v: r1Result) {
        ++count;
    }
    EXPECT_NE(0, count);
}

TEST(IntegrationConnectionMongoTest, RenameCollection)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});

    LCRange             collections1 = mongo["test"].listCollections();
    auto find1 = std::find_if(std::begin(collections1), std::end(collections1), [](CollectionInfo const& v) {return v.name == "Blob";});
    ASSERT_NE(find1, std::end(collections1));

    AdminResult         r1Result = mongo["test"]["Blob"].rename("Blob1");
    EXPECT_EQ(1, r1Result.ok);

    // Check
    LCRange             collections2 = mongo["test"].listCollections();
    auto find2 = std::find_if(std::begin(collections2), std::end(collections2), [](CollectionInfo const& v) {return v.name == "Blob";});
    ASSERT_EQ(find2, std::end(collections2));

    // Put Back.
    AdminResult         r2Result = mongo["test"]["Blob1"].rename("Blob");
    EXPECT_EQ(1, r2Result.ok);
}

TEST(IntegrationConnectionMongoTest, CreateDropCollection)
{
    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});

    LCRange             collections1 = mongo["test"].listCollections();
    auto find1 = std::find_if(std::begin(collections1), std::end(collections1), [](CollectionInfo const& v) {return v.name == "CreateTest";});
    ASSERT_EQ(find1, std::end(collections1));

    AdminResult         r1Result = mongo["test"].createCollection("CreateTest");
    EXPECT_EQ(1, r1Result.ok);
    std::cerr << ThorsAnvil::Serialize::jsonExporter(r1Result) << "\n\n";

    // Check
    LCRange             collections2 = mongo["test"].listCollections();
    auto find2 = std::find_if(std::begin(collections2), std::end(collections2), [](CollectionInfo const& v) {return v.name == "CreateTest";});
    ASSERT_NE(find2, std::end(collections2));

    AdminResult         r2Result = mongo["test"]["CreateTest"].drop();
    EXPECT_EQ(1, r2Result.ok);
    std::cerr << ThorsAnvil::Serialize::jsonExporter(r1Result) << "\n\n";
    std::cerr << ThorsAnvil::Serialize::jsonExporter(r2Result) << "\n\n";

    LCRange             collections3 = mongo["test"].listCollections();
    auto find3 = std::find_if(std::begin(collections3), std::end(collections3), [](CollectionInfo const& v) {return v.name == "CreateTest";});
    ASSERT_EQ(find3, std::end(collections3));
}

TEST(IntegrationConnectionMongoTest, SerializeFindAddSort)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setSort({{"name", SortOrder::Descending}, {"age", SortOrder::Ascending}})
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(3, findResult.cursor.data().size());
    EXPECT_EQ("Sam", findResult.cursor.data()[0].name);
    EXPECT_EQ("John", findResult.cursor.data()[1].name);
    EXPECT_EQ("John", findResult.cursor.data()[2].name);
    EXPECT_EQ(38, findResult.cursor.data()[1].age);
    EXPECT_EQ(45, findResult.cursor.data()[2].age);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindAddProjection)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"John", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setProjection(Projection::create<PeopleWithAddressCode>())
    );

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(3, findResult.cursor.data().size());
    EXPECT_EQ("John", findResult.cursor.data()[0].name);
    EXPECT_EQ("John", findResult.cursor.data()[1].name);
    EXPECT_EQ("John", findResult.cursor.data()[2].name);
    std::vector<int>    codes = {findResult.cursor.data()[0].address.code, findResult.cursor.data()[1].address.code, findResult.cursor.data()[2].address.code};
    std::sort(std::begin(codes), std::end(codes));
    std::vector<int>    expectedCodes{12,23,56};
    EXPECT_EQ(expectedCodes, codes);
    EXPECT_EQ("John", findResult.cursor.data()[1].name);
    EXPECT_EQ("John", findResult.cursor.data()[2].name);
    EXPECT_EQ(0, findResult.cursor.data()[0].age);
    EXPECT_EQ(0, findResult.cursor.data()[1].age);
    EXPECT_EQ(0, findResult.cursor.data()[2].age);
    EXPECT_EQ("", findResult.cursor.data()[0].address.street);
    EXPECT_EQ("", findResult.cursor.data()[1].address.street);
    EXPECT_EQ("", findResult.cursor.data()[2].address.street);
    EXPECT_EQ("", findResult.cursor.data()[0].address.city);
    EXPECT_EQ("", findResult.cursor.data()[1].address.city);
    EXPECT_EQ("", findResult.cursor.data()[2].address.city);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetSkip)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setSort({{"name", SortOrder::Descending}, {"age", SortOrder::Ascending}})
        .setSkip(2)
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(1, findResult.cursor.data().size());
    EXPECT_EQ("John", findResult.cursor.data()[0].name);
    EXPECT_EQ(45, findResult.cursor.data()[0].age);
    EXPECT_EQ(23, findResult.cursor.data()[0].address.code);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetLimit)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setSort({{"name", SortOrder::Descending}, {"age", SortOrder::Ascending}})
        .setSkip(1)
        .setLimit(1)
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(1, findResult.cursor.data().size());
    EXPECT_EQ("John", findResult.cursor.data()[0].name);
    EXPECT_EQ(38, findResult.cursor.data()[0].age);
    EXPECT_EQ(56, findResult.cursor.data()[0].address.code);
    EXPECT_EQ(1, r2Result.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetBatchSize)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setBatchSize(1)
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(1, findResult.cursor.data().size());
    EXPECT_EQ(3, r2Result.n);
    EXPECT_NE(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetSingleBatch)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setBatchSize(1)
        .setSingleBatch(true)
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(1, findResult.cursor.data().size());
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetComment)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setComment("Comment Time")
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetMaxTimeMS)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setMaxTimeMS(256)
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetHint)
{
    // TODO
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetSetMax)
{
    // TODO
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetMin)
{
    // TODO
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetReturnKey)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setReturnKey(true)
    );
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<People>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(3, findResult.cursor.data().size());
    EXPECT_EQ("", findResult.cursor.data()[0].name);
    EXPECT_EQ("", findResult.cursor.data()[1].name);
    EXPECT_EQ("", findResult.cursor.data()[2].name);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetShowRecordId)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{{"John", 45, {"Jes terror",   "FW", 23}},{"Sam", 45, {"Cour terror",  "NY", 12}}, {"John", 38, {"Limbo terror", "FG", 56}}};
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult                iResult = mongo["test"]["People"].insert(people);
    FindRange<PeopleWithRecordID>   r1Result = mongo["test"]["People"].find<PeopleWithRecordID>(FindAge{12}, FindConfig{}
        .setShowRecordId(true)
    );
    RemoveResult                r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});

    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(3, iResult.n);
    EXPECT_EQ(3, iResult.inserted.size());

    TestFindResult<FindResult<PeopleWithRecordID>>  findResult(r1Result);
    EXPECT_EQ(1, findResult.ok);
    EXPECT_EQ(3, findResult.cursor.data().size());
    EXPECT_NE(0, findResult.cursor.data()[0].$recordId);
    EXPECT_NE(0, findResult.cursor.data()[1].$recordId);
    EXPECT_NE(0, findResult.cursor.data()[2].$recordId);
    EXPECT_EQ(3, r2Result.n);
    EXPECT_EQ(0, findResult.cursor.getId());
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetTailablPlusAwait)
{
    // TODO: This test requires a capped collection.
    //       Need to work out how to do that.
    //       Currently this generates an error from the server.
    //       Use this test to test both Tailable and AwaitData.
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetNoCursorTimeout)
{
    // TODO: How do we test this without waiting for 30 minutes?
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetCollation)
{
    // TODO: Not sure how to test that this is working
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetAllowDiskUse)
{
    // TODO: How do we test this.
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindSetLet)
{
    // TODO: Testing this works depends on expressions.
    //       Currently we don't support expressions.
    //       So further work needs to be done to find a way to represent expressions.
    //       Then update this integration test to make sure it works with variables.
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, SerializeFindAndIterate)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAge       = AgeField<Gt<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FindRange<People>   r1Result = mongo["test"]["People"].find<People>(FindAge{12}, FindConfig{}
        .setBatchSize(2)
    );
    std::vector<std::uint32_t>  age;
    std::vector<std::uint32_t>  code;

    for (auto const& v: r1Result)
    {
        age.emplace_back(v.age);
        code.emplace_back(v.address.code);

        EXPECT_EQ(v.age + 3, v.address.code);
    }
    EXPECT_EQ(9, age.size());
    std::sort(std::begin(age), std::end(age));
    std::vector<std::uint32_t>  expectedResult{22, 23, 25, 28, 32, 36, 38, 45, 49};
    EXPECT_EQ(age, expectedResult);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAge>{0});
    EXPECT_EQ(9, r2Result.n);
}


TEST(IntegrationConnectionMongoTest, FindAndReplace)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{32}, People{"Tom", 128, {}});
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,           r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(8, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUsingSortAscending)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},   // Two people aged 32
                                {"Lam",  32, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{32}, People{"Tom", 128, {}}, FAModifyConfig{}.setSort({{"name",SortOrder::Ascending}}));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Lam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Limbo terror",r1Result.value->address.street);
    EXPECT_EQ("FG",          r1Result.value->address.city);
    EXPECT_EQ(41,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(8, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUsingSortDescending)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},   // Two people aged 32
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  32, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{32}, People{"Tom", 128, {}}, FAModifyConfig{}.setSort({{"name",SortOrder::Descending}}));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Ted",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Line Flog",   r1Result.value->address.street);
    EXPECT_EQ("TW",          r1Result.value->address.city);
    EXPECT_EQ(39,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(8, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUsingNew)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{32}, People{"Tom", 128, {}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Tom",         r1Result.value->name);
    EXPECT_EQ(128,           r1Result.value->age);
    EXPECT_EQ("",            r1Result.value->address.street);
    EXPECT_EQ("",            r1Result.value->address.city);
    EXPECT_EQ(0,             r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(8, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUsingFields)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{32}, People{"Tom", 128, {}}, FAModifyConfig{}.setFields(Projection::create<PeopleWithAddressCode>()));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(0,             r1Result.value->age);
    EXPECT_EQ("",            r1Result.value->address.street);
    EXPECT_EQ("",            r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(8, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUpsertFalse)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{128}, People{"Tom", 128, {}}, FAModifyConfig{}.setUpsert(false));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_EQ(nullptr,       r1Result.value.get());
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUpsertTrue)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{128}, People{"Tom", 128, {}}, FAModifyConfig{}
        .setUpsert(true)
    );

    EXPECT_EQ(1, r1Result.ok);
    ASSERT_EQ(nullptr,       r1Result.value.get());
    EXPECT_EQ(false, r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceUpsertTrueMatch)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndReplaceOne<People>(FindAgeEq{32}, People{"Tom", 128, {}}, FAModifyConfig{}.setUpsert(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,           r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(8, r2Result.n);
    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(1, r3Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndReplaceByPassDocumentValidation)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceWriteConcern)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceMaxTimeMS)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceCollation)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceArrayFilters)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceHint)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceComment)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndReplaceLet)
{
    // TODO
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, FindAndRemove)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndRemoveOne<People>(FindAgeEq{32});
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(false,         r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(8, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndRemoveSortAsc)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}}, // Two people are 32
                                {"Lam",  32, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndRemoveOne<People>(FindAgeEq{32}, FARemoveConfig{}.setSort({{"name", SortOrder::Ascending}}));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Lam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Limbo terror",r1Result.value->address.street);
    EXPECT_EQ("FG",          r1Result.value->address.city);
    EXPECT_EQ(41,            r1Result.value->address.code);
    EXPECT_EQ(false,         r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(8, r2Result.n);
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveSortDec)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}}, // Two people aged 32
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  32, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndRemoveOne<People>(FindAgeEq{32}, FARemoveConfig{}.setSort({{"name", SortOrder::Descending}}));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Ted",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Line Flog",   r1Result.value->address.street);
    EXPECT_EQ("TW",          r1Result.value->address.city);
    EXPECT_EQ(39,            r1Result.value->address.code);
    EXPECT_EQ(false,         r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(8, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndRemoveMiss)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndRemoveOne<People>(FindAgeEq{33});
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_EQ(nullptr,       r1Result.value.get());
    EXPECT_EQ(false,         r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndRemoveFields)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndRemoveOne<People>(FindAgeEq{32}, FARemoveConfig{}.setFields(Projection::create<PeopleWithAddressCode>()));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(0,             r1Result.value->age);
    EXPECT_EQ("",            r1Result.value->address.street);
    EXPECT_EQ("",            r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(false,         r1Result.lastErrorObject.updatedExisting);
    // ObjectID        upserted;

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{200});
    EXPECT_EQ(8, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndRemoveByPassDocumentValidation)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveWriteConcern)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveMaxTimeMS)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveCollation)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveHint)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveComment)
{
    // TODO
    GTEST_SKIP();
}
TEST(IntegrationConnectionMongoTest, FindAndRemoveLet)
{
    // TODO
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingSet)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Set<AgeField<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{12}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(12,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,           r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingCurrentDateDateTime)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = CurrentDate<DateField, SetDate>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<PeopleWithTime>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithTime>(FindAgeEq{32}, Update{}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);
    std::int64_t    milli = r1Result.value->dateTime.getMillSecSinceEpoch();
    std::int64_t    sec   = milli / 1000 / 2;

    auto            now   = std::chrono::system_clock::now();
    auto            nowS  = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() / 2;

    // Check the updated time was in the last 10 seconds.
    // Just rying to allow for some time drift and rounding errors.
    EXPECT_EQ(sec, nowS);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingCurrentDateTimeStamp)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = CurrentDate<TimeStampField, SetDate>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<PeopleWithTime>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithTime>(FindAgeEq{32}, Update{}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    //std::cerr << ThorsAnvil::Serialize::jsonExporter(Update{}) << "\n";
    //std::cerr << ThorsAnvil::Serialize::jsonExporter(r1Result) << "\n";
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    // This is an internal mongo type.
    // No documentation about it.
    // This should be seconds since epoch but if it is then it is strangely encoded.
    // TODO: Work out how to check the timestamp.
    //       See: ThorsAnvil::Serialize::MongoUtility::BsonTimeStamp
    //       for details about reading from the MongoDB.
    //       it could be done incorrectly there.
    std::uint32_t   sec   = r1Result.value->timeStamp.getSecSinceEpoch();
    std::uint64_t   inc   = r1Result.value->timeStamp.getIncrement();

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingIncrement)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Inc<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{4}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(36,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingMinFail)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Min<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{36}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingMinWork)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Min<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{12}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(12,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingMaxFail)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Max<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{12}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingMaxWork)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Max<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{56}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(56,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingMul)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update        = Mul<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{2}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(64,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingRename)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Rename<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{"originalAge"}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(0,             r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    // The age field nolonger exists.
    // So you can't remove it.
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(0, r2Result.n);

    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindName>{"Sam"});
    EXPECT_EQ(0, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingUnset)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Unset<AgeField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, Update{}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(0,             r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    // The age field was removed so we should not be able to find it.
    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(0, r2Result.n);

    RemoveResult        r3Result = mongo["test"]["People"].remove(Query<FindName>{"Sam"});
    EXPECT_EQ(0, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingAddToSet)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = AddToSet<FoodField<std::string>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs", "Ham"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{"Ham"}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPopFront)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = PopFront<FoodField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Eggs", "Ham"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPopBack)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = PopBack<FoodField>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPull)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Pull<FoodField<std::string>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Ham"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{"Eggs"}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPullWithTest)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Pull<FoodField<Lt<std::string>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Ham"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{"Figgs"}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPush)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Push<FoodField<std::string>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs", "Ham", "Figgs"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{"Figgs"}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPullAll)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = PullAll<FoodField, std::string>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{{"Figgs", "Ham"}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingEachAddToSet)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = AddToSet<FoodField<Each<std::string>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs", "Ham", "Figgs", "Beans", "Bunns"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{{"Figgs", "Beans", "Bunns"}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingEachPush)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Push<FoodField<Each<std::string>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs", "Ham", "Figgs", "Beans", "Bunns"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{{"Figgs", "Beans", "Bunns"}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingPosition)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Push<FoodField<Position<std::string>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Honey", "Toast", "Eggs", "Ham"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{{{"Honey", "Toast"}, 1}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingSlice)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Eggs", "Ham"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Push<FoodField<Slice<std::string>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{{{"Honey", "Toast"}, 0}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingSort)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<PeopleWithFood> people
                                {
                                    {"Sam",  32, {"Cour terror",  "NY", 35}, {}}
                                };
    people[0].food = {"Beacon", "Ham", "Eggs"};
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using FindName      = NameField<Eq<std::string>>;
    using Update        = Push<FoodField<Sort<std::string>>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    std::vector<std::string>    expectedFood{"Beacon", "Eggs", "Ham", "Honey", "Toast"};
    FAModifyResult<PeopleWithFood>  r1Result = mongo["test"]["People"].findAndUpdateOne<PeopleWithFood>(FindAgeEq{32}, Update{{{"Toast", "Honey"}, SortOrder::Ascending}}, FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Sam",         r1Result.value->name);
    EXPECT_EQ(32,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(expectedFood,  r1Result.value->food);
    EXPECT_EQ(true,          r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, FindAndUpdateUsingMultipleExpressions)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;
    using Update1       = Set<AgeField<std::uint32_t>>;
    using Update2       = Set<NameField<std::string>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(1, iResult.n);
    EXPECT_EQ(1, iResult.inserted.size());

    FAModifyResult<People>  r1Result = mongo["test"]["People"].findAndUpdateOne<People>(FindAgeEq{32}, std::make_tuple(Update1{12}, Update2{"Tim"}), FAModifyConfig{}.setReturnNew(true));
    EXPECT_EQ(1, r1Result.ok);
    ASSERT_NE(nullptr,       r1Result.value.get());
    EXPECT_EQ("Tim",         r1Result.value->name);
    EXPECT_EQ(12,            r1Result.value->age);
    EXPECT_EQ("Cour terror", r1Result.value->address.street);
    EXPECT_EQ("NY",          r1Result.value->address.city);
    EXPECT_EQ(35,            r1Result.value->address.code);
    EXPECT_EQ(true,           r1Result.lastErrorObject.updatedExisting);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(1, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, Count)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 32, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    CountResult         r1Result = mongo["test"]["People"].countDocuments(FindAgeEq{32}, CountConfig{});
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(2, r1Result.n);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, CountWithLimit)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",32, {"Glome Blob",   "FV", 26}},
                                {"Litle",32, {"Time Bob",     "HB", 28}},
                                {"Klin", 32, {"Court Film",   "PL", 52}},
                                {"Blow", 32, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    CountResult         r1Result = mongo["test"]["People"].countDocuments(FindAgeEq{32}, CountConfig{}.setLimit(3));
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(3, r1Result.n);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, CountWithSkip)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  32, {"Limbo terror", "FG", 41}},
                                {"Ted",  32, {"Line Flog",    "TW", 39}},
                                {"Rose", 32, {"Twine Forge",  "GB", 25}},
                                {"Blond",32, {"Glome Blob",   "FV", 26}},
                                {"Litle",32, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 32, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    CountResult         r1Result = mongo["test"]["People"].countDocuments(FindAgeEq{32}, CountConfig{}.setSkip(3));
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(4, r1Result.n);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, CountWithHint)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, CountWithReadConcern)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, CountWithMaxTimeMS)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, CountCollation)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, CountWithComment)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, CountAll)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    CountResult         r1Result = mongo["test"]["People"].countDocuments(CountConfig{});
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(9, r1Result.n);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, Distinct)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    DistinctResult<std::string> r1Result = mongo["test"]["People"].distinct<std::string>("name", FindAgeLt{37}, DistinctConfig{});
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(6, r1Result.values.size());
    std::vector<std::string>    expectedResults{"Blond", "Blow", "Litle", "Rose", "Sam", "Ted"};
    std::sort(std::begin(r1Result.values), std::end(r1Result.values));
    EXPECT_EQ(expectedResults, r1Result.values);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

TEST(IntegrationConnectionMongoTest, DistinctWithReadConcern)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, DistinctWithCollation)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, DistinctWithComment)
{
    GTEST_SKIP();
}

TEST(IntegrationConnectionMongoTest, DistinctAll)
{
    using namespace std::string_literals;

    ThorsMongo          mongo({"localhost", 27017}, {"test", "testPassword", "test"});
    std::vector<People> people{
                                {"John", 45, {"Jes terror",   "FW", 48}},
                                {"Sam",  32, {"Cour terror",  "NY", 35}},
                                {"Lam",  38, {"Limbo terror", "FG", 41}},
                                {"Ted",  36, {"Line Flog",    "TW", 39}},
                                {"Rose", 22, {"Twine Forge",  "GB", 25}},
                                {"Blond",23, {"Glome Blob",   "FV", 26}},
                                {"Litle",25, {"Time Bob",     "HB", 28}},
                                {"Klin", 49, {"Court Film",   "PL", 52}},
                                {"Blow", 28, {"Court Port",   "PL", 31}}
                              };
    using FindAgeLt     = AgeField<Lt<std::uint32_t>>;
    using FindAgeEq     = AgeField<Eq<std::uint32_t>>;

    InsertResult        iResult = mongo["test"]["People"].insert(people);
    EXPECT_EQ(1, iResult.ok);
    EXPECT_EQ(9, iResult.n);
    EXPECT_EQ(9, iResult.inserted.size());

    DistinctResult<std::string> r1Result = mongo["test"]["People"].distinct<std::string>("name", DistinctConfig{});
    EXPECT_EQ(1, r1Result.ok);
    EXPECT_EQ(9, r1Result.values.size());
    std::vector<std::string>    expectedResults{"Blond", "Blow", "John", "Klin", "Lam", "Litle", "Rose", "Sam", "Ted"};
    std::sort(std::begin(r1Result.values), std::end(r1Result.values));
    EXPECT_EQ(expectedResults, r1Result.values);

    RemoveResult        r2Result = mongo["test"]["People"].remove(Query<FindAgeLt>{100});
    EXPECT_EQ(9, r2Result.n);
}

