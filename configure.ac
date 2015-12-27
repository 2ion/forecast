AC_INIT([forecast], [0.5.0], [dev@2ion.de])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_CC_C99 

AC_CONFIG_SRCDIR([src/forecast.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_DEFINE([RCNAME], [".forecastrc"], [Configuration file name])
AC_DEFINE([FUSE_USE_VERSION], [26], [libfuse API version used by forecastfs])

PKG_CHECK_MODULES([LIBCONFIG], [libconfig])
PKG_CHECK_MODULES([LIBCRYPTO], [libcrypto])
PKG_CHECK_MODULES([LIBCURL], [libcurl])
PKG_CHECK_MODULES([LIBFUSE], [fuse])
PKG_CHECK_MODULES([LIBJSONC], [json-c])
PKG_CHECK_MODULES([LIBLZ4], [liblz4])
PKG_CHECK_MODULES([LIBNCURSESW], [ncursesw])
PKG_CHECK_MODULES([LIBTALLOC], [talloc])

# Deal with installing Bash completion functions
AC_ARG_WITH([bash-completion-dir],
            AS_HELP_STRING([--with-bash-completion-dir[=PATH]], [Install the Bash auto-completion script into the given directory @<:@default=yes@:>@]),
            [],
            [with_bash_completion_dir=yes])

if test "x$with_bash_completion_dir" = "xyes"; then
PKG_CHECK_MODULES([BASH_COMPLETION], [bash-completion >= 2.0],
                  [BASH_COMPLETION_DIR="`pkg-config --variable=completionsdir bash-completion`"],
                  [BASH_COMPLETION_DIR="$datadir/bash-completion/completions"])
else
  BASH_COMPLETION_DIR="$with_bash_completion_dir"
fi
AC_SUBST([BASH_COMPLETION_DIR])
AM_CONDITIONAL([ENABLE_BASH_COMPLETION],[test "x$with_bash_completion_dir" != "xno"])

AH_VERBATIM([_FORTIFY_SOURCE],
[/* Fortify source in gcc */
#if __OPTIMIZE__
#define _FORTIFY_SOURCE 2
#endif
])

AC_OUTPUT
