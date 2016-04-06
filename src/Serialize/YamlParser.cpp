
#include "SerializeConfig.h"
#ifdef HAVE_YAML
#include "YamlParser.h"

using namespace ThorsAnvil::Serialize;

extern "C"
{
    int thorsanvilYamlStreamReader(void* data, unsigned char* buffer, size_t size, size_t* size_read);
}

int thorsanvilYamlStreamReader(void* data, unsigned char* buffer, size_t size, size_t* size_read)
{
    YamlParser*     owner = reinterpret_cast<YamlParser*>(data);
    bool            result  = false;

    owner->input.read(reinterpret_cast<char*>(buffer), size);
    *size_read      = owner->input.gcount();
    result          = ((*size_read) != size_t(-1));

    return result;
}

YamlParser::YamlParser(std::istream& input)
    : ParserInterface(input)
    , first(true)
    , error(false)
{
    yaml_parser_initialize(&parser);
    yaml_parser_set_input(&parser, thorsanvilYamlStreamReader, this);
}

YamlParser::~YamlParser()
{
    if (!first)
    {
        yaml_event_delete(&event);
    }
    yaml_parser_delete(&parser);
}

ParserInterface::ParserToken YamlParser::getNextToken()
{
    // enum class ParserToken {Error, DocStart, DocEnd, MapStart, MapEnd, ArrayStart, ArrayEnd, Key, Value};
    if (first)
    {
        first = false;
        if (!yaml_parser_parse(&parser, &event) || event.type != YAML_STREAM_START_EVENT)
        {
            return parsingError();
        }
        state.emplace_back(State::Open, 0);
    }

    if (error)
    {
        return ParserToken::Error;
    }
    yaml_event_delete(&event);


    if (!yaml_parser_parse(&parser, &event))
    {
        return parsingError();
    }

    switch(event.type)
    {
        case YAML_STREAM_START_EVENT:
            generateParsingException("ThorsAnvil::Serialize::YamlParser: Start should only happen as first event");
        case YAML_ALIAS_EVENT:
            generateParsingException("ThorsAnvil::Serialize::YamlParser: Alias not supported");
        case YAML_NO_EVENT:
            generateParsingException("ThorsAnvil::Serialize::YamlParser: No Event not supported");

        case YAML_STREAM_END_EVENT:
        {
            return parsingError();
        }

        case YAML_DOCUMENT_START_EVENT:
        {
            generateParsingException([&](){return (state.size() != 1 || state.back().first != State::Open || state.back().second != 0);},
                    "ThorsAnvil::Serialize::YamlParser: Invalid document start event");
            ++state.back().second;
            return ParserToken::DocStart;
        }
        case YAML_DOCUMENT_END_EVENT:
        {
            generateParsingException([&](){return (state.size() != 1 || state.back().first != State::Open);},
                    "ThorsAnvil::Serialize::YamlParser: Invalid document end event");
            return ParserToken::DocEnd;
        }

        case YAML_MAPPING_START_EVENT:
        {
            ++state.back().second;
            generateParsingException([&](){return ((state.back().first == State::Map) && ((state.back().second % 2) == 1));},
                    "ThorsAnvil::Serialize::YamlParser: Map is not a valid Key");
            state.emplace_back(State::Map, 0);
            return ParserToken::MapStart;
        }
        case YAML_MAPPING_END_EVENT:
        {
            generateParsingException([&](){return ((state.back().second % 2) != 0);},
                    "ThorsAnvil::Serialize::YamlParser: Maps must have key value pairs");
            state.pop_back();
            return ParserToken::MapEnd;
        }

        case YAML_SEQUENCE_START_EVENT:
        {
            ++state.back().second;
            generateParsingException([&](){return ((state.back().first == State::Map) && ((state.back().second % 2) == 1));},
                    "ThorsAnvil::Serialize::YamlParser: Array is not a valid Key");
            state.emplace_back(State::Array, 0);
            return ParserToken::ArrayStart;
        }
        case YAML_SEQUENCE_END_EVENT:
        {
            state.pop_back();
            return ParserToken::ArrayEnd;
        }

        case YAML_SCALAR_EVENT:
        {
            ++state.back().second;
            return ((state.back().first == State::Map) && ((state.back().second % 2) == 1))
                        ? ParserToken::Key
                        : ParserToken::Value;
        }
        default:
            break;
    }
    return ParserToken::Error;
}

ParserInterface::ParserToken YamlParser::parsingError()
{
    error = true;
    return ParserToken::Error;
}

void YamlParser::generateParsingException(std::function<bool ()> test, std::string const& msg)
{
    if (test())
    {
        generateParsingException(msg);
    }
}

void YamlParser::generateParsingException(std::string const& msg)
{
    error = true;
    throw std::runtime_error(msg);
}

std::string YamlParser::getString()
{
//int   plain_implicit
//int   quoted_implicit
//yaml_scalar_style_t   style

    char const* buffer  = reinterpret_cast<char const*>(event.data.scalar.value);
    std::size_t length  = event.data.scalar.length;

/*
    char const* tag    = event.data.scalar.tag    ? (char const*)event.data.scalar.tag    : "NULL";
    std::cout << "Tag: " << tag << "\n"
              << "PI:  " << event.data.scalar.plain_implicit << "\n"
              << "QI:  " << event.data.scalar.quoted_implicit << "\n"
              << "ST:  " << event.data.scalar.style << "\n"
              << "LN:  " << length << "\n"
              << "VAL: " << std::string(buffer, buffer + length) << "\n";
*/


    return std::string(buffer, buffer + length);
}

std::string YamlParser::getKey()
{
    return getString();
}

template<typename T>
T YamlParser::scan()
{
    char const* buffer  = reinterpret_cast<char const*>(event.data.scalar.value);
    std::size_t length  = event.data.scalar.length;
    char*       end;

    T   value = scanValue<T>(buffer, &end);
    if (buffer + length != end)
    {
        throw std::runtime_error("ThorsAnvil::Serialize::YamlParser: Not an integer");
    }
    return value;
}


void YamlParser::getValue(short& value)                 {value   = scan<short>();}
void YamlParser::getValue(int& value)                   {value   = scan<int>();}
void YamlParser::getValue(long& value)                  {value   = scan<long>();}
void YamlParser::getValue(long long& value)             {value   = scan<long long>();}

void YamlParser::getValue(unsigned short& value)        {value   = scan<unsigned short>();}
void YamlParser::getValue(unsigned int& value)          {value   = scan<unsigned int>();}
void YamlParser::getValue(unsigned long& value)         {value   = scan<unsigned long>();}
void YamlParser::getValue(unsigned long long& value)    {value   = scan<unsigned long long>();}

void YamlParser::getValue(float& value)                 {value   = scan<float>();}
void YamlParser::getValue(double& value)                {value   = scan<double>();}
void YamlParser::getValue(long double& value)           {value   = scan<long double>();}

void YamlParser::getValue(bool& value)
{
    char const* buffer  = reinterpret_cast<char const*>(event.data.scalar.value);
    std::size_t length  = event.data.scalar.length;

    if (length == 4 && strncmp(buffer, "true", 4) == 0)
    {
        value = true;
    }
    else if (length == 5 && strncmp(buffer, "false", 5) == 0)
    {
        value = false;
    }
    else
    {
        throw std::runtime_error("ThorsAnvil::Serialize::YamlParser: Not a bool");
    }
}

void YamlParser::getValue(std::string& value)
{
    value = getString();
}

#endif

