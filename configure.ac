# Initialize autoconf and automake
AC_PREREQ([2.65])
AC_INIT(src/main.cpp)
AM_INIT_AUTOMAKE(PieDock, 1.4.1)

# Determine default prefix
test x$prefix = "xNONE" && prefix="$ac_default_prefix"

AC_LANG_CPLUSPLUS

# Checks for programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for mandatory libraries
AC_CHECK_LIB([X11], [XOpenDisplay], , AC_MSG_ERROR([libX11 not found]))
AC_CHECK_LIB([png], [png_create_read_struct], , AC_MSG_ERROR([libpng not found]))
AC_CHECK_LIB([z], [deflate], , AC_MSG_ERROR([libz not found]))

# Checks for Xft
XFT=false

AC_MSG_CHECKING([whether to have Xft support])
AC_ARG_ENABLE(
  xft,
[  --enable-xft        Xft support [default=yes]],
	if test x$enableval = "xyes"; then
		AC_MSG_RESULT([yes])
		AC_CHECK_LIB(Xft, XftFontOpen,
			AC_DEFINE(HAVE_XFT, 1, "Xft support")
			LIBS="$LIBS -lXft -lfreetype"
			XFT=true)
		AC_CHECK_LIB(freetype, FT_Init_FreeType,
			LIBS="$LIBS -lfreetype")
	else
		AC_MSG_RESULT([no])
	fi,
	AC_MSG_RESULT([yes])
	AC_CHECK_LIB(Xft, XftFontOpen,
		AC_DEFINE(HAVE_XFT, 1, "Xft support")
		LIBS="$LIBS -lXft"
		XFT=true)
	AC_CHECK_LIB(freetype, FT_Init_FreeType,
		LIBS="$LIBS -lfreetype")
)

# Checks for Freetype
if test "x$XFT" = "xtrue"; then
	AC_PATH_PROG(FREETYPE_CONFIG, freetype-config, no)

	if test x$FREETYPE_CONFIG = xno; then
		AC_MSG_ERROR([*** freetype-config not found])
	fi

	FREETYPE_CFLAGS="`$FREETYPE_CONFIG --cflags`"
	FREETYPE_LIBS="`$FREETYPE_CONFIG --libs`"
	CXXFLAGS="$CXXFLAGS $FREETYPE_CFLAGS"
fi

# Checks for Xrender
AC_MSG_CHECKING([whether to have Xrender support])
AC_ARG_ENABLE(
  xrender,
[  --enable-xrender        Xrender (transparent) support [default=yes]],
	if test x$enableval = "xyes"; then
		AC_MSG_RESULT([yes])
		AC_CHECK_LIB(Xrender, XRenderCreatePicture,
			AC_DEFINE(HAVE_XRENDER, 1, "Xrender support")
			LIBS="$LIBS -lXrender")
	else
		AC_MSG_RESULT([no])
	fi,
	AC_MSG_RESULT([yes])
	AC_CHECK_LIB(Xrender, XRenderCreatePicture,
		AC_DEFINE(HAVE_XRENDER, 1, "Xrender support")
		LIBS="$LIBS -lXrender")
)

# Check for libXmu
AC_MSG_CHECKING([whether to have libXmu])
AC_ARG_ENABLE(
  xmu,
[  --enable-xmu        use libXmu [default=yes]],
	if test x$enableval = "xyes"; then
		AC_MSG_RESULT([yes])
		AC_CHECK_LIB(Xmu, XmuClientWindow,
			AC_DEFINE(HAVE_XMU, 1, "libXmu support")
			LIBS="$LIBS -lXmu")
	else
		AC_MSG_RESULT([no])
	fi,
	AC_MSG_RESULT([yes])
	AC_CHECK_LIB(Xmu, XmuClientWindow,
		AC_DEFINE(HAVE_XMU, 1, "libXmu support")
		LIBS="$LIBS -lXmu")
)

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero floor memset pow select socket sqrt strcasecmp strcspn strdup strerror strstr])

# Print results
AC_MSG_RESULT([])
AC_MSG_RESULT([$PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
AC_MSG_RESULT([Using '$prefix' for installation.])
AC_MSG_RESULT([Using '$CXX' for C++ compiler.])
AC_MSG_RESULT([Building with '$CXXFLAGS' for C++ compiler flags.])
AC_MSG_RESULT([Building with '$LIBS' for linker flags.])
AC_MSG_RESULT([])

AC_CONFIG_FILES([Makefile src/Makefile utils/Makefile])
AC_OUTPUT
