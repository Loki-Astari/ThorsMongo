#ifndef THORSANVIL_DB_MONGO_THORSMONGO_COMMON_H
#define THORSANVIL_DB_MONGO_THORSMONGO_COMMON_H

#include "ThorsMongoConfig.h"
#include "WriteConcern.h"
#include "ReadConcern.h"
#include "BinaryData.h"
#include "MessageHandler.h"

namespace ThorsAnvil::DB::Mongo
{

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

// The following represent information that is either written to or received from Mongo.
enum class ReadPreference   { Primary, PrimaryPreferred, Secondary, SecondaryPreferred, Nearest};

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

// Maps name t expression.
using Let = std::map<std::string, std::string>;

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

}

ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::WriteError,              index, code, errmsg);
ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::WriteConcern,
                        ThorsAnvil::DB::Mongo::WriteConcernWithProv,    provenance);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::ErrInfo,                 writeConcern);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::WriteConcernError,       code, errmsg, errInfo);
ThorsAnvil_ExpandTrait( ThorsAnvil::DB::Mongo::CmdReplyBase,
                        ThorsAnvil::DB::Mongo::ModifyResult,            n, writeErrors, writeConcernError);
ThorsAnvil_MakeTrait(   ThorsAnvil::DB::Mongo::Collation,               locale, caseLevel, caseFirst, strength, numericOrdering, alternate, maxVariable, backwards);

#endif
