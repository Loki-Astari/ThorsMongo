![ThorsMongo](../img/thorsmongo.jpg)

# Delete:

This method uses the Mongo API [**insert**](https://www.mongodb.com/docs/manual/reference/command/insert/). Please read the Mongo documentation for details.

```C++
        template<typename T>        InsertResult    insert(std::vector<T> const& data, InsertConfig const& config = InsertConfig{});
        template<typename... T>     InsertResult    insert(std::tuple<T...> const& data, InsertConfig const& config = InsertConfig{});
```

Note: All objects in the vector or tuple must be a type that has been marked as serializeable via [ThorsSerialize](https://github.com/Loki-Astari/ThorsSerializer).

```C++
    // Example
    class BankAccount
    {
        // Make the serializeable class a friend so it can accesses private members.
        friend class ThorsAnvil::Serialize::Traits<BankAccount>;
        std::string     name;
        std::uint64_t   accountId;
        std::uint64_t   balance;

        public:
            // public Members;
    };

    // Declare the class as serializeable:
    ThorsAnvil_MakeTrait(BankAccount, name, accountId, balance);
```

#### Parameters:
The required parameter `data` can either be a `std::vector<>` or a `std::tuple<>`. Optional parameter that can be passed using the [`config`](../src/ThorsMongo/ThorsMongoInsert.h#L10-L30) object. Any optional parameters that are not explicitly set on the `config` object will not be sent to the server.

| Parameter | Description |
| --------- | ----------- |
| data | A container of serialazable objects that will be inserted into the collection |
| config.maxTimeMS | Specifies a time limit in milliseconds. If you do not specify a value for maxTimeMS, operations will not time out. A value of 0 explicitly specifies the default unbounded behavior.<br><br>MongoDB terminates operations that exceed their allotted time limit using the same mechanism as db.killOp(). MongoDB only terminates an operation at one of its designated interrupt points. |
| config.writeConcern | A document that expresses the write concern for the operation.<br>Omit to use the default write concern.<br>When issued on a sharded cluster, mongos converts the write concern of the renameCollection command and its helper db.collection.renameCollection() to "majority". |
| config.comment | A user-provided comment to attach to this command. |
| config.ordered | If true, then when an insert of a document fails, return without inserting any remaining documents listed in the inserts array.<br>If false, then when an insert of a document fails, continue to insert the remaining documents.<br>Defaults to true. |
| config.bypassDocumentValidation | Enables insert to bypass document validation during the operation. This lets you insert documents that do not meet the validation requirements. |

#### Returns:

The [`InsertResult`](../src/ThorsMongo/ThorsMongoInsert.h#L32-L37) contains the field `inserted` that is populated with a unique identifier for each object inserted. If the object has a member `_id` this is used otherwise one is automatically created on the client side and sent to the server. The result can also used to check the operation succeeded and on failure to get the error information.

#### Example:

```C++
        // Insert three bank accounts.
        // Use the C++ {} to automatically create a vector<>
        auto result = mongo["People"]["Home"].insert({LokiBankAccount, OdinBankAccount, ThorBankAccount});
        if (result) {                                           // Same as result.isOk()
            for (auto const& id: result.inserted) {
                std::cerr << "Inserted Id: " << id << "\n";
            }
        }
        else {
            std::cerr << "Remove Failed: " << result << "\n";   // Same as result.getHRErrorMessage()
        }
```

