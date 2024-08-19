![ThorsMongo](../img/thorsmongoapi.jpg)

# Connection:

To communicate with a Mongo Server you need a connection to the server. This is achieved by the class `ThorsMongo`. This page describes the construction of this object and the parameters needed.

Note 1: Currently only SCRAM-SHA-256 authentication is supported.  
Note 2: I do plan to support SCRAM-SHA-1 in the near future.  
Note 3: I want to support SSH soon after that, but will depend on requests.  


## Connecting with UserName / Password:

The constructor for [`ThorsMongo`](../src/ThorsMongo/ThorsMongo.h#L86-L90) looks like this:

```C++
        ThorsMongo(MongoURL const&                  url,
                   Auth::UserNamePassword const&    authInfo,
                   Compression                      compression = Compression::Snappy,
                   Auth::Client const&              clientInfo = Auth::Client{"", {}}
        );
```

### Parameters:

#### url:
`url` is the address where there Mongo server is located, which can be simply represented by a string and an optional port number (if the port number is not provided it defaults to 27017). So this will usually look like this `{"mongo.server.company.com"}`.

#### authInfo:
`authInfo` provides authentication information to connect with the server (username/password/db). These are all provided as a single object as strings. Example: `{"testUser", "myPassword", "db"}` (PS please don't put passwords in your code :-) ).

#### compression:
This parameter is passed as the compression you would like to use with the Mongo Server. The supported [compressions](../src/ThorsMongo/MongoUtil.h#L64-L70) in Mongo are Snappy / Zlib / ZStd. This parameter can be the 'binary or' of these enum values representing the compression you would like to use. Example: `(Compression::Snappy | Compression::ZStd)`.

Note 1: As part of the handshake the server and client will agree on a mutually supported compression or none.  
Note 2: Currently this library only support Snappy.  
Note 3: I plan on support ZLib / ZStd at some point depending on requests.  

#### clientInfo:
clientInfo is information about your application/os/hardware provided to the Mongo Server.

The most commonly overridden values in this are the application name and platform name. You can set these values in the constructor. All other values have generated default values but you are free to modify these values as you see fit.

### Example:

```C++
    using ThorsAnvil::DB::Mongo::ThorsMongo;
    using ThorsAnvil::DB::Mongo::Compression;
    using ThorsAnvil::DB::Mongo::Auth::Client;

    ThorsMongo          mongo({"localhost"},                                // Host where Mongo Server lives (default Mongo port used).
                              {"Loki", getPassword(), "DB"},                // Authentication Information.
                              Compression::Snappy | Compression::ZStd,      // Requested Compression.
                              Auth::Client{"MyApplication", "Platform"}     // Client Information for the Server.
                             );
```

## ThorsMongo Interface:

This class has the following interface:

* [listDatabases()](#listdatabases)
* [operator[<DB-Name>]](#operator)


### listDatabases:

This method uses the Mongo API [**listDatabases**](https://www.mongodb.com/docs/manual/reference/command/listDatabases/). Please read the Mongo documentation for details.

```C++
        template<typename F>
        DBRange                 listDatabases(F const& filter, CommandConfig const& config = CommandConfig{});
        DBRange                 listDatabases(CommandConfig const& config = CommandConfig{});
```

#### Parameters:
There are no required parameters but (I suspect) that a filter is often used. So this method takes an optional "[filter](Filter.md)" followed by an optional [config](../src/ThorsMongo/ThorsMongoCommandConfig.h#L10-L24) object. The config object is used to pass any optional parameters that the Mongo API can take. If a parameter is not explicitly set then it will not be sent to the MongoServer.

| Parameter | Description |
| --------- | ----------- |
| filter | A "filter" on the `DBInfo` type. |
| config.nameOnly | A flag to indicate whether the command should return just the database names, or return both database names and size information. |
| config.authorizedDatabases | A flag that determines which databases are returned based on the user privileges when access control is enabled. |
| config.comment | A user-provided comment to attach to this command. |

#### Returns:

The returned value is usable in a boolean expression that indicates if the Mogo operation failed. If it did fail then sending the object to a stream will print an error message:

```C++
    auto databases = mongo.listDatabases();
    if (databases) {                                    // Same as databases.isOk()
        // It worked.
    }
    else {
        std::cerr << "Error: " << databases << "\n";    // Same as databases.getHRErrorMessage()
    }
```

The return value is also [C++ range](https://en.cppreference.com/w/cpp/ranges) where each value you iterate over is a [`DBInfo`](../src/ThorsMongo/ThorsMongoListDatabase.h#L13-L18) object.

```C++
    for (auto const& v: databases) {
        std::cerr << v.name << "\n";
    }
```

### operator[]:

This simply returns an object of type `DB`. This is a lightweight object. You can create any number of these objects and they all refer to the same entity. Please see document about the [DB](DB.md).

