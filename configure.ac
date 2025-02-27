AC_PREREQ([2.71])
AC_INIT([get_fasta_info],[2.4.1],[johan.nylander@nrm.se])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CHECK_LIB([z], [gzopen], [], [AC_MSG_ERROR([zlib library not found])])
AC_CHECK_LIB([m], [lround], [], [AC_MSG_ERROR([math library not found])])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
