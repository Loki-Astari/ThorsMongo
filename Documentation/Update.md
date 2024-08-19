![ThorsMongo](../img/thorsmongoapi.jpg)

# Update:

In the API [findAndUpdateOne](FindAndModifyOne.md#findAndUpdateOne) you can specify an update operation.

Like "[Filter](Filter.md) these are special format documents the Mongo Server executes. This is not a specific API but just specially formatted documents that are passed to the Mongo Server that provide the ability to perform update operations on a record.

This library provides a mechanism to easily generate these documents without the developer needing to understand the lowest level details; BUT I feel it is worth explaining this from the bottom up. So I will explain the basics of these documents and how to build them manually from scratch. I will will then go over how to use some templates provided by ThorsMongo to simplify this processes. Finally I will introduce a set of tools that will do all the work automatically.

The Mongo [Update and Operators](https://www.mongodb.com/docs/manual/reference/operator/update/) define some basic operations. Please have a read of the Mongo documents.

## Building from Scratch:

I will use the Equality ([`$set`](https://www.mongodb.com/docs/manual/reference/operator/update/set/) as a basic example, but the following applies to all the basic updates.

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

If we want to "Update" a name to "John" you would need to construct and send the Mongo document.

````
    {
        "$set":
        {
            "name": "Mark"
        }
    }
````

To represent this using ThorsSerializer you would need to declare two classes.

```C++
    struct NameFieldToMark
    {
        std::string     name = "Mark";
    };
    struct SetNameToMark
    {
        NameFieldToMark $set;
    };
    ThorsAnvil_MakeTrait(NameFieldToJohn, name);
    ThorsAnvil_MakeTrait(SetNameToJohn,  $set);
```

Now we could use this with [findAndUpdateOne](FindAndModifyOne.md#findAndUpdateOne) method on [Collection](Collection.md) to update the "John" records to "Mark".

```C++
    // Changes the name of John to Mark
    mongo["People"]["Home"].findAndUpdateOne<Person>(FindNameEqualToJohn{}, SetNameToMark{});
```

### Update the city:

To update the city you need to access the address sub object. In Mongo this would look like:

````
    {
        "$set":
        {
            "address.city": <New City>
        }
    }
````

The language does not support class members with a `"."` in the middle, but ThorsSerialize has a technique to compensate for this `ThorsAnvil_MakeOverride`. Also we did not specify a specific value but want to allow the code to pass in something appropriate; So will also add some constructor to pass the value to the destination.

```C++
    struct CityField
    {
        std::string     address_city;
        CityField(std::string init) : address_city(std::move(init)) {}
    };
    struct SetCityField
    {
        CityField       $set;
        SetCityField(std::string init) : $set(std::move(init))      {}
    };
    ThorsAnvil_MakeOverride(CityField, {"address_city", "address.city"});
    ThorsAnvil_MakeTrait(CityField, address_city);
    ThorsAnvil_MakeTrait(SetCityField, $set);

    // .....

    // Updates the city that John lives in
    mongo["People"]["Home"].findAndUpdateOne<Person>(FindNameEqualToJohn{}, SetCityField{"Cardif"});
```


### Overview:

For simple cases like this its a tiny overhead but doable. But once you start using and/or and combining multiple updates this looks like it could becomes a bit tiresome quickly.

## Built-In Query Operators:

This library defines classes for the basic operators in [MongoUpdate.h](../src/ThorsMongo/MongoUpdate.h). These types are designed to be used with types provided by the engineer to specify a field.

### Find by Name:

Lets repeat the above example, but using the builtin types that represent the query operators.

```C++
    // Building the Field Accesses is exactly the same as in the "Filter" documentation.
    // This is by design so we can user Field accesses for both Filer and Update operations.

    // See: "FieldAccessName" in the "Filter.md" documentation.

    using ThorsAnvil::DB::Mongo::QueryOp::Lt;
    // Find the record with the name "Tom" and update the address.city field to "London"
    mongo["People"]["Home"].findAndUpdateOne<Person>(FieldAccessName<Lt<std::string>>{"Tom"}, SetCityField{"London"});
```

### Find by Country Code:

```C++
    // Building the Field Accesses is exactly the same as in the "Filter" documentation.
    // This is by design so we can user Field accesses for both Filer and Update operations.

    // See: "FieldAccessCountryCode" in the "Filter.md" documentation.

    using ThorsAnvil::DB::Mongo::QueryOp::In;
    using ThorsAnvil::DB::Mongo::QueryOp::Set;
    // Find a person in Country code that is the set 235,236,237 and set it to 250.
    mongo["People"]["Home"].findAndUpdateOne<Person>(FieldAccessCountryCode<In<std::uint64_t>>{{235, 236, 237}}, Set<FieldAccessCountryCode<SetPram<std::uint64_t>>>{250});
```

### Extend to more interesting classes:

```C++
    using ThorsAnvil::DB::Mongo::QueryOp::Eq;
    using ThorsAnvil::DB::Mongo::QueryOp::And;
    using SetCountryCode    = Set<FieldAccessCountryCode<std::uint64_t>>;
    using SetName           = Set<FieldAccessName<std::string>>;
```

If you look carefully the pattern is very similar for both filter and update operations:
```C++
    // FILTER
    using FindByCountryCodeEq   = FindByCountryCode<Eq<std::uint64_t>>;
    using SetName               = Set<FieldAccessName<SetParam<std::string>>>;
                                  ^^^                                           Operation
                                      ^^^^^^^^^^^^^^^                           Field To Accesses
                                                      ^^^^^^^^                  Operation type conversion
                                                               ^^^^^^^^^^^^     Type of the FieldName


    // Notice that on the updates there are two types we use:
    //      1: Set
    //      2: SetParam
    //
    // This pattern is used because the value we pass to Set may not be the same as the
    // the field type so we have an extra class to convert from field type to the type needed
    // by the Update action.
    // This pattern applies to all the Updates types.
    //  E.G.        Inc                 => IncParam
    //              SetCurrentTime      => SetCurrentTimeParam
```

We can take advantage of this commonality in the [Tool generation](#Tool generated filters).

### Overview:

With the ThorsMongo Update types the engineer only needs to define one class per field that needs to be modified. This class can then be specialized with existing update types. This is a better solution then doing everything by hand and much more readable, but still involves some boilerplate.

## Tool generated filters.

The filters described in the last section are so regular that we can define tools that build them for us.

There are two steps:
1. Create a "Field Access" class: [ThorsMongo_CreateFieldAccess](../src/ThorsMongo/MongoUtil.h#L31-L51)   
    This step is identicial to Filters (and can be "Field Access" class can be reused.
2. Delcare a "Update" class: [ThorsMongo_UpdateFromAccess](../src/ThorsMongo/MongoUtil.h#L61-L68)

### Field Access class:
```C++
    // Field accesses class (See: FindByCountryCode) are very regular.
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
    //      ThorsMongo_UpdateFromAccess( <Filter Op>, <Type>, <List of fields the drill donw> )

    // Example of Set check on a persons name
    using SetPeopleName = ThorsMongo_UpdateFromAccess(Set, People, name);

    // Example of Increment (Inc) on the countryCode of a person address field.
    using IncPeopleCC = ThorsMongo_UpdateFromAccess(Inc, People, address, countryCode);
```

### Overview:

The macros `ThorsMongo_CreateFieldAccess()` and `ThorsMongo_FilterFromAccess()` pull type information from the class to make sure that the "Filter Op" is correctly parameterized with the type of the field you speciy.

```C++
    ThorsMongo_CreateFieldAccess(People, address, countryCode);
    using IncPeopleCC = ThorsMongo_UpdateFromAccess(Inc, People, address, countryCode);

    // The long hand version of this looks something like this:
    // Note: Simplified for documentation.

    namespace ThorsAnvil::FieldAccess::People::address::city
    {
        template<typename T>
        struct Access
        {
            using CType = ConstructorType<T>;
            Access(CType init): field(std::move(init)) {}
            T   address.countryCode;
        }
        using AddressType      = decltype(::People::address);
        using countryCodeType  = decltype(AddressType::countryCode);
    };


    using IncPeopleCity = Inc<ThorsAnvil::FieldAccess::People::address::city::Access<IncParam<ThorsAnvil::FieldAccess::People::address::city::countryCode>>>;
```

### Filter Usage:

```C++
    // Update the person with the name Timothy by increment the country code by 12.
    mongo["People"]["Home"].findAndUpdateOne<Person>(FindByPeopleName{"Timothy"}, IncPeopleCC{12});

    // Update the person livin in Aberdeen by setting their name to Scottish
    mongo["People"]["Home"].findAndUpdateOne<Person>(FindByPeopleCity{"Aberdeen"}, SetPeopleName{"Scottish"});
```

