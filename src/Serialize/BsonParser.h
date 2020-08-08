#ifndef THORS_ANVIL_SERIALIZE_BSON_PARSER_H
#define THORS_ANVIL_SERIALIZE_BSON_PARSER_H
/*
 * BsonParser<T>
 *      This is used in conjunction with BsonPrinter<T>
 *
 *      Together these provide an implementation of:
 *          the ParserInterface for type T
 *          and PrinterInterface for type T
 *
 *      These Interfaces are used by Serializer and DeSerializer (see Serialize.h)
 *
 *      It uses ThorsAnvil::Serialize::Traits<T> to know what objects to pull from the stream.
 *      For arrays order is important.
 *      For Objects the order of elements is not important. It looks up the key in the Traits<T>
 *      information to understand which member is being de-serialized but unspecified elements
 *      will not cause an error.
 */

#include "Serialize.h"
#include "BsonManualLexer.h"
#include <istream>
#include <string>
#include <vector>

namespace ThorsAnvil
{
    namespace Serialize
    {

class BsonParser: public ParserInterface
{
    enum State          {Error, Init, OpenM, Key, Colon, ValueM, CommaM, CloseM, OpenA, ValueA, CommaA, CloseA, ValueD, Done};

    BsonManualLexer     lexer;
    std::vector<State>  parrentState;
    State               currentEnd;
    State               currentState;
    bool                started;

    std::string getString();
    std::string getRawString();

    template<typename T>
    T scan();
    public:
        BsonParser(std::istream& stream, ParserConfig config = ParserConfig{});
        virtual ParserToken getNextToken()                      override;
        virtual std::string getKey()                            override;

        virtual void    ignoreDataValue()                       override;

        virtual void    getValue(short int& value)              override;
        virtual void    getValue(int& value)                    override;
        virtual void    getValue(long int& value)               override;
        virtual void    getValue(long long int& value)          override;

        virtual void    getValue(unsigned short int& value)     override;
        virtual void    getValue(unsigned int& value)           override;
        virtual void    getValue(unsigned long int& value)      override;
        virtual void    getValue(unsigned long long int& value) override;

        virtual void    getValue(float& value)                  override;
        virtual void    getValue(double& value)                 override;
        virtual void    getValue(long double& value)            override;

        virtual void    getValue(bool& value)                   override;

        virtual void    getValue(std::string& value)            override;

        virtual bool    isValueNull()                           override;

        virtual std::string getRawValue()                       override;
};
    }
}

#if defined(HEADER_ONLY) && HEADER_ONLY == 1
#include "BsonParser.source"
#endif

#endif