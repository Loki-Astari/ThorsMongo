![ThorsMongo](../img/thorsmongoapi.jpg)

# Count:

This method uses the Mongo API [**count**](https://www.mongodb.com/docs/manual/reference/command/count/). Please read the Mongo documentation for details.

```C++
        template<typename F>    CountResult     countDocuments(F const& query, CountConfig const& config);
                                CountResult     countDocuments(CountConfig const& config);
```

The `countDocuments()` method takes an optional "[Filter](Filter.md)" as a search criteria and counts the number of records that match the "Filter".

#### Parameters:
Optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoCount.h#L10-L36) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.

| Parameter | Description |
| --------- | ----------- |
| config.limit | The maximum number of matching documents to return. |
| config.skip  | The number of matching documents to skip before returning results. |
| config.hint  | The index to use. Specify either the index name as a string or the index specification document. |
| config.readConcern | Defines the read concerns used. |
| config.maxTimeMS | Specifies a time limit in milliseconds. If you do not specify a value for maxTimeMS, operations will not time out. A value of 0 explicitly specifies the default unbounded behavior.<br><br>MongoDB terminates operations that exceed their allotted time limit using the same mechanism as db.killOp(). MongoDB only terminates an operation at one of its designated interrupt points. |
| config.collation | Specifies the collation to use for the operation. |
| config.comment | A user-provided comment to attach to this command. |


#### Returns:

The [`CountResult`](../src/ThorsMongo/ThorsMongoCount.h#L39-L42) contains `n` to represent the number of records found. It can also used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        // Note: See "Filter" documentation on how to define `FindPeopleOver`
        auto result = mongo["People"]["Home"].countDocuments(FindPeopleOver(21));
        if (result) {                                           // Same as !result.isOk()
            std::cerr << "There are " << result.n << " people over 21\n";
        }
        else {
            std::cerr << "Count Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

