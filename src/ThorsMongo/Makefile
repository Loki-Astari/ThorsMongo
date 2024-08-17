THORSANVIL_ROOT             ?= $(realpath ../../)

TARGET                      = ThorsMongo.slib

LDLIBS_EXTERN_BUILD			= MagicEnum snappy crypto ThorSerialize yaml

UNITTEST_LDLIBS				+= $(LDLIBS_$(PLATFORM))
LDLIBS						+= $(LDLIBS_$(PLATFORM))
LDLIBS						+= -lsnappy
LINK_LIBS					+= ThorSerialize ThorsSocket ThorsLogging
LDLIBS_MSYS_NT				= -lws2_32 -lwsock32
LDLIBS_MINGW64_NT			= -lws2_32 -lwsock32

FILE_WARNING_FLAGS			+= -Wno-dollar-in-identifier-extension -Wno-deprecated-declarations -Wno-missing-braces
FILE_WARNING_FLAGS          += -Wno-type-limits

UNITTEST_FILE_WARNING_FLAGS          += -Wno-type-limits

include $(THORSANVIL_ROOT)/build/tools/Makefile
