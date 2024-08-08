# ThorsMongo:
Mongo API for C++

# Description:
This is an attempt to have a simple to use TypeSafe API to connect and manipulate data on a Mongo Server.  

This API does not currently implement the full Mongo API, but rather the subset of the API around data manipulation and storage.  

This library makes heavy use of ThorsSerialize API to automate the generation of the BSON documents that are exchanged between the application and the Mongo Server. The advantage of this library is that no code needs to be written to create or generate BSON objects (this is all automated).  

# Usage:

Note: All the code documented here is in file [Example.cpp](./src/Example/Example.cpp) code.


## Connecting:
All communication done with the Mongo Server id done via the `ThorsMongo` class.  

````
    // Connecting to Mongo

    using ThorsAnvil::DB::Mongo::ThorsMongo;
    ThorsMongo          mongo({"localhost", 27017}, {"usernmae", "password", "DB"});
````


Note: I have currently only implemented SCRAM-SHA-256 authentication mechanism. So the user must have this enabled.  

````
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
````

On a Mongo Server there are multiple Databases. Each Database can have multiple Collections. There are appropriate class for these types of object.  

* `ThorsAnvil::DB::Mongo::DB`
* `ThorsAnvil::DB::Mongo::Collection`

````
    using ThorsAnvil::DB::Mongo::DB;
    using ThorsAnvil::DB::Mogno::Collection;

    DB          db(mongo, "DB");
    Collection  collection(db, "People");

    // For convenience the [] operator have been overloaded to give you accesses to these objects.
    // without the need to create local objects.
    mongo["DB"].method();               // Has same affect as: db.method();
    mongo["DB"]["People"].method();     // Has same effect as: db["People"].method()
                                        //                and: collection.method();
````

## Inserting Data:

You can insert any data you like into a collection.  
To serialize the data you must declare the class you are sending to Mongo as serializable (using ThorsSerialize). This has a tiny bit of boilerplate declaration per class. I provide a simple example here. More detailed examples and documentation can be found [here](https://github.com/Loki-Astari/ThorsSerializer/blob/master/README.md).  

````
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
    struct Person
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
````

## Inserting Data:

To insert data into a collection call the `insert()` method passing either a vector or a tuple of objects. Note: The types of all the objects have to be serializable.  

````
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
````

If you only want to insert a single item you can invisibly create a vector by wrapping the object in curly braces.        

````
        auto result = mongo["DB"]["Collection"].insert({ aSinglePersonObject });
````


## Find Data:

To find data you must specify a "Filter".  
This requires some boilerplate to allow you to match against specific fields in your data. I will provide a simple examples here. Details will be provided below in the Query documentation.  

The `find()` method takes a filter and returns a C++ range.  


````
    // This macro creates a type called "FindEqName"
    // It can be used to filter recors by Person.name on the server using "Eq" (Equality)

    ThorsMongo_CreateFilter(FindEqName, Person, name, Eq);

    void findPeopleInMongoByName(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name)
    {
        // Retuns a C++ range of all the objects that match the query.
        // Note:  The range hides a Mongo cursor so as you you iterate across the query
        //        this may result in more calls to Mongo to retrieve more data automatically.

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
````

## Delete Data:

The remove method operates on a `Query`. This is basically a "Filter" with an extra parameter indicating if the remove should apply to the first or all matches. The example below shows remove using a single `Query` object, but this method can take any number of `Query` objects that are all applied in parallel.  
Note: If you are only deleting one and the filter matches multiple records in the collection then you are effectively deleting a random matching record. Please read the full documentation to understand how to control the filter to get an exact match or use findAndRemoveOne() method for more control.  


````
    // Create a filter on Person.age using the "Gt" (Greater than) operator
    ThorsMongo_CreateFilter(FindGtAge, Person, age, Gt);

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
````

## Find And Modify One

There is a family of functions to find and modify a single record.  

* findAndReplaceOne();
* findAndRemoveOne();
* findAndUpdateOne();

All these functions use a "Filter" to select a single record. If your filter matches multiple records you can add a sort order (the first item in the sorted results is modified. See full documentation for details).  

### Find and Replace

The `findAndReplaceOne()` methods find and update a single record using a "Filter" and replaces the record with a new record. This is useful if all the updates are done in the application side.  

````
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
````

### Find and Remove

The `findAndRemoveOne()` methods find and removes a single record using a "Filter".

````
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
````

### Find and Update

The `findAndUpdateOne()` methods find a single record using a "Filter" and then applies a custom update operation on the server side. This can simplify the client code and allow updates to parts of the document on the server without having to download the data onto the client.

The update is expressed as an "Expression" that requires some boilerplate. I will provide a simple example here. Details will be provided below in the Update documentation.  

````
    // This macro creates a type called "SetAge"
    // It will "Set" the value of Person.age on the server.

    ThorsMongo_CreateUpdate(SetAge, Set, Person, age);

    void updateAddressCity(ThorsAnvil::DB::Mongo::ThorsMongo& mongo, std::string const& name, std::uint32_t newAge)
    {
        using ThorsAnvil::DB::Mongo::Query;
        using ThorsAnvil::DB::Mongo::Remove;
        std::cerr << "Checking: " <<  ThorsAnvil::Serialize::jsonExporter(SetAge{newAge}) << "\n";
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
````



