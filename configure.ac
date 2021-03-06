#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(cceph, 0.1, [coolceph@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SUBDIRS([module/googletest/googletest])
AC_CONFIG_SUBDIRS([module/bhook])
AC_PROG_CC
CFLAGS="-g -O0 -Wall -Wextra"
CXXFLAGS="-g -O0 -Wall -Wextra"

AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PROG_LIBTOOL

# Checks for programs.
AM_PROG_CC_C_O

# Checks for programs.
AC_PROG_CXX
if test "$CXX" = no || test "$CXX:$GXX" = "g++:"; then
  AC_MSG_ERROR([no C++ compiler found])
fi

# Checks for libraries.
# check if snappy-devel is installed, needed by leveldb
AC_CHECK_LIB([snappy], [snappy_compress], [true], [AC_MSG_FAILURE([libsnappy not found])])

# check leveldb
AC_CHECK_LIB([leveldb], [leveldb_open], [true], [AC_MSG_FAILURE([libleveldb not found])], [-lsnappy -lpthread])

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([linux/types.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
  src/Makefile])


AC_OUTPUT
