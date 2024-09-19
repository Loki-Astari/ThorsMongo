![ThorStream](../img/stream.jpg)

### Objective:
    The objective is to make serialization/de-serialization of C++ object to/from
    JSON/YAML/Bson trivial.

    This means:
        1) does not build a JSON/YAML/BSON object. Reads data directly into C++ object.
        2) In normal usage there should be NO need to write any code.
        3) User should not need to understand JSON/YAML/BSON or validate its input.
        4) Should work seamlessly with streams.
        5) Standard containers should automatically work

    I am not concerned about speed.
    Though my trivial test work just fine in terms of speed.

    The design was done with the primary goal of communicating with WEB-Servers
    that speak JSON. The main envisioned usage was for mobile devices were many
    small JSON objects are transfered in both directions.
