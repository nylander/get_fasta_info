AC_INIT([get_fast_info], [2.2.1], [johan.nylander@nrm.se])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
: ${CFLAGS=""}
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_CHECK_HEADERS([limits.h stdlib.h unistd.h])
AX_CHECK_ZLIB
AC_OUTPUT
