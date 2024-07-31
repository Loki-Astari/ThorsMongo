#include "gtest/gtest.h"
#include "test/Action.h"

#include "ThorsMongo.h"
#include "MongoQuery.h"

using ThorsAnvil::DB::Mongo::Action::Finder;
using ThorsAnvil::DB::Mongo::PrinterConfig;
using ThorsAnvil::DB::Mongo::Projection;
using ThorsAnvil::DB::Mongo::FindConfig;
using ThorsAnvil::DB::Mongo::SortOrder;
using ThorsAnvil::DB::Mongo::ReadConcern;
using ThorsAnvil::DB::Mongo::Level;
using ThorsAnvil::DB::Mongo::Query;
using namespace ThorsAnvil::DB::Mongo::QueryOp;


TEST(ThorsMongFindTest, ConstructProjection)
{
    Projection      projection = Projection::create<People>();

    using namespace std::string_literals;
    EXPECT_EQ("\x46\x00\x00\x00"
                "\x10"  "name\x00"      "\x01\x00\x00\x00"
                "\x10"  "age\x00"       "\x01\x00\x00\x00"
                "\x03"  "address\x00"
                    "\x25\x00\x00\x00"
                        "\x10"  "street\x00"    "\x01\x00\x00\x00"
                        "\x10"  "city\x00"      "\x01\x00\x00\x00"
                        "\x10"  "code\x00"      "\x01\x00\x00\x00"
                        "\x00"
                "\x00"s,
              projection.getData());
}


TEST(ThorsMongFindTest, ConstructFindConfig)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddSort)
{
    FindConfig      findConfig;

    findConfig.setSort({{"name", SortOrder::Ascending}, {"age", SortOrder::Descending}});
    EXPECT_TRUE(findConfig.getSort().has_value());

    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddProjection)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    findConfig.setProjection(Projection::create<People>());
    EXPECT_TRUE(findConfig.getProjection().has_value());

    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddHint)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    findConfig.setHint("Hit");
    EXPECT_TRUE(findConfig.getHint().has_value());

    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindSetSkip)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    findConfig.setSkip(32);
    EXPECT_TRUE(findConfig.getSkip().has_value());

    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindSetLimit)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    findConfig.setLimit(12);
    EXPECT_TRUE(findConfig.getLimit().has_value());

    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddBatchSize)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    findConfig.setBatchSize(90);
    EXPECT_TRUE(findConfig.getBatchSize().has_value());

    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddSingleBatch)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    findConfig.setSingleBatch(true);
    EXPECT_TRUE(findConfig.getSingleBatch().has_value());

    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindSetComment)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    findConfig.setComment("Comment on Life");
    EXPECT_TRUE(findConfig.getComment().has_value());

    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddMaxTimeMS)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    findConfig.setMaxTimeMS(255);
    EXPECT_TRUE(findConfig.getMaxTimeMS().has_value());

    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddReadConcern)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    findConfig.setReadConcern(ReadConcern{});
    EXPECT_TRUE(findConfig.getReadConcern().has_value());

    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddMax)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    findConfig.setMax(12);
    EXPECT_TRUE(findConfig.getMax().has_value());

    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindSetMin)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    findConfig.setMin(12);
    EXPECT_TRUE(findConfig.getMin().has_value());

    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddReturnKeu)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    findConfig.setReturnKey(true);
    EXPECT_TRUE(findConfig.getReturnKey().has_value());

    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddTailable)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    findConfig.setTailable(true);
    EXPECT_TRUE(findConfig.getTailable().has_value());

    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddAwaitData)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    findConfig.setAwaitData(true);
    EXPECT_TRUE(findConfig.getAwaitData().has_value());

    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddNoCursorTimeout)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    findConfig.setNoCursorTimeout(false);
    EXPECT_TRUE(findConfig.getNoCursorTimeout().has_value());

    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddAllowPartialResults)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    findConfig.setAllowPartialResults(false);
    EXPECT_TRUE(findConfig.getAllowPartialResults().has_value());

    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddCollation)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    findConfig.setCollation({});
    EXPECT_TRUE(findConfig.getCollation().has_value());

    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddDiskUse)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    findConfig.setAllowDiskUse(false);
    EXPECT_TRUE(findConfig.getAllowDiskUse().has_value());

    EXPECT_FALSE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, ConstructFindAddLet)
{
    FindConfig      findConfig;

    EXPECT_FALSE(findConfig.getSort().has_value());
    EXPECT_FALSE(findConfig.getProjection().has_value());
    EXPECT_FALSE(findConfig.getHint().has_value());
    EXPECT_FALSE(findConfig.getSkip().has_value());
    EXPECT_FALSE(findConfig.getLimit().has_value());
    EXPECT_FALSE(findConfig.getBatchSize().has_value());
    EXPECT_FALSE(findConfig.getSingleBatch().has_value());
    EXPECT_FALSE(findConfig.getComment().has_value());
    EXPECT_FALSE(findConfig.getMaxTimeMS().has_value());
    EXPECT_FALSE(findConfig.getReadConcern().has_value());
    EXPECT_FALSE(findConfig.getMax().has_value());
    EXPECT_FALSE(findConfig.getMin().has_value());
    EXPECT_FALSE(findConfig.getReturnKey().has_value());
    EXPECT_FALSE(findConfig.getShowRecordId().has_value());
    EXPECT_FALSE(findConfig.getTailable().has_value());
    EXPECT_FALSE(findConfig.getAwaitData().has_value());
    EXPECT_FALSE(findConfig.getNoCursorTimeout().has_value());
    EXPECT_FALSE(findConfig.getAllowPartialResults().has_value());
    EXPECT_FALSE(findConfig.getCollation().has_value());
    EXPECT_FALSE(findConfig.getAllowDiskUse().has_value());
    findConfig.setLet({{"x", "12"}, {"y", "32"}});
    EXPECT_TRUE(findConfig.getLet().has_value());
}

TEST(ThorsMongFindTest, SerializeFindConfig)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}, find};

    EXPECT_EQ(0x45, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x45\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddSort)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setSort({{"name", SortOrder::Descending}, {"age", SortOrder::Ascending}}), find};

    EXPECT_EQ(0x63, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x63\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "sort\x00"
                        "\x18\x00\x00\x00"
                            "\x10" "name\x00"   "\xFF\xFF\xFF\xFF"
                            "\x10" "age\x00"    "\x01\x00\x00\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddProjection)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setProjection(Projection::create<PeopleWithAddressCode>()), find};

    EXPECT_EQ(0x78, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x78\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "projection\x00"
                        "\x27\x00\x00\x00"
                            "\x10"  "name\x00"      "\x01\x00\x00\x00"
                            "\x03"  "address\x00"
                                "\x0F\x00\x00\x00"
                                    "\x10"  "code\x00"  "\x01\x00\x00\x00"
                                    "\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddHint)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setHint("Hi"), find};

    EXPECT_EQ(0x52, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x52\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x02"  "hint\x00" "\x03\x00\x00\x00"   "Hi\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindSetSkip)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setSkip(2), find};

    EXPECT_EQ(0x4F, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x4F\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "skip\x00"  "\x02\x00\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindSetLimit)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setLimit(2), find};

    EXPECT_EQ(0x50, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x50\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "limit\x00" "\x02\x00\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddBatchSize)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setBatchSize(0), find};

    EXPECT_EQ(0x54, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x54\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "batchSize\x00" "\x00\x00\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddSingleBatch)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setSingleBatch(true), find};

    EXPECT_EQ(0x53, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x53\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "singleBatch\x00" "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindSetComment)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setComment("Comment Zone"), find};

    EXPECT_EQ(0x5F, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x5F\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x02"  "comment\x00" "\x0D\x00\x00\x00" "Comment Zone\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddMaxTimeMS)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setMaxTimeMS(256), find};

    EXPECT_EQ(0x54, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x54\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x10"  "maxTimeMS\x00" "\00\x01\x00\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddReadConcern)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setReadConcern({Level::Snapshot}), find};

    EXPECT_EQ(0x6B, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x6B\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "readConcern\x00"
                        "\x19\x00\x00\x00"
                            "\x02"  "level\x00" "\x09\x00\x00\x00"  "snapshot\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}


TEST(ThorsMongFindTest, SerializeFindAddMax)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setMax(12), find};

    EXPECT_EQ(0x56, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x56\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x07"  "max\x00"s;
                    // Not checking the actual value of the ObjectId
                    // "\x0C\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                    //"\x00"s;

    EXPECT_EQ(expected, output.str().substr(0, 0x56 - 12 - 1));
}

TEST(ThorsMongFindTest, SerializeFindSetMin)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setMin(111), find};

    EXPECT_EQ(0x56, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x56\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x07"  "min\x00"s;
                    // Not checking the actual value of the ObjectId
                    // "\x0C\x00\x00\x00" "\x00\x00\x00\x00\x00" "\x00\x00\x00"
                    //"\x00"s;

    EXPECT_EQ(expected, output.str().substr(0, 0x56 - 12 - 1));
}


TEST(ThorsMongFindTest, SerializeFindAddReturnKey)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setReturnKey(false), find};

    EXPECT_EQ(0x51, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x51\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "returnKey\x00" "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddShowRecordId)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setShowRecordId(true), find};

    EXPECT_EQ(0x54, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x54\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "showRecordId\x00" "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddTailable)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setTailable(true), find};

    EXPECT_EQ(0x50, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x50\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "tailable\x00"  "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddAwaitData)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setAwaitData(true), find};

    EXPECT_EQ(0x51, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x51\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "awaitData\x00" "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddNoCursorTimeout)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setNoCursorTimeout(true), find};

    EXPECT_EQ(0x57, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x57\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "noCursorTimeout\x00" "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddAllowPartialResults)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setAllowPartialResults(true), find};

    EXPECT_EQ(0x5B, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x5B\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "allowPartialResults\x00" "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}


TEST(ThorsMongFindTest, SerializeFindAddCollation)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setCollation({"C", true}), find};

    EXPECT_EQ(0x6F, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x6F\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "collation\x00"
                        "\x1F\x00\x00\x00"
                            "\x02"  "locale\x00"    "\x02\x00\x00\x00"  "C\x00"
                            "\x08"  "caseLevel\x00" "\x01"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

TEST(ThorsMongFindTest, SerializeFindAddDiskUse)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setAllowDiskUse(true), find};

    EXPECT_EQ(0x54, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x54\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x08"  "allowDiskUse\x00" "\x01"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}


TEST(ThorsMongFindTest, SerializeFindAddLet)
{
    using   Search = NameField<Eq<std::string>>;

    std::string             collection("col");
    std::string             db("db");
    Search                  find{"John"};
    Finder<People, Search>  finder{collection, db, FindConfig{}.setLet({{"x", "12"}, {"y", "13"}}), find};

    EXPECT_EQ(0x63, finder.getSizeBson());

    std::stringstream   output;
    finder.writeBson(output, PrinterConfig{});

    using namespace std::string_literals;
    std::string expected = 
                "\x63\x00\x00\x00"
                    "\x02"  "find\x00"  "\x04\x00\x00\x00"  "col\x00"
                    "\x02"  "$db\x00"   "\x03\x00\x00\x00"  "db\x00"
                    "\x03"  "filter\x00"
                        "\x1E\x00\x00\x00"
                            "\x03"  "name\x00"
                                "\x13\x00\x00\x00"
                                    "\x02"  "$eq\x00"   "\x05\x00\x00\x00"  "John\x00"
                                    "\x00"
                            "\x00"
                    "\x03"  "let\x00"
                        "\x19\x00\x00\x00"
                            "\x02"  "x\x00" "\x03\x00\x00\x00"  "12\x00"
                            "\x02"  "y\x00" "\x03\x00\x00\x00"  "13\x00"
                            "\x00"
                    "\x00"s;

    EXPECT_EQ(expected, output.str());
}

