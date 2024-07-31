#include "ConnectionMongo.h"

using namespace ThorsAnvil::DB::Mongo;

ConnectionMongo::ConnectionMongo(MongoURL const& url)
    : ThorsSocket::SocketStream<ConnectionBufferMongo>({url.hostname, url.port})
{}
