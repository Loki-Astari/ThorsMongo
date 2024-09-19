#ifndef THORSANVIL_SERIALIZE_JSON_PRINTER_H
#define THORSANVIL_SERIALIZE_JSON_PRINTER_H
/*
 * JsonaryPrinter<T>
 *  See documentation in JsonParser.h
 */

#include "SerializeConfig.h"
#include "Serialize.h"
#include <vector>

namespace ThorsAnvil::Serialize
{

using PrintState = std::tuple<int, TraitType, bool>;
class JsonPrinter: public PrinterInterface
{
    std::vector<PrintState> state;
    public:
        JsonPrinter(std::ostream& output, PrinterConfig config = PrinterConfig{});
        virtual FormatType formatType()                     override {return FormatType::Json;}
        virtual void openDoc()                              override;
        virtual void closeDoc()                             override;

        virtual void openMap(std::size_t size)              override;
        virtual void closeMap()                             override;
        virtual void openArray(std::size_t size)            override;
        virtual void closeArray()                           override;

        virtual void addKey(std::string const& key)         override;

        virtual void addValue(short int value)              override;
        virtual void addValue(int value)                    override;
        virtual void addValue(long int value)               override;
        virtual void addValue(long long int value)          override;

        virtual void addValue(unsigned short int value)     override;
        virtual void addValue(unsigned int value)           override;
        virtual void addValue(unsigned long int value)      override;
        virtual void addValue(unsigned long long int value) override;

        virtual void addValue(float value)                  override;
        virtual void addValue(double value)                 override;
        virtual void addValue(long double value)            override;

        virtual void addValue(bool value)                   override;

        virtual void addValue(std::string const& value)     override;
        virtual void addValue(std::string_view const& value)override;

        virtual void addRawValue(std::string const& value)  override;

        virtual void addNull()                              override;

        void addPrefix();
};

}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "JsonPrinter.source"
#endif

#endif
