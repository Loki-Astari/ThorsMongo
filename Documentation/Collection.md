![ThorsMongo](../img/thorsmongoapi.jpg)

# Collection:

An object if type `Collection` is a lightweight representation of a specific database/collection on the Mongo Server. There can be multiple `Collection` objects representing the same Mongo Server database/collection; an action applied to any of these objects has the same affect.

You can explicitly create objects of type `Collection` or you can use the `operator[]` on an [`DB`](DB.md) object to get a reference to an object.

It is **NOT** an error to create an object that does not represent a database/collection on the Mongo Server (i.e. The name does not match an existing database or collection name). You will get an error response when you communicate with the server using any of the methods on `Collection` object.

## Constructor:

Using the [constructor](../src/ThorsMongo/ThorsMongo.h#L167-L170).

```C++
        Collection(DB& db, std::string&& name)
            : mongoServer(db.mongoServer)
            , name(db.name + "::" + name)
        {}
```

### Parameters:

#### db:
A [DB](DB.md) object that has previously been created.

### name:
The collection name.

### Example:

```C++
        using ThorsAnvil::DB::Mongo::Collection;
        Collection      peopleTaxRecords(people, "TaxRecords");

        peopleTaxRecords.action();

        // Alternatively you don't need to create an object you can simply use the [] operator.
        // Because all Collection objects are lightweight objects that represent the same underlying collection data
        // this will always have the same effect as above.
        mongo["People"]["TaxRecords"].action();
```

## Collection Interface:

This class has the following interface:

* [rename()](#rename)
* [drop()](#drop)
* [countDocuments()](Count.md)
* [distinct()](Distinct.md)
* [insert()](Insert.md)
* [remove()](Delete.md)
* [find()](Find.md)
* [findAndReplaceOne()](FindAndModifyOne.md)
* [findAndRemoveOne()](FindAndModifyOne.md)
* [findAndUpdateOne()](FindAndModifyOne.md)


### rename:

This method uses the Mongo API [**renameCollection**](https://www.mongodb.com/docs/manual/reference/command/renameCollection/). Please read the Mongo documentation for details.

```C++
        AdminResult             rename(std::string const& name, RenameConfig const& config = RenameConfig{});
```

#### Parameters:
The only required parameter is `name`, but there are multiple optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoAdmin.h#L11-L25) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.

| Parameter | Description |
| --------- | ----------- |
| name | The new name of the collection. |
| config.dropTarget | If true, mongod will drop the target of renameCollection prior to renaming the collection.<br>The default value is false. |
| config.writeConcern | A document that expresses the write concern for the operation.<br>Omit to use the default write concern.<br>When issued on a sharded cluster, mongos converts the write concern of the renameCollection command and its helper db.collection.renameCollection() to "majority". |
| config.comment | A user-provided comment to attach to this command. |

#### Returns:

The [`AdminResult`](../src/ThorsMongo/ThorsMongoAdmin.h#L71) type contains no data, but can used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        auto result = mongo["People"]["TaxRecords"].rename("TaxRecord2020" RenameConfig{}.setComment("Tax seasson finished"));
        if (!result) {                                          // Same as !result.isOk()
            std::cerr << "Rename Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

### drop:

This method uses the Mongo API [**drop**](https://www.mongodb.com/docs/manual/reference/command/drop/). Please read the Mongo documentation for details.

```C++
        AdminResult             drop(DropCollectionConfig const& config = DropCollectionConfig{});
```

#### Parameters:
There are no required parameters. The optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoAdmin.h#L27-L38) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.

| Parameter | Description |
| --------- | ----------- |
| config.writeConcern | A document that expresses the write concern for the operation.<br>Omit to use the default write concern.<br>When issued on a sharded cluster, mongos converts the write concern of the renameCollection command and its helper db.collection.renameCollection() to "majority". |
| config.comment | A user-provided comment to attach to this command. |

#### Returns:

The [`AdminResult`](../src/ThorsMongo/ThorsMongoAdmin.h#L71) type contains no data, but can used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        auto result = mongo["People"]["TaxRecords"].drop();
        if (!result) {                                      // Same as !result.isOk()
            std::cerr << "Drop Failed: " << result << "\n"; // Same as result.getHRErrorMessage()
        }
```

