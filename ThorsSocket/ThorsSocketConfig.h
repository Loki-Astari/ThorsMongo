/* src/ThorsSocket/ThorsSocketConfig.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */



#ifndef  THORS_SOCKET_CONFIG_H
#define  THORS_SOCKET_CONFIG_H
#ifndef  THORS_PACKAGE_INFO_CONFIG_H
#define  THORS_PACKAGE_INFO_CONFIG_H

    

/* EWOULDBLOCK is defined and different from EAGAIN */
#define HAS_UNIQUE_EWOULDBLOCK 0

/* We have found package CRYPTO */
/* #undef HAVE_CRYPTO */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* We have found package Thors Serializer */
#define HAVE_THORSSERIALIZER 1

/* We have found ThorSerializeHeaderOnly package */
/* #undef HAVE_ThorSerializeHeaderOnly */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* We have found package ZLIB */
/* #undef HAVE_ZLIB */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "thorssocket"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Loki.Astari+ThorsSocket@gmail.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ThorsSocket"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ThorsSocket 3.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "thorssocket"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.0"

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Enable to use header only libraries */

/* For header only convert to inline */

/* Set up to build on Conan */
#define THOR_CONAN 0

/* Disable integration tests */
#define THOR_DISABLE_TEST_WITH_INTEGRATION 0

/* Name of package for header files */
#define THOR_PACKAGE_NAME ThorsSocket

/* Version number of package */
#define VERSION "3.0"




    
#endif


/* Enable to use header only libraries */
#define THORS_SOCKET_HEADER_ONLY 1

/* For header only convert to inline */
#define THORS_SOCKET_HEADER_ONLY_INCLUDE inline


#endif