#include "ThorSerialize/Traits.h"
#include "../MongoUtil.h"
#include "../MongoQuery.h"

#if defined(THOR_DISABLE_TEST_WITH_MONGO_QUERY) && (THOR_DISABLE_TEST_WITH_MONGO_QUERY == 1)
#define SKIP_INTEGRATION_TEST()   GTEST_SKIP()
#else
#define SKIP_INTEGRATION_TEST()
#endif

#ifdef  __WINNT__
#include <winsock2.h>
#include <windows.h>
#endif

class SocketSetUp
{
#ifdef  __WINNT__
    public:
        SocketSetUp()
        {
            WSADATA wsaData;
            WORD wVersionRequested = MAKEWORD(2, 2);
            int err = WSAStartup(wVersionRequested, &wsaData);
            if (err != 0) {
                printf("WSAStartup failed with error: %d\n", err);
                throw std::runtime_error("Failed to set up Sockets");
            }
        }
        ~SocketSetUp()
        {
            WSACleanup();
        }
#endif
};


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

template<typename T = std::string>
struct NameField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    NameField(CType init)
        : name(std::move(init))
    {}
    T       name;
};

template<typename T = std::uint32_t>
struct AgeField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    AgeField(CType init = CType{})
        : age(std::move(init))
    {}
    T       age;
};

template<typename T = std::uint32_t>
struct LengthField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    LengthField(CType init)
        : length(std::move(init))
    {}
    T       length;
};

template<typename T>
struct DataField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    DataField(CType init)
        : data(std::move(init))
    {}
    T       data;
};

template<typename T>
struct DateField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    DateField(CType init): dateTime(std::move(init)) {}
    T       dateTime;
};

template<typename T>
struct TimeStampField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    TimeStampField(CType init): timeStamp(std::move(init)) {}
    T       timeStamp;
};

template<typename T>
struct FoodField
{
    using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
    FoodField(T init): food(std::move(init)) {}
    T   food;
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
ThorsAnvil_Template_MakeTrait(1,
                     NameField,             name);
ThorsAnvil_Template_MakeTrait(1,
                     AgeField,              age);
ThorsAnvil_Template_MakeTrait(1,
                     LengthField,           length);
ThorsAnvil_Template_MakeTrait(1,
                     DataField,             data);
ThorsAnvil_Template_MakeTrait(1,
                     FoodField,             food);
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
ThorsAnvil_Template_MakeTrait(1,
                     DateField,             dateTime);
ThorsAnvil_Template_MakeTrait(1,
                     TimeStampField,        timeStamp);


