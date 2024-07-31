#include "MessageHandler.h"

struct TestMessageHandler: public ThorsAnvil::DB::Mongo::MessageHandler
{
    std::stringstream stream;
    virtual std::iostream& getStream() override {return stream;}
};


