#ifndef THORSANVIL_DB_MONGO_THORSMONGO_H
#define THORSANVIL_DB_MONGO_THORSMONGO_H

#include "ThorsMongoConfig.h"
#include "AuthInfo.h"
#include "AuthClient.h"
#include "ConnectionMongo.h"
#include "MessageHandler.h"

#include "ThorSerialize/MongoUtility.h"

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

class DB;
class Collection;
class ThorsMongo
{
    struct DBInfo
    {
    };
    struct CollectionInfo
    {
    };
    ConnectionMongo     mongoStream;
    MongoMessageHandler  messageHandler;

    friend class DB;
    friend class Collection;
    std::map<std::string, DBInfo>           databases;
    std::map<std::string, CollectionInfo>   collections;

    public:
        ThorsMongo(MongoURL const&                  url,
                   Auth::UserNamePassword const&    authInfo,
                   Compression                      compression = Compression::Snappy,
                   Auth::Client const&              clientInfo = Auth::Client{"", {}}
        );
        ThorsMongo(MongoURL const&                  url,
                   Auth::Certificate const&         authInfo,
                   Compression                      compression= Compression::Snappy,
                   Auth::Client const&              clientInfo = Auth::Client{"", {}}
        );

        DB operator[](std::string&& dbName);

    private:
        ConnectionMongo&      getStream() {return mongoStream;}
};

class DB
{
    friend class Collection;
    ThorsMongo&     mongoServer;
    std::string     name;
    public:
        DB(ThorsMongo& mongoServer, std::string&& name)
            : mongoServer(mongoServer)
            , name(name)
        {}

        std::string const&  getName() const;

        Collection operator[](std::string&& collectionName);
};

class Collection
{
    ThorsMongo&     mongoServer;
    std::string     name;           // Combined Name of DB and collection so it is unique.
    public:
        Collection(DB& db, std::string&& name)
            : mongoServer(db.mongoServer)
            , name(db.name + "::" + name)
        {}

    private:
        std::string_view    dbName()    const {return {name.data(), name.find("::")};}
        std::string_view    colName()   const {return {name.data() + name.find("::") + 2};}

};

inline DB          ThorsMongo::operator[](std::string&& dbName)     {return DB(*this, std::move(dbName));}
inline Collection  DB::operator[](std::string&& collectionName)     {return Collection(*this, std::move(collectionName));}

}

#endif
