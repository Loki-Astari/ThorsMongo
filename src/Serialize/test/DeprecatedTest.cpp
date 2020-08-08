#include "SerializeConfig.h"
#include "IgnoreUneededData.h"
#include "gtest/gtest.h"
#include "JsonThor.h"
#include "YamlThor.h"

TEST(DeprecatedTest, Json)
{
    IgnoreUneededData::Thing obj;

    std::stringstream data;
    data << TA::jsonExport(obj);

    IgnoreUneededData::Thing objver;
    data >> TA::jsonImport(objver);
}
TEST(DeprecatedTest, Yaml)
{
    IgnoreUneededData::Thing obj;

    std::stringstream data;
    data << TA::yamlExport(obj);

    IgnoreUneededData::Thing objver;
    data >> TA::yamlImport(objver);
}
