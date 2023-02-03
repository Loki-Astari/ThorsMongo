# ThorsSerializer

## Support for

* [Json](https://www.json.org/json-en.html)
* [Yaml](https://yaml.org/)
* [Bson](http://bsonspec.org/) **NEW**


[![Build Status](https://travis-ci.com/Loki-Astari/ThorsSerializer.svg?branch=master)](https://travis-ci.com/Loki-Astari/ThorsSerializer)
[![codecov.io](http://codecov.io/github/Loki-Astari/ThorsSerializer/coverage.svg?branch=master)](http://codecov.io/github/Loki-Astari/ThorsSerializer?branch=master)   
[![Code Review](http://www.zomis.net/codereview/shield/?qid=81920)](http://codereview.stackexchange.com/q/81920/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=79281)](http://codereview.stackexchange.com/q/79281/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=7567)](http://codereview.stackexchange.com/q/7567/507)
[![Code Review](http://www.zomis.net/codereview/shield/?qid=7536)](http://codereview.stackexchange.com/q/7536/507)

[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/thors-serializer)
[![Brew package](https://img.shields.io/badge/Brew-package-blueviolet)](https://formulae.brew.sh/formula/thors-serializer)


## Benchmark Results
Conformance [mac](https://LokiAstari.com/Json/Conformance.osx.html) [linux](https://LokiAstari.com/Json/Conformance.linux.html)  
Performance [max](https://LokiAstari.com/Json/Performance.osx.html) [linux](https://LokiAstari.com/Json/Performance.linux.html)  
For details see: [JsonBenchmark](https://github.com/Loki-Astari/JsonBenchmark)  

![ThorStream](img/stream.jpg)

Yet another JSON/YAML/BSON serialization library for C++.

Unlike other libraries this one does not require you to build DOM of you object before serialization. Using a declarative C++ style you define what C++ classes (and members) you want to serialize "ThorSerializer" will generate the appropriate code automagically.

* [Objective](doc/objective.md)
* [Trivial Example](doc/example0.md)
* [Simple Example](doc/example1.md)
* [Enum Example](doc/exampleE.md)
* [Bigger Example](doc/example2.md)
* [PolyMorphic Example](doc/example3.md)
* [Installing](doc/building.md)
* [Documentation Usage](doc/usage.md)
* [Documentation Internal](https://lokiastari.com/ThorsSerializer/)

## HomeBrew

Can be installed via brew on Mac and Linux

    brew install thors-serializer

* Mac: https://formulae.brew.sh/formula/thors-serializer
* Linux: https://formulae.brew.sh/formula-linux/thors-serializer

## Header Only

To install header only version

    git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsSerializer.git

## Version 2

I have deprecated the `jsonImport()` and `jsonExport()` functions. These have been replaced with `jsonImporter()` and `jsonExporter()` functions (though the original versions still exist but are marked `[[deprecated]]`). The main difference is that the new functions catch exceptions (by default) this makes the serialization work like normal C++ serialization and simply set the bad bit on the stream.

## Contributors

Added the all-contributers bot to generate the table.


-----

[One Page](doc/full.md)

