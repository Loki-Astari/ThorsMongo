![ThorsMongo](../img/thorsmongo.jpg)

# FindAndModifyOne:

This method uses the Mongo API [**findAndModify**](https://www.mongodb.com/docs/manual/reference/command/findAndModify/). Please read the Mongo documentation for details.

The behavior of this API depends a lot on the parameters passed. So I have split it usage into three distinct methods. But because the methods share a lot of commonality I will discuss them all together. The main point of this API is that it will "Modify" zero or one record. If the search criteria match multiple records on the Mongo Server only one will be updated (effectively this is Random (or you should think of it as Random)). To compensate for this you should use an appropriate search function to select a single value, or provide a sort order (operation will be applied to the top of the sort) so that you know explicitly which document will be modified.

## findAndReplaceOne

```C++
        template<typename T, typename F>    FAModifyResult<T>   findAndReplaceOne(F const& search, T const& replace, FAModifyConfig const& config = FAModifyConfig{});
```

This method replaces the matched record with a new record (overwriting the existing record (effectively removing it)).

## findAndRemoveOne

```C++
        template<typename T, typename F>    FAModifyResult<T>   findAndRemoveOne(F const& search, FARemoveConfig const& config = FARemoveConfig{});
```

This method removes the matched record from the collection.

## findAndUpdateOne

```C++
        template<typename T, typename F, typename U>
                                            FAModifyResult<T>   findAndUpdateOne(F const& search, U const& update, FAModifyConfig const& config = FAModifyConfig{});
        template<typename T, typename F, typename... U>
                                            FAModifyResult<T>   findAndUpdateOne(F const& search, std::tuple<U const&...> update, FAModifyConfig const& config = FAModifyConfig{});
```

This method updates in place the matched record.  
The `update` parameter is applied to the record on the server to modify in place the existing record. This parameter should be an "[Update](Update.md)" object.


#### Parameters:
* The parameter `search` is a "[Filter](Filter.md)".
* The parameter `update` is a "[Update](Update.md)".
* Other optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoFindAndModify.h#L13-L46) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.


| Parameter | Description |
| --------- | ----------- |
| search | The "[Filter](Filter.md)" used to select records from the collection. |
| update | The "[Update](Update.md)" used to modify a record in place. |
| config.sort | The sort specification for the ordering of the results. |
| config.new  | When true, returns the updated document rather than the original. The default is false. |
| config.fields | (Not valid for findAndRemoveOne) A projection specification to determine which fields to include in the returned documents.<br><br>See [Find](Find.md) for details on projections. |
| config.upsert | (Not valid for findAndRemoveOne). If zero records were matched performs an insert. |
| config.bypassDocumentValidation | This lets you update documents that do not meet the validation requirements. |
| config.writeConcern | A document that expresses the write concern for the operation.<br>Omit to use the default write concern.<br>When issued on a sharded cluster, mongos converts the write concern of the renameCollection command and its helper db.collection.renameCollection() to "majority". |
| config.maxTimeMS | Specifies a time limit in milliseconds. If you do not specify a value for maxTimeMS, operations will not time out. A value of 0 explicitly specifies the default unbounded behavior.<br><br>MongoDB terminates operations that exceed their allotted time limit using the same mechanism as db.killOp(). MongoDB only terminates an operation at one of its designated interrupt points. |
| config.collation | Specifies the collation to use for the operation. |
| config.hint | Index specification. Specify either the index name as a string or the index key pattern. If specified, then the query system will only consider plans using the hinted index. |
| config.comment | A user-provided comment to attach to this command. |
| config.let | Specifies a document with a list of variables. This allows you to improve command readability by separating the variables from the query text. |

#### Returns:

The [`FAModifyResult<T>`](../src/ThorsMongo/ThorsMongoFindAndModify.h#L89-L94) contains `value`. This is the original value before the "Modify" operation was performed (Note: unless `config.new == true`; then the new value (rather than the original) is returned. The result can also be used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        // Note: See "Filter" documentation on how to define `FindByBalanceOver`
        // Note: See "Update" documentation on how to define `AddMilionareBalanceBonus`
        auto result = mongo["People"]["Home"].
            findAndUpdateOne<BankAccount>(FindByBalanceOver(1'000'000'00),
                                          AddMilionareBalanceBonus{12}
                                          FAModifyConfig{}
                                            .setSort({{"name", Sort::Decending}})
                                         );
        if (result) {                                           // Same as !result.isOk()
            // Dump the original value before the change.
            std::cerr << ThorsAnvil::jsonExporter(result.value) << "\n"
        }
        else {
            std::cerr << "Find Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

