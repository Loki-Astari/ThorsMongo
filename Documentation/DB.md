![ThorsMongo](../img/thorsmongo.jpg)

# DB:

The DB object is a lightweight representation of a specific database. There can be multiple DB objects representing the same DB; an action applied to any of these objects has the same affect.

You can explicitly create a DB object or you can use the `operator[]` on the [`ThorsMongo`](ThorsMongo.db) connection object.

It is **NOT** an error to create an object that does not represent a DB on the Mongo Server (i.e. The name does not match an existing DB). You will get an error response when you communicate with the server using any of the API's on the DB or associated [`Collection`](Collection.md) objects.

## Constructor:

Using the [constructor](../src/ThorsMongo/ThorsMongo.h#L138-L141):

        DB(ThorsMongo& mongoServer, std::string&& name)
            : mongoServer(mongoServer)
            , name(name)
        {}

### Parameters:

#### mongoServer:
A [connection](ThorsMogno.md) object that has previously been created.

### name:
The database name.

### Example:

        using ThorsAnvil::DB::Mongo::DB;
        DB      people(mongo, "People");

        people.action();

        // Alternatively you don't need to create an object you can simply use the [] operator.
        // Because all DB objects are lightwight objects that represent the same underlying DB data
        // this will always have the same effect as above.
        mongo["People"].action();


## DB Interface:

This class has the following interface:

* listCollections()
* operator[<Collection-Name>]


### listCollections:

This method uses the Mongo API [`lisCollections`](https://www.mongodb.com/docs/manual/reference/command/listCollections/).

There are no required parameters but (I suspect) that a filter is often used. So this method takes an optional [filter](Filter.md) followed by an optional [config](../src/ThorsMongo/ThorsMongoCommandConfig.h#L10-L24) object. The config object is used to pass any optional parameters that the Mongo API can take. If a parameter is not explicitly set then it will not be sent to the MongoServer.

        template<typename F>
        LCRange                 listCollections(F const& filter, CommandConfig const& config = CommandConfig{});
        LCRange                 listCollections(CommandConfig const& config = CommandConfig{});

#### Example:

Normal usage would be:

    auto collections = mongo["People"].listDatabases();

The returned value is also usable in a boolean expression that indicates if the Mogo operation failed. If it did fail then sending the object to a stream will print an error message:

    if (collections) {                                  // Same as databases.isOk()
        // It worked.
    }
    else {
        std::cerr << "Error: " << collections << "\n";  // Same as databases.getHRErrorMessage()
    }

The return value is a [C++ range](https://en.cppreference.com/w/cpp/ranges) where each value you iterate over is a [`CollectionInfo`](../ThorsMongo/ThorsMongoListCollection.h#L92-L99).

    for (auto const& v: collections) {
        std::cerr << v.name << "\n";
    }


### operator[]:

This simply returns an object of `Collection`. This is a lightweight object. You can create any number of these objects and they all refer to the same entity. Please see document about the [Collection](Collection.md).








