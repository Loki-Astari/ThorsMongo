![ThorsMongo](../img/thorsmongo.jpg)

# Distinct:

This method uses the Mongo API [**distinct**](https://www.mongodb.com/docs/manual/reference/command/distinct/). Please read the Mongo documentation for details.

```C++
        template<typename T, typename F>    DistinctResult<T>   distinct(std::string const& key, F const& query, DistinctConfig const& config = DistinctConfig{});
        template<typename T>                DistinctResult<T>   distinct(std::string const& key, DistinctConfig const& config = DistinctConfig{});
```

The `distinct()` method takes a field name (`key`) an optional "[Filter](Filter.md)" as a search criteria and returns all the distinct values in the specified field.

Note: The user is supposed to provide the type of the field `T`.

#### Parameters:
Optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoDistinct.h#L10-L24) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.

| Parameter | Description |
| --------- | ----------- |
| config.readConcern | Defines the read concerns used. |
| config.collation | Specifies the collation to use for the operation. |
| config.comment | A user-provided comment to attach to this command. |


#### Returns:

The [`RDistinctResult<T>`](../src/ThorsMongo/ThorsMongoDistinct.h#L26-L30) contains vector `values` that contains all the distinct values in the requested field. The result  can also used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        // Note: See "Filter" documentation on how to define `FindPeopleOver`
        auto result = mongo["People"]["Home"].distinct<std::string>("name", FindPeopleOver(90));
        if (result) {
            for (auto const& name: result.values) {
                std::cerr << "Name: " << name << "\n";
            }
        }
        else {
            std::cerr << "Distinct Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

