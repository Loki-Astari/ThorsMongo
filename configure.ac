AC_INIT([ThorsMongo], [0.1], [YourName+ThorsMongo@Email.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsMongo], [src/ThorsMongo/ThorsMongo.cpp], [20])

AX_THOR_OS_APP_CHECK([LINUX], [lsb_release], [
    The class "Client" uses lsb-release to detect architecture.
    Please install:
        apt-get install lsb_release
])

AX_THOR_FEATURE_HEADER_ONLY_VARIANT([THORS_MONGO])

#
# Add extra configuration here
#
AX_THOR_ENABLE_CONAN
AX_THOR_CHECK_USE_SNAPPY
AX_THOR_CHECK_USE_CRYPTO
AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_THORS_SERIALIZE_HEADER_ONLY
AX_THOR_CHECK_USE_THORS_SERIALIZE([ThorSerialize ThorsSocket ThorsLogging])

AX_THOR_DISABLE_TEST_REQUIREING_MONGO_QUERY
AX_THOR_SERVICE_AVAILABLE_MONGO

AX_THOR_FUNC_TEST_COMP_ACTION([[
#include <charconv>
int main() {
    char    buffer[100];
    double  value = 12.345;
    auto result = std::to_chars(buffer, buffer+100, value, std::chars_format::fixed, 6);
}]],
    [
        AC_MSG_NOTICE([[Supports std::to_chars(begin, end, double)]])
    ],
    [
        AC_MSG_NOTICE([Setting NO_STD_SUPPORT_TO_CHAR_DOUBLE])
        AC_DEFINE([NO_STD_SUPPORT_TO_CHAR_DOUBLE], [1], [Check for older build tools and use simpler code with old tools])
    ]
)
AX_THOR_FUNC_TEST_COMP_ACTION([[
#include <charconv>
int main() {
    char    buffer[] = "12.345";
    double  value;
    auto result = std::from_chars(buffer, buffer+100, value);
}]],
    [
        AC_MSG_NOTICE([[Supports std::from_chars(begin, end, double)]])
    ],
    [
        AC_MSG_NOTICE([Setting NO_STD_SUPPORT_FROM_CHAR_DOUBLE])
        AC_DEFINE([NO_STD_SUPPORT_FROM_CHAR_DOUBLE], [1], [Check for older build tools and use simpler code with old tools])
    ]
)
LT_INIT

AX_THOR_FUNC_TERM_BUILD([THORSMONGO], [src/ThorsMongo/ThorsMongoConfig.h:config.h.in])

AC_OUTPUT
