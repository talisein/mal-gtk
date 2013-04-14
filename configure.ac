AC_PREREQ([2.68])
AC_INIT([mal-gtk], [0.0.1], [agpotter@gmail.com])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_MACRO_DIR([m4/])

AM_INIT_AUTOMAKE
#AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_11([noext])

# Checks for libraries.

PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 >= 2.32.0])
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.4.0])
PKG_CHECK_MODULES([CURL], [libcurl >= 7.24.0])
PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.7.8])
PKG_CHECK_MODULES([LIBSECRET], [libsecret-1 >= 0.12])

# To use C++11 lambdas with sigc++:
PKG_CHECK_MODULES([SIGCPP], [sigc++-2.0 >= 2.2.11])

AC_SUBST([GLIBMM_LIBS])
AC_SUBST([GLIBMM_CFLAGS])
AC_SUBST([GTKMM_LIBS])
AC_SUBST([GTKMM_CFLAGS])
AC_SUBST([CURL_LIBS])
AC_SUBST([CURL_CFLAGS])
AC_SUBST([LIBXML_LIBS])
AC_SUBST([LIBXML_CFLAGS])
AC_SUBST([LIBSECRET_CFLAGS])
AC_SUBST([LIBSECRET_LIBS])

#GLIB_GSETTINGS

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
