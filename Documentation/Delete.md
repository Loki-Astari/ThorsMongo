![ThorsMongo](../img/thorsmongoapi.jpg)

# Delete:

This method uses the MongoDB API [**delete**](https://www.mongodb.com/docs/manual/reference/command/delete/). Please read the MongoDB documentation for details.

```C++
        template<typename T>        RemoveResult    remove(std::vector<T> const& search, RemoveConfig const& config = RemoveConfig{});
        template<typename... T>     RemoveResult    remove(std::tuple<T...> const& search, RemoveConfig const& config = RemoveConfig{});
        template<typename T>        RemoveResult    remove(Query<T, std::string> const& search, RemoveConfig const& config = RemoveConfig{});
```

The `remove()` method takes one or more `Query<Filter>` objects. A "[Filter](Filter.md)" object defines a search criteria that is used to match records on the server. Please see the filter section on how to write them.

Note: A record is matched if any query object matches the record.

#### Parameters:
The required parameter `search` can either be a `std::vector<>` or a `std::tuple<>` or a single `Query<T>`. Optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoRemove.h#L10-L30) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.

Note: All objects passed in `search` should be of `Query<Filter>` (inside the vector or tuple).


| Parameter | Description |
| --------- | ----------- |
| search | One or more `Query<Filter>` objects that will be used to match records in the collection. |
| config.maxTimeMS | Specifies a time limit in milliseconds. If you do not specify a value for maxTimeMS, operations will not time out. A value of 0 explicitly specifies the default unbounded behavior.<br><br>MongoDB terminates operations that exceed their allotted time limit using the same mechanism as db.killOp(). MongoDB only terminates an operation at one of its designated interrupt points. |
| config.writeConcern | A document that expresses the write concern for the operation.<br>Omit to use the default write concern.<br>When issued on a sharded cluster, mongos converts the write concern of the renameCollection command and its helper db.collection.renameCollection() to "majority". |
| config.comment | A user-provided comment to attach to this command. |
| config.let | Specifies a document with a list of variables. This allows you to improve command readability by separating the variables from the query text. |
| config.ordered | If true, then when a delete statement fails, return without performing the remaining delete statements.<br>If false, then when a delete statement fails, continue with the remaining delete statements, if any.<br>Defaults to true. |

#### Returns:

The [`RemoveResult`](../src/ThorsMongo/ThorsMongoRemove.h#L32-L33) type contains no data, but can used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        // Note: See "Filter" documentation on how to define `FindByAccountId`
        auto result = mongo["People"]["Home"].remove(Query<FindByAccountId>(12));
        if (!result) {                                          // Same as !result.isOk()
            std::cerr << "Remove Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

