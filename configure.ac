AC_INIT([ThorsMongo], [0.1], [YourName+ThorsMongo@Email.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsMongo], [src/ThorsMongo/ThorsMongo.cpp], [20])


#
# Add extra configuration here
#
AX_THOR_CHECK_USE_SNAPPY
AX_THOR_CHECK_USE_CRYPTO
AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_THORS_SERIALIZE_HEADER_ONLY
AX_THOR_CHECK_USE_THORS_SERIALIZE

AX_THOR_SERVICE_AVAILABLE_MONGO

LT_INIT

AX_THOR_FUNC_TERM_BUILD([ThorsMongo], [src/ThorsMongo/ThorsMongoConfig.h])

AC_OUTPUT
