#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H
#define THORSANVIL_DB_MONGO_THORSMONGO_H

#include "ThorsMongoConfig.h"
#include "ConnectionMongo.h"
#include "MessageHandler.h"
#include <bit>

static_assert(
    std::endian::little == std::endian::native,
    "Don't want to support big endian unless I can do some good testing. Don't have a system for that so currently not suppoorted, but should be a relatively easy change"
);


namespace ThorsAnvil::DB::Mongo
{

class MongoMessageHandler: public MessageHandler
{
    ConnectionMongo& mongoStream;

    public:
        MongoMessageHandler(ConnectionMongo& mongoStream)
            : mongoStream(mongoStream)
        {}
        virtual std::iostream& getStream() override {return mongoStream;}
};

}

#endif
