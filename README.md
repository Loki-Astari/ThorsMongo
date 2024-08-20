[![Brew package](https://img.shields.io/badge/Brew-package-blueviolet)](https://formulae.brew.sh/formula/thors-mongo)

![ThorsMongo](img/thorsmongoapi.jpg)

# ThorsMongo:
Mongo API for C++

# TOC:
* [Description](#description)  
* [Installation](#installation)  
  * [Easy Brew](#easy-brew)  
  * [Medium Build](#medium-build)  
  * [Header Only](#header-only)  
* [Usage](#usage)  
  * [Connecting](#connecting)  
  * [Serializing Data](#serializing-data)  
  * [Inserting Data](#inserting-data)  
  * [Find Data](#find-data)  
  * [Delete Data](#delete-data)  
  * [Find And Modify One](#find-and-modify-one)  
    * [Find and Replace](#find-and-replace)  
    * [Find and Remove](#find-and-remove)  
    * [Find and Update](#find-and-update)  
* [Documentation](#documentation)  

# Description:
This is an attempt to have a simple to use Type-Safe API to connect and manipulate data on a MongoDB Server.  

This API does not currently implement the full MongoDB API, but rather the subset of the API around data manipulation and storage.  

This library makes heavy use of ThorSerialize API to automate the generation of the BSON documents that are exchanged between the application and the MongoDB Server. The advantage of this library is that no code needs to be written to create or generate BSON objects (this is all automated).  

# Installation:

## Easy Brew:

These libraries are available via [brew](https://brew.sh/)

```Shell
    > brew install thors-mongo
```

## Medium Build:

Build and install the libraries manually:

```Shell
    > git clone git@github.com:Loki-Astari/ThorsMongo.git
    > cd ThorsMongo
    > ./configure
    > make
    > make install
```

Note: The configuration script should check for all dependencies and give instructions on how to fix any issues.


## Header Only:

There is a header only version. You will need to install it and a couple of other libraries manually.

```Shell
    > git clone --single-branch --branch header-only git@github.com:Loki-Astari/ThorsMongo.git
    > git clone git@github.com:Neargye/magic_enum
    > # or brew install magic_enum
    > brew install snappy
```

# Usage:

Look at the [Example](./src/Example) folder for example of how to build against this project. This project also includes all the code on this page.


## Connecting:
All communication done with the MongoDB Server id done via the `ThorsMongo` class.  

```C++
    #include "ThorsMongo/ThorsMongo.h"

    // Connecting to MongoDB

    using ThorsAnvil::DB::Mongo::ThorsMongo;
    ThorsMongo          mongo({"localhost", 27017}, {"usernmae", "password", "DB"});
```


Note: I have currently only implemented SCRAM-SHA-256 authentication mechanism. So the user must have this enabled.  

```Shell
   // Mongo Shell
   DB> show users
    [
      {
        _id: 'DB.username',
        userId: UUID('eb180bc7-6347-4cf9-8f5c-9aaf7236b3a9'),
        user: 'username',
        db: 'DB',
        roles: [
          { role: 'dbAdmin', db: 'DB' }
        ],
        mechanisms: [ 'SCRAM-SHA-256' ]
                    ^^^^^^^^^^^^^^^^^^ Please check your user has this enabled.
      }
    ]
```

On a MongoDB Server there are multiple Databases. Each Database can have multiple Collections. There are appropriate class for these types of object.  

* `ThorsAnvil::DB::Mongo::DB`
* `ThorsAnvil::DB::Mongo::Collection`

```C++
    using ThorsAnvil::DB::Mongo::DB;
    using ThorsAnvil::DB::Mogno::Collection;

    DB          db(mongo, "DB");
    Collection  collection(db, "People");

    // For convenience the [] operator have been overloaded to give you accesses to these objects.
    // without the need to create local objects.
    mongo["DB"].method();               // Has same affect as: db.method();
    mongo["DB"]["People"].method();     // Has same effect as: db["People"].method()
                                        //                and: collection.method();
```

## Serializing Data:

You can insert any data you like into a collection.  
To serialize the data you must declare the class you are sending to MongoDB as serializable (using [ThorsSerialize](https://github.com/Loki-Astari/ThorsSerializer/) ). This has a tiny bit of boilerplate declaration per class. I provide a simple example here. More detailed examples and documentation can be found [here](https://github.com/Loki-Astari/ThorsSerializer/blob/master/README.md).  

```C++
    // The structure I want to store.
    // C++ class declarations.
    class Address
    {
        // To give the serializer accesses to private data.
        friend class ThorsAnvil::Serialize::Traits<Address>;

        std::string                 street1;
        std::string                 street2;
        std::string                 city;
        std::string                 country;
        std::string                 postCode;

        public:
            Address(std::string const& street1, std::string const& street2, std::string const& city, std::string const& country, std::string const& postCode)
                : street1(street1)
                , street2(street2)
                , city(city)
                , country(country)
                , postCode(postCode)
            {}
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
            Person(std::string const& name, std::uint32_t age, Address const& address)
                : name(name)
                , age(age)
                , address(address)
            {}
    };

    // Declare the types serializable using ThorsSerialize
    // All POD types can be serialized.
    // All types in the standard library can be serialized.
    // A declaration is need for each custom type you want to serialize.

    ThorsAnvil_MakeTrait(Address, street1, street2, city, country, postCode);
    ThorsAnvil_MakeTrait(Person, name, age, alergies, address);
```

## Inserting Data:

To insert data into a collection call the `insert()` method passing either a vector or a tuple of objects. Note: The types of all the objects have to be serializable.  

```C++
    void addPeopleToMongo(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::vector<Person> const& people)
    {
        // The insert() method takes either
        // 1: a vector of any object that can be serialized.
        // 2: a tuple of objects that can be serialized.

        auto result = mongo["DB"]["Collection"].insert(people);

        if (result)
        {
            // Data was inserted successfully
            std::cout << "Inserted ID:\n";
            for (auto const& id: result.inserted)
            {
                std::cout << "\t" << id << "\n";
            }
        }
        else
        {
            std::cout << "Error: " << result << "\n";
        }
    }
```

If you only want to insert a single item you can invisibly create a vector by wrapping the object in curly braces.        

```C++
        auto result = mongo["DB"]["Collection"].insert({ aSinglePersonObject });
```


## Find Data:

To find data you must specify a "[Filter](Documentation/Filter.md)".  
This requires some boilerplate to allow you to match against specific fields in your data. I will provide a simple examples here. Details will be provided below in the Query documentation.  

The `find()` method takes a filter and returns a C++ range.  


```C++
    // This macro creates a type called "FindEqName"
    // It can be used to filter records by Person.name on the server using "Eq" (Equality)

    ThorsMongo_CreateFieldAccess(Person, name);
    using FindEqName = ThorsMongo_FilterFromAccess(Eq, Person, age);

    void findPeopleInMongoByName(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name)
    {
        // Returns a C++ range of all the objects that match the query.
        // Note:   The range hides a MongoDB cursor so as you you iterate across the query
        //         this may result in more calls to MongoDB to retrieve more data automatically.

        auto range = mongo["DB"]["Collection"].find<Person>(FindEqName{name});

        if (range)
        {
            // Print all the returned people
            for (auto const& person: range) {
                std::cout << ThorsAnvil::Serialize::jsonExporter(person);
            }
        }
        else
        {
            std::cout << "Error: " << range << "\n";
        }
    }
```

## Delete Data:

The remove method operates on a `Query`. This is basically a "[Filter](Documentation/Filter.md)" with an extra parameter indicating if the remove should apply to the first or all matches. The example below shows remove using a single `Query` object, but this method can take any number of `Query` objects that are all applied in parallel.  
Note: If you are only deleting one and the filter matches multiple records in the collection then you are effectively deleting a random matching record. Please read the full documentation to understand how to control the filter to get an exact match or use findAndRemoveOne() method for more control.  


```C++
    // Create a filter on Person.age using the "Gt" (Greater than) operator
    ThorsMongo_CreateFieldAccess(Person, age);
    using FindGtAge = ThorsMongo_FilterFromAccess(Gt, Person, age);

    void removePeopelFromMongoByName(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::uint32_t minAge, bool removeOne)
    {
        using ThorsAnvil::DB::Mongo::Query;
        using ThorsAnvil::DB::Mongo::Remove;
        auto result = mongo["test"]["People"].remove(Query<FindGtAge>{minAge, removeOne ? Remove::One : Remove::All});
        if (result)
        {
            std::cerr << "Removed: " << result.n << "\n";
        }
        else
        {
            std::cerr << "Error: " << result << "\n";
        }
    };
```

## Find And Modify One:

There is a family of functions to find and modify a single record.  

* findAndReplaceOne();
* findAndRemoveOne();
* findAndUpdateOne();

All these functions use a "[Filter](Documentation/Filter.md)" to select a single record. If your filter matches multiple records you can add a sort order (the first item in the sorted results is modified. See full documentation for details).  

### Find and Replace:

The `findAndReplaceOne()` methods find and update a single record using a "[Filter](Documentation/Filter.md)" and replaces the record with a new record. This is useful if all the updates are done in the application side.  

```C++
    void replacePerson(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name, Person const& p)
    {
        auto result = mongo["test"]["People"].findAndReplaceOne(FindEqName{name}, p);
        if (result)
        {
            if (!result.value)
            {
                std::cerr << "No original value\n";
            }
            else
            {
                std::cerr << "Orginal Value: " << ThorsAnvil::Serialize::jsonExporter(*result.value) << "\n";
            }
        }
        else
        {
            std::cerr << "Error: " << result << "\n";
        }
    }
```

### Find and Remove:

The `findAndRemoveOne()` methods find and removes a single record using a "[Filter](Documentation/Filter.md)".

```C++
    void removePerson(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name)
    {
        auto result = mongo["test"]["People"].findAndRemoveOne<Person>(FindEqName{name});
        if (result)
        {
            if (!result.value)
            {
                std::cerr << "No original value\n";
            }
            else
            {
                std::cerr << "Orginal Value: " << ThorsAnvil::Serialize::jsonExporter(*result.value) << "\n";
            }
        }
        else
        {
            std::cerr << "Error: " << result << "\n";
        }
    }
```

### Find and Update:

The `findAndUpdateOne()` methods find a single record using a "[Filter](Documentation/Filter.md)" and then applies a custom update operation on the server side. This can simplify the client code and allow updates to parts of the document on the server without having to download the data onto the client.

The update is expressed as an "[Expression](Documentation/Update.md)" that requires some boilerplate. I will provide a simple example here. Details will be provided below in the Update documentation.  

```C++
    // This macro creates a type called "SetAge"
    // It will "Set" the value of Person.age on the server.

    ThorsMongo_CreateFieldAccess(Person, age);
    using SetAge = ThorsMongo_UpdateFromAccess(Set, Person, age);

    void updateAddressCity(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name, std::uint32_t newAge)
    {
        using ThorsAnvil::DB::Mongo::Query;
        using ThorsAnvil::DB::Mongo::Remove;
        auto result = mongo["test"]["People"].findAndUpdateOne<Person>(FindEqName{name}, SetAge{newAge});
        if (result)
        {
            if (!result.value)
            {
                std::cerr << "No original value\n";
            }
            else
            {
                std::cerr << "Orginal Value: " << ThorsAnvil::Serialize::jsonExporter(*result.value) << "\n";
            }
        }
        else
        {
            std::cerr << "Error: " << result << "\n";
        }
    };
```

## Documentation:

* [Serializing C++ classes](https://github.com/Loki-Astari/ThorsSerializer/blob/master/README.md)
* [`ThorsMongo`: Connection to the Mongo Server](Documentation/ThorsMongo.md)
* [`DB`: The DB object](Documentation/DB.md)
* [`Collection`: The Collection object](Documentation/Collection.md)
  * [Inserting Data](Documentation/Insert.md)
  * [Find Data](Documentation/Find.md)
  * [Delete Data](Documentation/Delete.md)
  * [Find And Modify One](Documentation/FindAndModifyOne.md)
  * [Distinct](Documentation/Distinct.md)
  * [Count](Documentation/Count.md)
* [Filter Creation](Documentation/Filter.md)
* [Update Expression](Documentation/Update.md)

