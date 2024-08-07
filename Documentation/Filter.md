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
        EqualString     addressCountry;
        FindByCountryEqual(std::uint64_t init) : addressCountry(init)   {}
    };
    ThorsAnvil_MakeTrait(EqualCountryCode, $eq);
    ThorsAnvil_MakeOverride(FindByCountryCodeEqual, {"addressCountry", "address.countryCode"});
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
    struct FindByName
    {
        // Boilerplate to get constructor input type needed by T.
        using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
        FindByName(CType init)  : name(std::move(init)) {}

        // The field we will be searching for
        T           name;
    };
    ThorsAnvil_Template_MakeTrait(1, FindByName, name);

    using ThorsAnvil::DB::Mongo::QueryOp::Lt;
    // Remove all people with a name Lt than "Tom"
    mongo["People"]["Home"].remove(Query<FindByName<Lt<std::string>>{"Tom"});
```

### Find by Country Code:

```C++
    template<typename T>
    struct FindByCountryCode
    {
        // Boilerplate to get constructor input type needed by T.
        using CType = ThorsAnvil::DB::Mongo::ConstructorType<T>;
        FindByCountryCode(CType init)  : addressCountry(std::move(init)) {}

        // The field we will be searching for
        T           addressCountry;
    };
    ThorsAnvil_Template_MakeOverride(1, FindByCountryCode, {"addressCountry", "address.countryCode"});
    ThorsAnvil_Template_MakeTrait(1, FindByCountryCode, addressCountry);

    using ThorsAnvil::DB::Mongo::QueryOp::In;
    // Find by Country code that is the set 235,236,237
    mongo["People"]["Home"].remove(Query<FindByCountryCode<In<std::uint64_t>>{{235, 236, 237}});
```

### Extend to more interesting classes:

```C++
    using ThorsAnvil::DB::Mongo::QueryOp::Eq;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using FindByCountryCodeEq   = FindByCountryCode<Eq<std::uint64_t>>;
    using FindByNameEq          = FindByName<Eq<std::string>>;
    using FindByCountryAndName  = And<FindByCountryCodeEq, FindByNameEq>

    // Removes all people in countryCode 345 with the name Branden
    mongo["People"]["Home"].remove(Query<FindByCountryAndName>{345, "Branden"});
```

### Overview:

With the ThorsMongo query operator types the engineer only needs to define one class per field that needs to be filtered on. This class can then be specialized with existing query types. This is a better solution then doing everything by hand and much more readable, but still involves some boilerplate.

## Tool generated filters.

The filters described in the last section are so regular that we can define tools that build them for us.
See: [ThorsMongo_CreateFilter](https://github.com/Loki-Astari/ThorsMongo/blob/d272311db1c83b7e99eec554d0cf15f8700cfdd6/src/ThorsMongo/MongoQuery.h#L8)

```C++
    // Defines the type "FindByPeopleName"
    // This can be used to search for "Person.name"
    // Note the "Person" is important as it used
    // to get the type of "name"
    ThorsMongo_CreateFilter(FindByPeopleName, Eq, People, name);

    mongo["People"]["Home"].remove(Query<FindByPeopleName>{"Timothy"});
```

The `ThorsMongo_CreateFilter()` supports multi level.

```C++
    ThorsMongo_CreateFilter(PeopleAdressCode, Lt, People, address, code);

    mongo["People"]["Home"].remove(Query<PeopleAdressCode>{122});
```
