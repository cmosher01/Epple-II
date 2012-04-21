# Process this file with autoconf to produce a configure script.
AC_INIT(src/apple2.cpp)
AC_PREREQ(2.59)

AC_CONFIG_SRCDIR([config.h.in])

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(epple2, 1.0)



# Checks for programs.
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([SDL],[SDL_Init],,[AC_MSG_ERROR([cannot find libsdl])])
AC_CHECK_LIB([X11],[XOpenDisplay],,[AC_MSG_ERROR([cannot find X11])])



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
