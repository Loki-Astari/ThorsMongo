#include "gtest/gtest.h"

#include "ConnectionMongo.h"
#include "ThorsMongo.h"
#include "Authenticate.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "MongoQuery.h"
#include "AuthenticateScramSha.h"
#include "ThorSerialize/Traits.h"
#include "test/Action.h"

using ThorsAnvil::DB::Mongo::ConnectionMongo;
using ThorsAnvil::DB::Mongo::MongoMessageHandler;
using ThorsAnvil::DB::Mongo::Authenticate;
using ThorsAnvil::DB::Mongo::Compression;
using ThorsAnvil::DB::Mongo::Auth::Client;
using ThorsAnvil::DB::Mongo::ThorsMongo;
using ThorsAnvil::DB::Mongo::InsertResult;
using ThorsAnvil::DB::Mongo::RemoveResult;
using ThorsAnvil::DB::Mongo::Query;
using namespace ThorsAnvil::DB::Mongo::QueryOp;

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

