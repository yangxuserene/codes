#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([codes], [0.5.2], [http://trac.mcs.anl.gov/projects/codes/newticket],[],[http://www.mcs.anl.gov/projects/codes/])

AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([doc/BUILD_STEPS])
AC_CONFIG_HEADERS([codes_config.h])

AX_PROG_BISON([],[AC_MSG_WARN([Could not find bison])])
AX_PROG_FLEX([],[AC_MSG_WARN([Could not find flex])])
AC_SUBST([BISON])
AC_SUBST([FLEX])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_RANLIB

PKG_PROG_PKG_CONFIG

if test -z "$PKG_CONFIG" ; then
    AC_MSG_ERROR([pkg-config is required. Please install the pkg-config program on your PATH or set the PKG_CONFIG environment variable to the appropriate package.])
fi

# Check for C99
AC_PROG_CC_C99

AC_REQUIRE_CPP

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h execinfo.h pthread.h malloc.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Add warning flags by default
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS="$CFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS="$CFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([-Wshadow], [CFLAGS="$CFLAGS -Wshadow"])

# Checks for library functions.
AC_CHECK_FUNCS([memset])
AC_CHECK_LIB([pthread],[pthread_create],,[AC_MSG_ERROR([Could not find pthread_create!])])
AC_CHECK_LIB([m],[sqrt],,[AC_MSG_ERROR([Could not find sqrt!])])


AX_PROG_BISON_CLFEATURES([],[AC_MSG_WARN([Could not find bison])],
[bison_ok="yes"], [bison_ok="no"])
AC_SUBST([bison_ok])

dnl Check to see if CC is an MPI compiler
AC_MSG_CHECKING(whether the mpicc compiler works)
AC_TRY_COMPILE([#include <mpi.h>], [int ret = MPI_Init(0, (void*)0)],
    AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no)
    AC_MSG_ERROR(CC doesnt appear to be a valid MPI compiler.  See INSTALL document or try adding CC=mpicc to your configure command line.)
)

# check for ROSS
PKG_CHECK_MODULES_STATIC([ROSS], [ross], [],
                  [AC_MSG_ERROR([Could not find working ross installation via pkg-config])])

# check for Darshan
AC_ARG_WITH([darshan],[AS_HELP_STRING([--with-darshan],
                        [Build with the darshan workload support])],
                      [use_darshan=yes],[use_darshan=no])
if test "x${use_darshan}" = xyes ; then
    PKG_CHECK_MODULES_STATIC([DARSHAN], [darshan-util], [],
                      [AC_MSG_ERROR([Could not find working darshan installation via pkg-config])])
    DARSHAN_VER=`pkg-config --modversion darshan-util`
    AX_COMPARE_VERSION([$DARSHAN_VER],[ge],[2.3],[],
        [AC_MSG_ERROR([Found Darshan $DARSHAN_VER but 2.3 or greater is needed])])
fi
AM_CONDITIONAL(USE_DARSHAN, [test "x${use_darshan}" = xyes])


# check for Recorder
AM_CONDITIONAL(USE_RECORDER, true)
RECORDER_CPPFLAGS="-DUSE_RECORDER=1"
AC_SUBST(RECORDER_CPPFLAGS)

#check for Dumpi
AC_ARG_WITH([dumpi],[AS_HELP_STRING([--with-dumpi@<:@=DIR@:>@],
                        [location of Dumpi installation])])
if test "x${with_dumpi}" != "x" ; then
	AC_CHECK_FILE([${with_dumpi}/lib/libundumpi.la],
                  AM_CONDITIONAL(USE_DUMPI, true),
                  AC_MSG_ERROR(Could not find libundumpi.la))
	DUMPI_CFLAGS="-I${with_dumpi}/include"
#	DUMPI_CFLAGS+=" -I${with_dumpi}/include/dumpi/common" 
#	DUMPI_CFLAGS+=" -I${with_dumpi}/include/dumpi/libdumpi"
#	DUMPI_CFLAGS+=" -I${with_dumpi}/include/dumpi/libundumpi"
	DUMPI_LIBS="-L${with_dumpi}/lib/ -lundumpi"
	AC_SUBST(DUMPI_LIBS)
	AC_SUBST(DUMPI_CFLAGS)
else
	AM_CONDITIONAL(USE_DUMPI, false)
fi

dnl ======================================================================
dnl  Try harder to be valgrind safe
dnl ======================================================================
AC_ARG_ENABLE(valgrind-clean,
        [AS_HELP_STRING(
                [--enable-valgrind-clean],
                [Try harder to avoid valgrind warnings])
        ])

AS_IF([test "x$enable_valgrind_clean" = "xyes"], [
      AC_DEFINE([VALGRIND], [1], [If enabling valgrind-clean build])
])


dnl AC_CONFIG_FILES([src/iokernellang/codesparser.y])
if test "x$srcdir" != "x."; then
    AC_CONFIG_LINKS([tests/conf:$srcdir/tests/conf])
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT([maint/codes.pc])
