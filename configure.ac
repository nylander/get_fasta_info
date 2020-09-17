AC_PREREQ([2.69])
AC_INIT([get_fast_info], [2.2.1], [johan.nylander@nrm.se])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
: ${CFLAGS=""}
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CPP
AC_PROG_MKDIR_P
AC_CHECK_LIB([m], [roundf])
AC_CHECK_LIB([z], [gzgetc], [AC_ERROR([zlib dev (e.g. zlib1g-dev) is required])])
#AC_CHECK_HEADERS(zlib.h, [], [AC_ERROR([zlib dev is required])])
AC_CHECK_HEADERS([limits.h stdlib.h unistd.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
