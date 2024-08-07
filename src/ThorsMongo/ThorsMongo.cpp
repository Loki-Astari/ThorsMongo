#include "ThorsMongo.h"
#include "Authenticate.h"
#include "AuthenticateScramSha.h"

using namespace ThorsAnvil::DB::Mongo;

THORS_MONGO_HEADER_ONLY_INCLUDE
ThorsMongo::ThorsMongo(MongoURL const&                  url,
                       Auth::UserNamePassword const&    authInfo,
                       Compression                      compression,
                       Auth::Client const&              clientInfo)
    : mongoStream(url)
    , messageHandler(mongoStream)
{
    Authenticate    authenticator;
    // TODO
    // authenticator.addAuthenticator("SCRAM-SHA-1", ThorsAnvil::DB::Mongo::Auth::ScramSha1::authenticate);
    authenticator.addAuthenticator("SCRAM-SHA-256", ThorsAnvil::DB::Mongo::Auth::ScramSha256::authenticate);
    authenticator.handShake(messageHandler, authInfo, compression, clientInfo);
}

THORS_MONGO_HEADER_ONLY_INCLUDE
ThorsMongo::ThorsMongo(MongoURL const&                  url,
                       Auth::Certificate const&         /*authInfo*/,
                       Compression                      /*compression*/,
                       Auth::Client const&              /*clientInfo*/)
    : mongoStream(url)
    , messageHandler(mongoStream)
{
    throw std::runtime_error("TODO: Certificate authentication has not been done");
}

THORS_MONGO_HEADER_ONLY_INCLUDE
ThorsMongo::ThorsMongo(MongoURL const&                  /*url*/,
                   TestAuthenticator const&             /*authInfo*/,
                   Compression                          /*compression*/,
                   Auth::Client const&                  /*clientInfo*/)
    : mongoStream({"google.com", 80})
    , messageHandler(mongoStream)
{
    // No authentication done.
    // Just used for testing.
}
