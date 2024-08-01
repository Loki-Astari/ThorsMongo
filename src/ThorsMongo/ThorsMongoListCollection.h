#ifndef THORSANVIL_DB_MONGO_THORSMONGO_LIST_COLLECTION_H
#define THORSANVIL_DB_MONGO_THORSMONGO_LIST_COLLECTION_H

#include "ThorsMongoConfig.h"
#include "ThorsMongoCommon.h"
#include "MongoCursor.h"

namespace ThorsAnvil::DB::Mongo
{

class ListCollectionConfig: public ActionConfig<ListCollectionConfig>
{
    std::optional<bool>             nameOnly;
    std::optional<bool>             authorizedCollections;
    std::optional<std::string>      comment;

    public:
        std::optional<bool>             const& getNameOnly()                const {return nameOnly;}
        std::optional<bool>             const& getAuthorizedCollections()   const {return authorizedCollections;}
        std::optional<std::string>      const& getComment()                 const {return comment;}

        ListCollectionConfig&     setNameOnly(bool value)               {nameOnly.emplace(value);return *this;}
        ListCollectionConfig&     setAuthorizedCollections(bool value)  {authorizedCollections.emplace(value);return *this;}
        ListCollectionConfig&     setComment(std::string value)         {comment.emplace(value);return *this;}
};

struct TimeSeries
{
    std::string                 timeField;
    std::string                 metaField;
    std::string                 granularity;
    std::uint32_t               bucketMaxSpanSeconds;
    std::uint32_t               bucketRoundingSeconds;
};

struct ClusterIndex
{
    ObjectID                    key;
    bool                        unique                      = false;
    std::string                 name;
};

struct ChangeStreamPrePost
{
    bool                        enabled                     = false;
};

struct CollectionOptions
{
    // TODO
    // bool                        capped                      = 0;
    // TimeSeries                  timeseries;
    // std::uint32_t               expireAfterSeconds          = 0;
    // ClusterIndex                clusteredIndex;
    // ChangeStreamPrePost         changeStreamPreAndPostImages;
    // std::uint32_t               size                        = 0;
    // std::uint32_t               max                         = 0;
    // TODO
    //  storageEngine
    //  validator
    //  indexOptionDefaults
    //  pipeline
    // std::string                 viewOn;
    // Collation                   collation;
    // WriteConcern                writeConcern;
};

struct UUID
{
    std::vector<unsigned char>     uuid;
    std::size_t getSize() const             {return uuid.size();}
    void        resize(std::size_t size)    {uuid.resize(size);}
    char const* getBuffer() const           {return reinterpret_cast<char const*>(&uuid[0]);}
    char*       getBuffer()                 {return reinterpret_cast<char*>(&uuid[0]);}
};

class UUIDSerializer: public ThorsAnvil::Serialize::MongoUtility::BinarySerializer<UUID, '\x04'>
{
    public:
        virtual void writeJson(Serialize::JsonPrinter& printer, UUID const& object) const override
        {
            std::stringstream tmp;
            for (auto v: object.uuid)
            {
                tmp << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(v) << "-";
            }
            printer.addValue(tmp.str().substr(0, 47));
        }
};

struct CollectionInfo
{
    bool                        readOnly;   // Seen
    UUID                        uuid;       // Seen
};

struct CollectionIndexKey
{
    std::int64_t                _id;        // Seen
};
struct CollectionIndex
{
    std::int32_t                v;          // Seen
    CollectionIndexKey          key;        // Seen
};

struct ListInfo
{
    std::string                 name;       // Seen
    std::string                 type;       // Seen
    CollectionOptions           options;    // Seen
    CollectionInfo              info;       // Seen
    CollectionIndex             idIndex;    // Seen
};

class ListCollectionResult: public CursorData<ListInfo>, public MongoActionReadInterfaceTrivialImpl<ListCollectionResult>
{
    friend class ThorsAnvil::Serialize::Traits<ListCollectionResult>;
    friend class TestFindResult<ListInfo>;

    static GetMoreConfig buildGetMoreConfig(ListCollectionConfig const& config)
    {
        GetMoreConfig   result;
        std::optional<std::string> const&   comment     = config.getComment();
        if (comment.has_value()) {
            result.setComment(comment.value());
        }

        result.setPrinterConfig(config.getPrinterConfig());
        result.setParserConfig(config.getParserConfig());
        return result;
    }

    static KillCursorConfig buildKillCursorConfig(ListCollectionConfig const& config)
    {
        KillCursorConfig    result;
        result.setPrinterConfig(config.getPrinterConfig());
        result.setParserConfig(config.getParserConfig());
        return result;
    }

    public:

        using ValueType = ListInfo;

        ListCollectionResult(ThorsMongo& owner, std::string_view dbName, ListCollectionConfig const& config)
            : CursorData<ListInfo>(owner, dbName, "", buildGetMoreConfig(config), buildKillCursorConfig(config))
        {}

        friend void swap(ListCollectionResult& lhs, GetMoreResult<ListInfo>& rhs) noexcept
        {
            using std::swap;
            swap(static_cast<CursorData<ListInfo>&>(lhs), rhs);
        }
};

using LCRange = Range<ListCollectionResult>;

}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::DB::Mongo::UUID, ThorsAnvil::DB::Mongo::UUIDSerializer);

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::TimeSeries,                      timeField, metaField, granularity,
                                                                                bucketMaxSpanSeconds, bucketRoundingSeconds);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::ClusterIndex,                    key, unique, name);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::ChangeStreamPrePost,             enabled);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::CollectionOptions /* TODO */);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::CollectionInfo,                  readOnly, uuid);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::CollectionIndexKey,              _id);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::CollectionIndex,                 v, key);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::ListInfo,                        name, type, options, info, idIndex);
ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::CursorData<ThorsAnvil::DB::Mongo::ListInfo>,
                        ThorsAnvil::DB::Mongo::ListCollectionResult);


#endif
