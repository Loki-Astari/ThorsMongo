#include "ThorSerialize/Traits.h"
#include "../MongoUtil.h"

struct PersonNoId
{
    std::string     name;
    int             age;
};

struct PersonYesId
{
    ThorsAnvil::DB::Mongo::ObjectID        _id;
    std::string     name;
    int             age;
};

struct Funky
{
    using CType = std::string;
    Funky(CType init)
        : mark(std::move(init))
    {}
    std::string mark;
};

struct Splotch
{
    int         length;
    int         width;
    int         color;
};

struct Address
{
    std::string street;
    std::string city;
    int         code = 0;
};

struct People
{
    std::string name;
    int         age = 0;
    Address     address;
    std::optional<std::vector<int>> data;
};
struct PeopleWithTime: public People
{
    ThorsAnvil::Serialize::MongoUtility::UTCDateTime    dateTime;
    ThorsAnvil::Serialize::MongoUtility::BsonTimeStamp  timeStamp;
};

struct PeopleWithRecordID: public People
{
    std::int64_t    $recordId;
};

struct PeopleWithFood: public People
{
    std::vector<std::string>    food;
};

struct AddressCode
{
    int         code;
};

struct PeopleWithAddressCode
{
    std::string name;
    AddressCode address;
};


ThorsAnvil_MakeTrait(PersonNoId,            name, age);
ThorsAnvil_MakeTrait(PersonYesId,           _id, name, age);
ThorsAnvil_MakeTrait(Funky,                 mark);
ThorsAnvil_MakeTrait(Splotch,               length, width, color);
ThorsAnvil_MakeTrait(Address,               street, city, code);
ThorsAnvil_MakeTrait(People,                name, age, address, data);
ThorsAnvil_ExpandTrait(People,
                     PeopleWithRecordID,    $recordId);
ThorsAnvil_ExpandTrait(People,
                     PeopleWithTime,        dateTime, timeStamp);
ThorsAnvil_ExpandTrait(People,
                     PeopleWithFood,        food);
ThorsAnvil_MakeTrait(AddressCode,           code);
ThorsAnvil_MakeTrait(PeopleWithAddressCode, name, address);


