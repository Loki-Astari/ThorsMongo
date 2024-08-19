![ThorsMongo](../img/thorsmongoapi.jpg)

# Find:

This method uses the Mongo API [**find**](https://www.mongodb.com/docs/manual/reference/command/find/). Please read the Mongo documentation for details.

```C++
        template<typename T>                FindRange<T>    find(FindConfig const& config = FindConfig{});                  // Find All
        template<typename T, typename F>    FindRange<T>    find(F const& search, FindConfig const& config = FindConfig{});
```

The `find()` method takes an optional "[Filter](Filter.md)" and returns a C++ `range` of type T. Iterating through this range may result in subsequent calls to the Mongo API [**getMore**](https://www.mongodb.com/docs/manual/reference/command/getMore/) but this is invisible to the user of the API.

#### Parameters:
The optional parameter `search` is a "[Filter](Filter.md)". Other optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoFind.h#L11-L79) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.


| Parameter | Description |
| --------- | ----------- |
| search | The filter used to select records from the collection. |
| config.sort | The sort specification for the ordering of the results. |
| config.projection | The projection specification to determine which fields to include in the returned documents. |
| config.hint | Index specification. Specify either the index name as a string or the index key pattern. If specified, then the query system will only consider plans using the hinted index. |
| config.skip | Number of documents to skip. Defaults to 0. |
| config.limit | The maximum number of documents to return.<br>If unspecified, then defaults to no limit.<br>A limit of 0 is equivalent to setting no limit. |
| config.batchSize | The number of documents to return in the first batch.<br>Defaults to 101.<br>A batchSize of 0 means that the cursor will be established, but no documents will be returned in the first batch. |
| config.singleBatch | Determines whether to close the cursor after the first batch. Defaults to false. |
| config.comment | A user-provided comment to attach to this command. |
| config.maxTimeMS | Specifies a time limit in milliseconds. If you do not specify a value for maxTimeMS, operations will not time out. A value of 0 explicitly specifies the default unbounded behavior.<br><br>MongoDB terminates operations that exceed their allotted time limit using the same mechanism as db.killOp(). MongoDB only terminates an operation at one of its designated interrupt points. |
| config.readConcern | Defines the read concerns used. |
| config.max | The exclusive upper bound for a specific index. See cursor.max() for details. |
| config.min | The inclusive lower bound for a specific index. See cursor.min() for details. |
| config.returnKey | If true, returns only the index keys in the resulting documents.<br>Default value is false.<br>If returnKey is true and the find command does not use an index, the returned documents will be empty. |
| config.showRecordId | Determines whether to return the record identifier for each document. If true, adds a field $recordId to the returned documents. |
| config.tailable | Returns a tailable cursor for a capped collections. |
| config.awaitData | Use in conjunction with the tailable option to block a getMore command on the cursor temporarily if at the end of data rather than returning no data. After a timeout period, find returns as normal. |
| config.noCursorTimeout | Prevents the server from timing out non-session idle cursors after an inactivity period of 30 minutes. Ignored for cursors that are part of a session. For more information, refer to Session Idle Timeout. |
| config.allowPartialResults | For queries against a sharded collection, allows the command (or subsequent getMore commands) to return partial results, rather than an error, if one or more queried shards are unavailable. |
| config.collation | Specifies the collation to use for the operation. |
| config.allowDiskUse | Use this option to override allowDiskUseByDefault for a specific query. |
| config.let | Specifies a document with a list of variables. This allows you to improve command readability by separating the variables from the query text. |


#### Returns:

The [`FindRange<T>`](../src/ThorsMongo/ThorsMongoFind.h#L83-L132) is a C++ `range` for the data retrieved. The result can also be used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        // Note: See "Filter" documentation on how to define `FindByBalanceOver`
        auto result = mongo["People"]["Home"].find<BankAccount>(FindByBalanceOver(1'000'000'00));
        if (result) {                                           // Same as !result.isOk()
            for (auto const& f: result)
            {
                // f is an object of type bank account.
            }
        }
        else {
            std::cerr << "Find Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

#### Interesting Parameters:

##### config.sort:

An [ordered list of fields](../src/ThorsMongo/ThorsMongoCommon.h#L160-L169) to sort the records on the server side. Each field is a string with the field name and if the sort should be Ascending or Descending.

```C++
        auto result = mongo["People"]["Home"].
            find<BankAccount>(FindByBalanceOver(1'000'000'00),
                              FindConfig{}
                                .setSort(
                                         // Sort by account Name then accountID
                                         {{"name", Sort::Descending}, {"accounID", Sort::Ascending}}
                                        )
                                .setLimit(500)
                             );
```

##### config.projection:

The records in the collection can be arbitrarily large. By default the `find()` will return the whole record. If you only need a portion of the record this can be a waste of bandwidth. A projection allows you to define what fields of a record to return. Usually (but not required) the projection will match the type being iterated over.

```C++
        class PersonBigClass
        {
            friend class ThorsAnvil::Serialize::Traits<PersonBigClass>;
            std::vector<BigObjects>     big;
            std::vector<MoreData>       data;
            std::vector<Stuff>          stuff;
            std::string                 name;
            std::uint32_t               age;
        };
        class PersonLimited
        {
            friend class ThorsAnvil::Serialize::Traits<PersonLimited>;
            std::vector<Stuff>          stuff;
            std::string                 name;
            std::uint32_t               age;
        };


        ThorsAnvil_MakeTrait(PersonBigClass, big, data, stuff, name, age);
        ThorsAnvil_MakeTrait(PersonLimited,  stuff, name, age);

        auto result = mongo["People"]["Big"].insert({PersonBigClass{/* Construct This object */}});

        // If you don't care about the big/data fields there is no point in retrieving them from the Big Collection.
        // In this case you can use a **projection** to limit the fields that are returned.
        using ThorsAnvil::DB::Mongo::Projection;
        auto result = mongo["People"]["Big"]
            .find<PersonLimited>(FindByAgeOver(21),
                                 FindConfig{}
                                    .setProjection(Projection::create<PersonLimited>())
                                    .setLimit(500)
                                    .setBatchSize(500)
                                    .setSingleBatch(true)
                                );
```


