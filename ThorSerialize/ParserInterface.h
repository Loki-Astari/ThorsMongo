#ifndef THORSANVIL_SERIALIZER_PARSER_INTERFACE_H
#define THORSANVIL_SERIALIZER_PARSER_INTERFACE_H

#include "SerializeConfig.h"
#include "ThorsSerializerUtilTypes.h"
#include "ParserConfig.h"
#include "StringInput.h"
#include <cstddef>
#include <variant>
#include <string>
#include <string_view>
#include <iostream>
#include <map>
#include <any>
#include <memory>

namespace ThorsAnvil::Serialize
{

class ParserInterface
{
    public:
        ParserConfig const config;

        ParserInterface(std::string_view const& str, ParserConfig  config = ParserConfig{})
            : config(config)
            , input(str)
            , pushBack(ParserToken::Error)
        {}
        ParserInterface(std::istream& stream, ParserConfig  config = ParserConfig{})
            : config(config)
            , input(&stream)
            , pushBack(ParserToken::Error)
        {}
        virtual ~ParserInterface() {}
        virtual FormatType formatType()                 = 0;
                ParserToken     getToken();
                void            pushBackToken(ParserToken token);
        virtual ParserToken     getNextToken()          = 0;
        virtual std::string     getKey()                = 0;

        virtual void    ignoreDataValue()               {}
        virtual void    ignoreDataMap(bool)             {}
        virtual void    ignoreDataArray(bool)           {}

        virtual void    getValue(short int&)             = 0;
        virtual void    getValue(int&)                   = 0;
        virtual void    getValue(long int&)              = 0;
        virtual void    getValue(long long int&)         = 0;

        virtual void    getValue(unsigned short int&)    = 0;
        virtual void    getValue(unsigned int&)          = 0;
        virtual void    getValue(unsigned long int&)     = 0;
        virtual void    getValue(unsigned long long int&)= 0;

        virtual void    getValue(float&)                 = 0;
        virtual void    getValue(double&)                = 0;
        virtual void    getValue(long double&)           = 0;

        virtual void    getValue(bool&)                  = 0;

        virtual void    getValue(std::string&)           = 0;

        virtual bool    isValueNull()                    = 0;

        virtual std::string getRawValue()                = 0;

        void    ignoreValue();

        bool            read(char* dst, std::size_t size)
        {
            struct Read
            {
                char*       dst;
                std::size_t size;
                Read(char* dst, std::size_t size):dst(dst),size(size){}
                bool operator()(std::istream* input)    {return static_cast<bool>(input->read(dst, size));}
                bool operator()(StringInput& input)     {return input.read(dst, size);}
            };
            return std::visit(Read{dst, size}, input);
        }
        bool            readTo(std::string& dst, char delim)
        {
            struct ReadTo
            {
                std::string&    dst;
                char            delim;
                ReadTo(std::string& dst, char delim):dst(dst),delim(delim){}
                bool operator()(std::istream* input)    {return static_cast<bool>(std::getline((*input), dst, delim));}
                bool operator()(StringInput& input)     {return input.readTo(dst, delim);}
            };
            return std::visit(ReadTo(dst, delim), input);
        }
        std::size_t     lastReadCount() const
        {
            struct LastReadCount
            {
                std::size_t operator()(std::istream const* input)    {return input->gcount();}
                std::size_t operator()(StringInput const& input)     {return input.getLastReadCount();}
            };
            return std::visit(LastReadCount{}, input);
        }
        std::streampos  getPos()
        {
            struct GetPos
            {
                std::streampos operator()(std::istream* input)    {return input->tellg();}
                std::streampos operator()(StringInput& input)     {return input.getPos();}
            };
            return std::visit(GetPos{}, input);
        }
        int             get()
        {
            struct Get
            {
                int operator()(std::istream* input)    {return input->get();}
                int operator()(StringInput& input)     {return input.get();}
            };
            return std::visit(Get{}, input);
        }
        int             peek()
        {
            struct Peek
            {
                int operator()(std::istream* input)    {return input->peek();}
                int operator()(StringInput& input)     {return input.peek();}
            };
            return std::visit(Peek{}, input);
        }
        void            ignore(std::size_t size)
        {
            struct Ignore
            {
                std::size_t size;
                Ignore(std::size_t size): size(size) {}
                void operator()(std::istream* input)    {input->ignore(size);}
                void operator()(StringInput& input)     {input.ignore(size);}
            };
            std::visit(Ignore{size}, input);
        }
        void            clear()
        {
            struct Clear
            {
                void operator()(std::istream* input)    {input->clear();}
                void operator()(StringInput& input)     {input.clear();}
            };
            std::visit(Clear{}, input);
        }
        void            unget()
        {
            struct Unget
            {
                void operator()(std::istream* input)    {input->unget();}
                void operator()(StringInput& input)     {input.unget();}
            };
            std::visit(Unget{}, input);
        }
        bool            ok() const
        {
            struct OK
            {
                bool operator()(std::istream const* input)    {return !input->fail();}
                bool operator()(StringInput const& input)     {return input.isOk();}
            };
            return std::visit(OK{}, input);
        }
        void setFail()
        {
            struct SetFail
            {
                void operator()(std::istream* input)    {input->setstate(std::ios::failbit);}
                void operator()(StringInput& input)     {input.setFail();}
            };
            std::visit(SetFail{}, input);
        }
        template<typename T>
        bool readValue(T& value)
        {
            struct ReadValue
            {
                T& value;
                ReadValue(T& value) :value(value) {}
                bool operator()(std::istream* input)    {return static_cast<bool>((*input) >> value);}
                bool operator()(StringInput& input)     {input.readValue(value);return true;}
            };
            return std::visit(ReadValue{value}, input);
        }

        template<typename T>
        void getShared(SharedInfo<T> const& info, std::shared_ptr<T>& object)
        {
            std::intmax_t index = info.sharedPtrName;
            if (info.data.has_value())
            {
                object.reset(info.data.value());
                savedSharedPtr[index] = object;
                return;
            }
            std::shared_ptr<T>  sharedPtr = std::any_cast<std::shared_ptr<T>>(savedSharedPtr[index]);
            object = sharedPtr;
        }

    private:
        using DataInputStream = std::variant<std::istream*, StringInput>;

        DataInputStream input;
        ParserToken     pushBack;
        std::map<std::intmax_t, std::any>     savedSharedPtr;
        void    ignoreTheValue();
        void    ignoreTheMap();
        void    ignoreTheArray();

};


}

#if defined(THORS_SERIALIZER_HEADER_ONLY) && THORS_SERIALIZER_HEADER_ONLY == 1
#include "ParserInterface.source"
#endif

#endif
