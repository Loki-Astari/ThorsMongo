#ifndef THORSANVIL_DB_MONGO_THORSMONGO_COMMON_H
#define THORSANVIL_DB_MONGO_THORSMONGO_COMMON_H

#include "ThorsMongoConfig.h"
#include "WriteConcern.h"
#include "ReadConcern.h"
#include "BinaryData.h"
#include "MessageHandler.h"

namespace ThorsAnvil::DB::Mongo
{

// Forward Declarations
class DB;               // Database     (See Below)
class Collection;       // Collection   (See Below)

// Testing class.
// Simplifies access to private members.
template<typename T>
class TestFindResult;

template<typename T>
struct HasCType
{
    using One = char;
    struct Two { char x[2]; };


    template <typename C> static One testHasType(typename C::CType*);
    template <typename C> static Two testHasType(...);

    static constexpr bool val = sizeof(testHasType<T>(nullptr)) == sizeof(char);
};

template<typename T, bool = HasCType<T>::val>
struct ConstructorType_T;


template<typename T>
struct ConstructorType_T<T, true>
{
    using CType = typename T::CType;
};

template<typename T>
struct ConstructorType_T<T, false>
{
    using CType = T;
};

template<typename T>
using ConstructorType = typename ConstructorType_T<T>::CType;

// Standard utility class re-names.
template<typename T>
using Cref = std::reference_wrapper<T const>;

template<typename T>
using Nref = std::reference_wrapper<T>;

using TimeStamp     = ThorsAnvil::Serialize::MongoUtility::BsonTimeStamp;

// The following represent information that is either written to or received from Mongo.
enum class ReadPreference   { Primary, PrimaryPreferred, Secondary, SecondaryPreferred, Nearest};
enum class SortOrder        { Ascending = 1, Descending = -1};

struct WriteError
{
    std::uint32_t               index;              // An integer that identifies the document in the documents array, which uses a zero-based index.
    std::uint32_t               code;               // An integer value identifying the error.
    std::string                 errmsg;             // A description of the error.
};

struct WriteConcernWithProv: public WriteConcern
{
    std::string                 provenance;
};

struct ErrInfo
{
    WriteConcernWithProv        writeConcern;       // The write concern object used for the corresponding operation.
                                                    // For information on write concern object fields, see Write Concern Specification.
                                                    // The write concern object may also contain the following field, indicating the source of the write concern:
};

struct WriteConcernError
{
    std::uint32_t               code;               // An integer value identifying the cause of the write concern error.
    std::string                 errmsg;             // A description of the cause of the write concern error.
    ErrInfo                     errInfo;
};

struct Signature                                    // Information about the cluster.
{
    Signature()
        : hash{0}
        , keyId{0}
    {}

    Internal::Binary            hash;               // Binary Blob
    std::uint64_t               keyId;
};

struct ClusterTime                                  // Cluster Information.
{
    TimeStamp                   clusterTime;
    Signature                   signature;
};

// Maps name t expression.
using Let = std::map<std::string, std::string>;

class SerializeSort;
class Sort
{
    friend class SerializeSort;
    using SortInfo = std::pair<std::string, SortOrder>;
    std::vector<SortInfo>       sort;
    public:
        Sort(std::initializer_list<SortInfo> sort)
            : sort(sort)
        {}
};

class Projection
{
    friend class SerializeProjection;
    std::string  rawData;
    private:
        Projection(std::string input)
            : rawData(std::move(input))
        {}
    public:
        template<typename T>
        static Projection create()
        {
            std::stringstream                       stream;
            ThorsAnvil::Serialize::Projection<T>    data;
            stream << ThorsAnvil::Serialize::bsonExporter(data);
            return Projection{stream.str()};
        }

        std::string const& getData() const {return rawData;}
};

// TODO:    - Just to get things rolling.
//            Probably needs some work.
using Hint = std::string;

// TODO:    - Used by Min/Max
//            Probably needs some work.
using Limit = ObjectID;

struct Collation
{
    std::string                     locale;
    std::optional<bool>             caseLevel;
    std::optional<std::string>      caseFirst;
    std::optional<std::uint32_t>    strength;
    std::optional<bool>             numericOrdering;
    std::optional<std::string>      alternate;
    std::optional<std::string>      maxVariable;
    std::optional<bool>             backwards;
};

class Cursor
{
    friend class ThorsAnvil::Serialize::Traits<Cursor>;
    bool                        partialResultsReturned  = false;
    std::uint64_t               id                      = 0;
    std::string                 ns;
    public:
        std::uint64_t           getId() const {return id;}
        friend void swap(Cursor& lhs, Cursor& rhs) noexcept
        {
            using std::swap;
            swap(lhs.partialResultsReturned, rhs.partialResultsReturned);
            swap(lhs.id,                     rhs.id);
            swap(lhs.ns,                     rhs.ns);
        }
};

template<typename T>
class CursorFirst;
template<typename T>
class CursorNext: public Cursor
{
    friend class ThorsAnvil::Serialize::Traits<CursorNext<T>>;
    public:
        std::vector<T>          nextBatch;
        std::vector<T> const&   data()  const {return nextBatch;}
};

template<typename T>
class FindResult;
template<typename T>
class CursorFirst: public Cursor
{
    friend class ThorsAnvil::Serialize::Traits<CursorFirst<T>>;
    friend class FindResult<T>;

    std::vector<T>              firstBatch;
    public:
        std::vector<T> const&   data()  const {return firstBatch;}
        friend void swap(CursorFirst<T>& lhs, CursorNext<T>& rhs) noexcept
        {
            using std::swap;
            swap(static_cast<Cursor&>(lhs), static_cast<Cursor&>(rhs));
            swap(lhs.firstBatch,            rhs.nextBatch);
        }
};

// Configuration of actions.

// So we don't over create Printer/Parser config object
// have a default version that can be used for the majroity of situations.
class DefaultConfig
{
    public:
        static const PrinterConfig      defaultPrinterConfig;
        static const ParserConfig       defaultParserConfig;
};

// All Config inherit from this class.
// This configuration has options that allow us to configure the message sending at the lowest level.
template<typename T>
class ActionConfig
{
    std::optional<PrinterConfig>    printerConfig;
    std::optional<ParserConfig>     parserConfig;
    OP_MsgFlag                      msgFlag         = OP_MsgFlag::empty;

    public:
        T& setPrinterConfig(PrinterConfig const& pc)    {printerConfig.emplace(pc); return *static_cast<T*>(this);}
        T& setParserConfig(ParserConfig const& pc)      {parserConfig.emplace(pc);  return *static_cast<T*>(this);}
        T& setMsgFlag(OP_MsgFlag flag)                  {msgFlag = flag;            return *static_cast<T*>(this);}
        PrinterConfig const& getPrinterConfig() const
        {
            return printerConfig.has_value() ? printerConfig.value() : DefaultConfig::defaultPrinterConfig;
        }
        ParserConfig const& getParserConfig() const
        {
            return parserConfig.has_value() ? parserConfig.value() : DefaultConfig::defaultParserConfig;
        }
        OP_MsgFlag getMsgFlag() const
        {
            return msgFlag;
        }
};

// Result object
struct ModifyResult: public CmdReplyBase
{
    std::uint32_t               n;                  // The number of documents inserted.
    std::vector<WriteError>     writeErrors;        // An array of documents that contains information regarding any error encountered during the insert operation.
                                                    // The writeErrors array contains an error document for each insert that errors.
    WriteConcernError           writeConcernError;  // Document that describe error related to write concern and contains the field:
};

class SerializeSort
{
    public:
        static std::size_t getPrintSizeBson(ThorsAnvil::Serialize::PrinterInterface&, Sort const& object)
        {
            std::size_t size = 4 + 1; // Size + end marker
            for (auto const& value: object.sort)
            {
                size += (1 + std::size(value.first) + 1 + 4);
                     // Line Marker + Name + '\x00' + Number.
            }
            return size;
        }
        static void writeCustom(ThorsAnvil::Serialize::PrinterInterface& printer, Sort const& object)
        {
            printer.openMap(getPrintSizeBson
            (printer, object));
            for (auto const& value: object.sort)
            {
                printer.addKey(value.first);
                printer.addValue(static_cast<std::int32_t>(value.second));
            }
            printer.closeMap();
        }
        static void readCustom(ThorsAnvil::Serialize::ParserInterface& parser, Sort& object);
};

class SerializeProjection: public ThorsAnvil::Serialize::DefaultCustomSerializer<Projection>
{
    public:
        virtual std::size_t getPrintSizeBson(ThorsAnvil::Serialize::BsonPrinter&, Projection const& object) const override {return object.rawData.size();}
        virtual char getBsonByteMark()                                               const override {return '\x03';} // You should also override this see Bson specifications
        virtual void writeBson(ThorsAnvil::Serialize::BsonPrinter& printer, Projection const& object)       const override
        {
            printer.stream().write(object.rawData.c_str(), object.rawData.size());
        }
};

// Find interface.
template<typename T>
class CursorIterator
{
    Nref<FindResult<T>>     findResult;
    bool                    input;

    public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::remove_cv_t<T>;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::input_iterator_tag;
        CursorIterator(FindResult<T>& findResult, bool input)
            : findResult(findResult)
            , input(input)
        {}

        // Input iterator.
        // If they are both the end they are equal.
        // Since this is an input iterator then if they are not the end they are also always equal.
        friend bool operator==(CursorIterator const& lhs, CursorIterator const& rhs)    {return lhs.input == rhs.input;}
        friend bool operator!=(CursorIterator const& lhs, CursorIterator const& rhs)    {return !(lhs == rhs);}

        T& operator*()  {return findResult.get().current();}
        T* operator->() {return &findResult.get().current();}
        CursorIterator& operator++()     /* prefix */
        {
            // increment() returns true if more data is
            // available after getting more data.
            input = findResult.get().increment();
            return *this;
        }
        // Postfix ++ is not supported on input iterators.
        //CursorIterator  operator++(int)  /* postfix */
};

template<typename T, typename I = CursorIterator<T>>
struct Range
{
    std::shared_ptr<FindResult<T>>   findData;
    public:
        Range(std::unique_ptr<FindResult<T>> result)
            : findData(std::move(result))
        {}
        CursorIterator<T> begin()   {return CursorIterator<T>{*findData, true};}
        CursorIterator<T> end()     {return CursorIterator<T>{*findData, false};}

    private:
        friend class TestFindResult<T>;
        FindResult<T>& getResult() const {return *findData;}
};

class All{};

}

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::WriteError,              index, code, errmsg);
ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::WriteConcern,
                        ThorsAnvil::DB::Mongo::WriteConcernWithProv,    provenance);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::ErrInfo,                 writeConcern);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::WriteConcernError,       code, errmsg, errInfo);
ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::ModifyResult,            n, writeErrors, writeConcernError);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Cursor,                  partialResultsReturned, id, ns);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::Cursor,
                        ThorsAnvil::DB::Mongo::CursorFirst,             firstBatch);
ThorsAnvil_Template_ExpandTrait(1,
                        ThorsAnvil::DB::Mongo::Cursor,
                        ThorsAnvil::DB::Mongo::CursorNext,              nextBatch);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Signature,               hash, keyId);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::ClusterTime,             clusterTime, signature);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Collation,               locale, caseLevel, caseFirst, strength, numericOrdering, alternate, maxVariable, backwards);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::DB::Mongo::Sort,        ThorsAnvil::DB::Mongo::SerializeSort);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::DB::Mongo::Projection,  ThorsAnvil::DB::Mongo::SerializeProjection);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::All);

#endif
