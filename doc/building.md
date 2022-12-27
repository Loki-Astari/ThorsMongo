[![Build Status](https://travis-ci.org/Loki-Astari/ThorsSerializer.svg?branch=master)](https://travis-ci.org/Loki-Astari/ThorsSerializer)

![ThorStream](../img/stream.jpg)

# Install Instructions:
## From [Homebrew](https://brew.sh/)
````bash
brew update
brew install thors-serializer
````
## From [GitHub](https://github.com/Loki-Astari/ThorsSerializer) Header only version

```bash
    git clone --single-branch --branch header-only https://github.com/Loki-Astari/ThorsSerializer.git
````

This will download just the set of required header files into the directory ThorsSerializer.

You will need to add this directory to your include path.


## From [GitHub](https://github.com/Loki-Astari/ThorsSerializer) For Development

The basic script for installing everything is:

````bash
    > git clone https://github.com/Loki-Astari/ThorsSerializer.git
    > cd ThorsSerializer
    > ./configure
    > make
    > sudo make install
````

But installing everything requires a couple of extra libraries and some development tools. You may not need all these tools (try and use brew if you don't).

### Description
By default installation will be in `/usr/local/include` and `/usr/local/lib`. You can override this with the normal auto-tools defaults. Use `./configure --help` to get details.

Note: On M1 Macs this is now `/opt/homebrew/include` and `/opt/homebrew/lib`.

Note:
libThorSerialize17.so is build using `-O3` and thus is fully optimized and debug symbols have been stripped.  
libThorSerialize17D.so is build using `-g` and is useful for debugging purposes.
libThorsLogging17.so
libThorsLogging17D.so
libThorsStorage17.so
libThorsStorage17D.so

Note: On mac it uses .dylib rather than .so.


### What is Downloaded
The configuration processes will download the generic makefiles (using git) from [ThorMaker](https://github.com/Loki-Astari/ThorMaker) which in turn will download and build [google's gtest](https://github.com/google/googletest) and [vera++](https://github.com/Loki-Astari/vera-plusplus) library that is used in running the unit tests.

## Requirements
This library uses features from C++17 so you will need a compiler that supports this. The generic makefile also does code coverage tests so your compiler will also need to support a code coverage tool that has an interface similar to `gcov`.

### What is installed:
* `/usr/local/include/ThorSerialize/*`
* `/usr/local/include/ThorsIOUtil/*`
* `/usr/local/include/ThorsStorage/*`
* `/usr/local/include/GitUtility/*`
* `/usr/local/lib/libThorSerialize17.so`
* `/usr/local/lib/libThorSerialize17D.so`
* `/usr/local/lib/libThorsStorage17.so`
* `/usr/local/lib/libThorsStorage17D.so`
* `/usr/local/lib/libyaml.so`
* `/usr/local/share/man/man3/*`



