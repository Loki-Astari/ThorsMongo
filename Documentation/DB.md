![ThorsMongo](../img/thorsmongoapi.jpg)

# DB:

An object if type `DB` is a lightweight representation of a specific database on the MongoDB Server. There can be multiple `DB` objects representing the same MongoDB Server database; an action applied to any of these objects has the same affect.

You can explicitly create objects of type `DB` or you can use the `operator[]` on the [`ThorsMongo`](ThorsMongo.md) connection object to get a reference to an object.

It is **NOT** an error to create an object that does not represent a database on the MongoDB Server (i.e. The name does not match an existing database). You will get an error response when you communicate with the server using any of the methods on `DB` or associated [`Collection`](Collection.md) objects.

## Constructor:

Using the [constructor](../src/ThorsMongo/ThorsMongo.h#L138-L141):

```C++
        DB(ThorsMongo& mongoServer, std::string&& name)
            : mongoServer(mongoServer)
            , name(name)
        {}
```

### Parameters:

#### mongoServer:
A [connection](ThorsMongo.md) object that has previously been created.

### name:
The database name.

### Example:

```C++
        using ThorsAnvil::DB::Mongo::DB;
        DB      people(mongo, "People");

        people.action();

        // Alternatively you don't need to create an object you can simply use the [] operator.
        // Because all DB objects are lightweight objects that represent the same underlying database data
        // this will always have the same effect as above.
        mongo["People"].action();
```

## DB Interface:

This class has the following interface:

* [listCollections()](#listcollections)
* [operator[<Collection-Name>]](#operator)


### listCollections:

This method uses the Mongo API [**listCollections**](https://www.mongodb.com/docs/manual/reference/command/listCollections/). Please read the MongoDB documentation for details.

```C++
        template<typename F>
        LCRange                 listCollections(F const& filter, CommandConfig const& config = CommandConfig{});
        LCRange                 listCollections(CommandConfig const& config = CommandConfig{});
```

#### Parameters:
There are no required parameters but (I suspect) that a filter is often used. So this method takes an optional [filter](Filter.md) followed by an optional [config](../src/ThorsMongo/ThorsMongoCommandConfig.h#L10-L24) object. The config object is used to pass any optional parameters that the MongoDB API can take. If a parameter is not explicitly set then it will not be sent to the MongoDB Server.

| Parameter | Description |
| --------- | ----------- |
| filter | A "filter" on the `CollectionInfo` type. |
| config.nameOnly | A flag to indicate whether the command should return just the name and type or return both the name and other information. |
| comfig.authorizedCollections | A flag, when set to true and used with nameOnly: true, that allows a user without the required privilege (i.e. listCollections action on the database) to run the command when access control is enforced. |
| config.comment | A user-provided comment to attach to this command. |

#### Returns:
The returned value is usable in a boolean expression that indicates if the Mogo operation failed. If it did fail then sending the object to a stream will print an error message:

```C++
    auto collections = mongo["People"].listDatabases();
    if (collections) {                                  // Same as databases.isOk()
        // It worked.
    }
    else {
        std::cerr << "Error: " << collections << "\n";  // Same as databases.getHRErrorMessage()
    }
```

The return value is also [C++ range](https://en.cppreference.com/w/cpp/ranges) where each value you iterate over is a [`CollectionInfo`](../src/ThorsMongo/ThorsMongoListCollection.h#L92-L99).

```C++
    for (auto const& v: collections) {
        std::cerr << v.name << "\n";
    }
```

### operator[]:

This simply returns an object of `Collection`. This is a lightweight object. You can create any number of these objects and they all refer to the same entity. Please see document about the [Collection](Collection.md).

