#ifndef THORSANVIL_DB_MONGO_THORSMONGO_COMMON_H
#define THORSANVIL_DB_MONGO_THORSMONGO_COMMON_H

#include "ThorsMongoConfig.h"
#include "WriteConcern.h"
#include "ReadConcern.h"
#include "BinaryData.h"
#include "MessageHandler.h"

namespace ThorsAnvil::DB::Mongo
{

// The following represent information that is either written to or received from Mongo.
enum class ReadPreference   { Primary, PrimaryPreferred, Secondary, SecondaryPreferred, Nearest};

}

#endif
