AC_INIT([get_fast_info], [2.2.1], [johan.nylander@nrm.se])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
: ${CFLAGS=""}
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_SEARCH_LIBS(deflate, z, [], [AC_ERROR([A working zlib is required])])
AC_CHECK_HEADERS(zlib.h, [], [AC_ERROR([zlib dev is required])])
AC_OUTPUT
