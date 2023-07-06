#include "MongoUtility.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <tuple>
#ifndef __WINNT
#include <arpa/inet.h>
#endif

using namespace ThorsAnvil::Serialize;
using namespace ThorsAnvil::Serialize::MongoUtility;

int ObjectID::classCounter = 32769;

ObjectID::ObjectID(std::int32_t timestamp, std::int64_t random, std::int32_t counter)
    : timestamp(timestamp)
    , random(random)
    , counter(counter)
{}

UTCDateTime::UTCDateTime(std::int64_t datetime)
    : datetime(datetime)
{}
BsonTimeStamp::BsonTimeStamp(std::time_t timestamp, int inc)
    : increment(inc)
    , timestamp(timestamp)
{}

UTCDateTime BsonTimeStamp::asDateTime()
{
    return UTCDateTime(timestamp * 1000);
}
