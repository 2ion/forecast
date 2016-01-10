AC_INIT([forecast], [0.5.0], [dev@2ion.de])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AM_GNU_GETTEXT_VERSION([0.19.6])
AM_GNU_GETTEXT([external])
AC_PROG_CC_C99 
AC_CONFIG_SRCDIR([src/forecast.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])

#######################################################################
# macro definitions affecting control flow
#######################################################################

# Configuration file name relative to $HOME
AC_DEFINE([RCNAME], [".forecastrc"], [Configuration file name])
# Enable libfuse enhancements
AC_DEFINE([FUSE_USE_VERSION], [26], [libfuse API version used by forecastfs])

# Libraries other than GNU libc do not provide error_at_line(). If not
# available, an alternative implementation of that function is provided
# by src/lerror.c.
AC_CHECK_FUNCS([error_at_line])

AH_VERBATIM([_FORTIFY_SOURCE],
[/* Fortify source in gcc */
#if __OPTIMIZE__
#define _FORTIFY_SOURCE 2
#endif
])

#######################################################################
# libfuse detection
#######################################################################

AC_CANONICAL_BUILD
AC_MSG_CHECKING([whether the build system supports libfuse])
if expr "$build_os" : linux >/dev/null; then
AC_MSG_RESULT([yes, will also build forecastfs])
PKG_CHECK_MODULES([LIBFUSE], [fuse])
else
AC_MSG_RESULT([no])
fi
AM_CONDITIONAL([BUILD_FORECASTFS],[expr "$build_os" : linux >/dev/null])

#######################################################################
# detection of required modules
#######################################################################

PKG_CHECK_MODULES([LIBCONFIG], [libconfig])
PKG_CHECK_MODULES([LIBCRYPTO], [libcrypto])
PKG_CHECK_MODULES([LIBCURL], [libcurl])
PKG_CHECK_MODULES([LIBJSONC], [json-c])
PKG_CHECK_MODULES([LIBLZ4], [liblz4])
PKG_CHECK_MODULES([LIBNCURSESW], [ncursesw])
PKG_CHECK_MODULES([LIBTALLOC], [talloc])

#######################################################################
# Bash completion configuration
#######################################################################

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

#######################################################################
# Misc automake configuration
#######################################################################

AC_OUTPUT
