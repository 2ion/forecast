AC_INIT([forecast], [0.5.0], [dev@2ion.de])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_CC_C99 

AC_CONFIG_SRCDIR([src/forecast.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_DEFINE([RCNAME], [".forecastrc"], [Configuration file name])

PKG_CHECK_MODULES([LIBJSONC], [json-c])
PKG_CHECK_MODULES([LIBCONFIG], [libconfig])
PKG_CHECK_MODULES([LIBCURL], [libcurl])
PKG_CHECK_MODULES([LIBNCURSESW], [ncursesw])
PKG_CHECK_MODULES([LIBCRYPTO], [libcrypto])
PKG_CHECK_MODULES([LZ4], [liblz4])

AH_VERBATIM([_FORTIFY_SOURCE],
[/* Fortify source in gcc */
#if __OPTIMIZE__
#define _FORTIFY_SOURCE 2
#endif
])

AC_OUTPUT
