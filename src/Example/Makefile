
CC				= $(CXX)
SRC				= $(wildcard *.cpp)
OBJ				= $(subst %.cpp, %.o, $(SRC))

SNAPPY_ROOT		= /opt/homebrew
MAGIC_ENUM_ROOT	= /opt/homebrew
THORS_ROOT		= ../../../../build

CXXFLAGS		+= -Wall -Wextra
CXXFLAGS		+= -std=c++20
CXXFLAGS		+= -I $(THORS_ROOT)/include -I $(SNAPPY_ROOT)/include -I$(MAGIC_ENUM_ROOT)/include
LDFLAGS			+= -L $(THORS_ROOT)/lib -L $(SNAPPY_ROOT)/lib
LDLIBS			+= -lThorsMongo -lThorSerialize -lThorsSocket -lThorsLogging -lsnappy

all:			Example

Example:		$(OBJ)

