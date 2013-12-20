# Process this file with autoconf to produce a configure script.
AC_INIT(epple2, 1.2-SNAPSHOT)
AC_PREREQ(2.69)

AC_CONFIG_SRCDIR([src/apple2.cpp])

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE



# Checks for programs.
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB



# Checks for libraries.
AC_CHECK_LIB([SDL2],[SDL_Init],,[AC_MSG_ERROR([cannot find libsdl])])



# Checks for header files.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_HEADERS([windows.h])
AC_CHECK_HEADER([windows.h],[WINDOWS=true].[WINDOWS=])
AM_CONDITIONAL([HAVE_WINDOWS],test "$WINDOWS")

# Checks for library functions.
AC_HEADER_STDC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memset sqrt])



# Checks for typedefs, structures, and compiler characteristics.



AC_CONFIG_FILES([Makefile src/Makefile conf/Makefile installer/Makefile])
AC_OUTPUT
