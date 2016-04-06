
#ifndef THORS_ANVIL_SERIALIZE_JSON_LEXEMES_H
#define THORS_ANVIL_SERIALIZE_JSON_LEXEMES_H

#include "Serialize.h"
#include <string>

namespace ThorsAnvil
{
    namespace Serialize
    {

enum YYtokentype
{
    JSON_STRING     = 258,
    JSON_INTEGER    = 259,
    JSON_FLOAT      = 260,
    JSON_TRUE       = 261,
    JSON_FALSE      = 262,
    JSON_NULL       = 263
};
    }
}

#endif

