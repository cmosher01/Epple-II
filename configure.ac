# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT(epple2, 1.2-SNAPSHOT)
AC_PREREQ(2.68)

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
AC_PATH_PROGS([XA65],[xa65 xa])
if test "X$ac_cv_path_XA65" == X
then
    AC_MSG_ERROR([Cannot find xa (xa65) 6502 cross assembler by Andre Fachat])
fi
AC_ARG_VAR([XA65],[path to xa (xa65) 6502 cross assembler by Andre Fachat])

AC_PATH_PROG([LDO65],[ldo65])
if test "X$ac_cv_path_LDO65" == X
then
    AC_MSG_ERROR([Cannot find ldo65, from xa (xa65) 6502 cross assembler by Andre Fachat])
fi
AC_ARG_VAR([LDO65],[path to ldo65, from xa (xa65) 6502 cross assembler by Andre Fachat])

AC_PATH_PROG([RELOC65],[reloc65])
if test "X$ac_cv_path_RELOC65" == X
then
    AC_MSG_ERROR([Cannot find reloc, from xa (xa65) 6502 cross assembler by Andre Fachat])
fi
AC_ARG_VAR([RELOC65],[path to reloc65, from xa (xa65) 6502 cross assembler by Andre Fachat])

AC_PATH_PROG([M4],[m4])
if test "X$ac_cv_path_M4" == X
then
    AC_MSG_ERROR([Cannot find m4])
fi
AC_ARG_VAR([M4],[path to m4])

AC_PATH_PROG([MD5SUM],[md5sum])
AC_ARG_VAR([MD5SUM],[path to md5sum])



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



AC_CONFIG_FILES([Makefile src/Makefile conf/Makefile rom/Makefile installer/Makefile doc/Makefile])
AC_OUTPUT
