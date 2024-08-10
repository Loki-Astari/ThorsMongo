![ThorsMongo](../img/thorsmongo.jpg)

# Filter:

In the API there are several places that you can specify a "Filter".  

There are special documents in Mongo that allow you to perform filtering of the records. This is not a specific API but just specially formatted documents that are passed to the Mongo Server that provide the ability to perform boolean based operations to filter out records.

This library provides a mechanism to easily generate these documents without the developer needing to understand the lowest level details; BUT I feel it is worth explaining this from the bottom up. So I will explain the basics of these documents and how to build them manually from scratch. I will will then go over how to use some templates provided by ThorsMongo to simplify this processes. Finally I will introduce a set of tools that will do all the work automatically.

The Mongo [Query and Projection Operators](https://www.mongodb.com/docs/manual/reference/operator/query/) define some basic operations. Please have a read of the Mongo documents.

## Building from Scratch:

I will use the Equality ([`$eq`](https://www.mongodb.com/docs/manual/reference/operator/query/eq/#mongodb-query-op.-eq) as a basic example, but the following applies to all the basic operations (eq `==`, ne `!=`, gt `>`, gte`>=`, lt `<`, lte`<=`, in `member of`, nin `not a member of`).

If we store a `Person` in a collection.

```C++
    class Address
    {
        // To give the serializer accesses to private data.
        friend class ThorsAnvil::Serialize::Traits<Address>;

        std::string                 street1;
        std::string                 street2;
        std::string                 city;
        std::uint64_t               countryCode;
        std::string                 postCode;

        public:
            // Some stuff (including appropriate constructors)
    };
    class Person
    {
        // To give the serializer accesses to private data.
        friend class ThorsAnvil::Serialize::Traits<Person>;
        std::string                 name;
        std::uint32_t               age;
        std::vector<std::string>    alergies;
        Address                     address;
        public:
            // Some stuff (including appropriate constructors)
    };
    ThorsAnvil_MakeTrait(Address, street1, street2, city, country, postCode);
    ThorsAnvil_MakeTrait(Person, name, age, alergies, address);
```

### Find by Name:

If we want to "Filter" on a name equaling a "John" you would need to construct and send the Mongo document.

````
    {
        "name":
        {
            "$eq": "John"
        }
    }
````

To represent this using ThorsSerializer you would need to declare two classes.

```C++
    struct EqualToJohn
    {
        std::string     $eq = "John";
    };
    struct FindNameEqualToJohn
    {
        EqualToJohn name;
    };
    ThorsAnvil_MakeTrait(EqualToJohn, $eq);
    ThorsAnvil_MakeTrait(FindNameEqualToJohn,  name);
```

Now we could use this with [`remove()`](Delete.md) method on [Collection](Collection.md) to remove all the records having the name "John".

```C++
    // See the main README.md on how to create the mongo object.
    using ThorsAnvil::DB::Mongo::Query;

    // removes all the people with name John.
    mongo["People"]["Home"].remove(Query<FindNameEqualToJohn>{});
```

### Find by Country:

To find by country you need to access the address sub object. In Mongo this would look like:

````
    {
        "address.countryCode":
        {
            "$eq":  <Country Code>
        }
    }
````

The language does not support class members with a `"."` in the middle, but ThorsSerialize has a technique to compensate for this `ThorsAnvil_MakeOverride`. Also we did not specify a specific value but want to allow the code to pass in something appropriate; So will also add some constructor to pass the value to the destination.

```C++
    struct EqualCountryCode
    {
        std::uint64_t     $eq;
        EqualCountryCode(std::uint64_t init) : $eq(init) {}
    };
    struct FindByCountryCodeEqual
    {
        EqualString     address_countryCode;
        FindByCountryCodeEqual(std::uint64_t init) : addressCountry(init)   {}
    };
    ThorsAnvil_MakeTrait(EqualCountryCode, $eq);
    ThorsAnvil_MakeOverride(FindByCountryCodeEqual, {"address_countryCode", "address.countryCode"});
    ThorsAnvil_MakeTrait(FindByCountryCodeEqual, addressCountry);

    // .....

    // removes all the people with address.countryCode = 235
    mongo["People"]["Home"].remove(Query<FindByCountryCode>{235});
```


### Overview:

For simple cases like this its a tiny overhead but doable. But once you start using and/or and combining multiple conditions this looks like it could becomes a bit tiresome quickly. Mainly because you would need to define a class for every type of comparison you needed on a field (e.g. FindByCountryCodeEqual, FindByCountryCodeLess, FindByCountryCodeIn etc).

## BuiltIn Query Operators:

This library defines classes for the basic operators in [MongoQuery.h](../src/ThorsMongo/MongoQuery.h). These types are designed to be used with types provided by the engineer to specify a field.

### Find by Name:

Lets repeat the above example, but using the builtin types that represent the query operators.

```C++
    // So the engineer still needs to specify a class that can defines the field being searched for.
    // T is the operation will perform (Eq/Lt/Gte etc)
    template<typename T>
    struct FieldAccessName
    {
        // Boilerplate to get constructor input type needed by T.
        using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
        FieldAccessName(CType init)  : name(std::move(init)) {}

        // The field we will be searching for
        T           name;
    };
    ThorsAnvil_Template_MakeTrait(1, FieldAccessName, name);

    using ThorsAnvil::DB::Mongo::QueryOp::Lt;
    // Remove all people with a name Lt than "Tom"
    mongo["People"]["Home"].remove(Query<FieldAccessName<Lt<std::string>>{"Tom"});
```

### Find by Country Code:

```C++
    template<typename T>
    struct FieldAccessCountryCode
    {
        // Boilerplate to get constructor input type needed by T.
        using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
        FieldAccessCountryCode(CType init)  : addressCountry(std::move(init)) {}

        // The field we will be searching for
        T           addressCountry;
    };
    ThorsAnvil_Template_MakeOverride(1, FieldAccessCountryCode, {"addressCountry", "address.countryCode"});
    ThorsAnvil_Template_MakeTrait(1, FieldAccessCountryCode, addressCountry);

    using ThorsAnvil::DB::Mongo::QueryOp::In;
    // Find by Country code that is the set 235,236,237
    mongo["People"]["Home"].remove(Query<FieldAccessCountryCode<In<std::uint64_t>>{{235, 236, 237}});
```

### Extend to more interesting classes:

```C++
    using ThorsAnvil::DB::Mongo::QueryOp::Eq;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using FindByCountryCodeEq   = FieldAccessCountryCode<Eq<std::uint64_t>>;
    using FindByNameEq          = FieldAccessName<Eq<std::string>>;
    using FindByCountryAndName  = And<FindByCountryCodeEq, FindByNameEq>

    // Removes all people in countryCode 345 with the name Branden
    mongo["People"]["Home"].remove(Query<FindByCountryAndName>{345, "Branden"});
```

If you look carefully the pattern is very similar for most filter operations:
```C++
    using FindByCountryCodeEq   = FieldAccessCountryCode<Eq<std::uint64_t>>;
                                  ^^^^^^^^^^^^^^^^                              FieldAccess
                                                         ^^                     Operation
                                                            ^^^^^^^^^^^^        Type of the CountryCode in Address class
```

We can take advantage of this commonality in the [Tool generation](#Tool generated filters).

### Overview:

With the ThorsMongo query operator types the engineer only needs to define one class per field that needs to be filtered on. This class can then be specialized with existing query types. This is a better solution then doing everything by hand and much more readable, but still involves some boilerplate.

## Tool generated filters.

The filters described in the last section are so regular that we can define tools that build them for us.

There are two steps:
1. Create a "Field Access" class: [ThorsMongo_CreateFieldAccess](../src/ThorsMongo/MongoUtil.h#L31-L51)
2. Delcare a "Filter" class: [ThorsMongo_FilterFromAccess](../src/ThorsMongo/MongoUtil.h#L54-L59)

### Field Access class:
```C++
    // Field accesses class (See: FieldAccessCountryCode) are very regular.
    // So to simplify their creation and remove the manual boilerplate
    // the ThorsMongo_CreateFieldAccess() can be used to create the class.
    //
    // The format is:
    //      ThorsMongo_CreateFieldAccess(<Type>, <List of fields that drill down>)
    //
    // This macro creates a unique namespace under: ThorsAnvil::FieldAccess::
    // So there is no pollution of the global namespace and no possibility of name
    // clashes. But this does means ThorsMongo_CreateFieldAccess() must be used
    // from within the global scope.

    // Example to access People.name
    ThorsMongo_CreateFieldAccess(People, name);

    // Example to access People.address.city (Note city is part of the Address class)
    ThorsMongo_CreateFieldAccess(People, address, city);
```


### Filter class:

```C++
    // Once You have created the field access object you can declare local filters that use them
    // with the ThorsMongo_FilterFromAccess
    //
    // The format is:
    //      ThorsMongo_FilterFromAccess( <Filter Op>, <Type>, <List of fields the drill donw> )

    // Example of Eqality (Eq) check on a persons name
    using FindByPeopleName = ThorsMongo_FilterFromAccess(Eq, People, name);

    // Example of Equality (Eq) check for a city in a person address field.
    using FindByPeopleCity = ThorsMongo_FilterFromAccess(Eq, People, address, city);
```

### Overview:

The macros `ThorsMongo_CreateFieldAccess()` and `ThorsMongo_FilterFromAccess()` pull type information from the class to make sure that the "Filter Op" is correctly parameterized with the type of the field you speciy.

```C++
    ThorsMongo_CreateFieldAccess(People, address, city);
    using FindByPeopleCity = ThorsMongo_FilterFromAccess(Eq, People, address, city);

    // The long hand version of this looks something like this:
    // Note: Simplified for documentation.

    namespace ThorsAnvil::FieldAccess::People::address::city
    {
        template<typename T>
        struct Access
        {
            using CType = ConstructorType<T>;
            Access(CType init): field(std::move(init)) {}
            T   address.city;
        }
        using AddressType      = decltype(::People::address);
        using CityType         = decltype(AddressType::city);
    };


    using FindByPeopleCity = ThorsAnvil::FieldAccess::People::address::city::Access<Eq<ThorsAnvil::FieldAccess::People::address::city::CityType>>;
```

### Filter Usage:

```C++
    // Remove all the people with name timothy.
    mongo["People"]["Home"].remove(Query<FindByPeopleName>{"Timothy"});

    // Remove all the popel from Aberdeen.
    mongo["People"]["Home"].remove(Query<FindByPeopleCity>{"Aberdeen"});
```

