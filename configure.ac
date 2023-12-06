AC_PREREQ([2.71])
AC_INIT([NetinoFastPath],[m4_esyscmd(./scripts/git_hash.sh .)],[bogdan.pricope at netinosoft.org],[NetinoFastPath])
AM_INIT_AUTOMAKE([1.9 tar-pax subdir-objects])
AC_CONFIG_SRCDIR([include/config.h.in])
AC_CONFIG_HEADERS([include/config.h])


AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O

# Use libtool
LT_INIT([])
AC_SUBST([LIBTOOL_DEPS])
LT_INIT

PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h memory.h stdint.h stdlib.h string.h sys/socket.h unistd.h])
AC_CHECK_HEADERS([sys/ioctl.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T

AC_CHECK_SIZEOF([int *])

AX_TLS

##########################################################################
# Set correct platform library version
##########################################################################
LIBNFP_VERSION=8:1:0
AC_SUBST(LIBNFP_VERSION)

##########################################################################
# Set correct pkgconfig version
##########################################################################
PKGCONFIG_VERSION=$(echo $VERSION | awk -F '.git' '{print $1}')
AC_SUBST(PKGCONFIG_VERSION)

##########################################################################
# Check for pthreads availability
##########################################################################

AX_PTHREAD([], [
	       echo "Error! We require pthreads to be available"
	       exit -1
	       ])
LIBNFP_LIBS="$PTHREAD_LIBS $LIBNFP_LIBS"
AM_CFLAGS="$AM_CFLAGS $PTHREAD_CFLAGS"
AM_LDFLAGS="$AM_LDFLAGS $PTHREAD_LDFLAGS"
CC="$PTHREAD_CC"

m4_include([./m4/visibility.m4])

##########################################################################
# Default warning setup
##########################################################################
OFP_CFLAGS="$OFP_CFLAGS -W -Wall -Wstrict-prototypes -Wmissing-prototypes"
OFP_CFLAGS="$OFP_CFLAGS -Wmissing-declarations -Wold-style-definition -Wpointer-arith"
OFP_CFLAGS="$OFP_CFLAGS -Wcast-align -Wnested-externs -Wcast-qual -Wformat-nonliteral"
OFP_CFLAGS="$OFP_CFLAGS -Wformat-security -Wundef -Wwrite-strings"
## OFP_CFLAGS="$OFP_CFLAGS -Werror"
OFP_CFLAGS="$OFP_CFLAGS -std=c11 -D_DEFAULT_SOURCE -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE"

OFP_CXXFLAGS="-std=c++11 $OFP_CXXFLAGS"
OFP_CXXFLAGS="-fno-strict-aliasing $OFP_CXXFLAGS"
OFP_CXXFLAGS="-W -Wall -Wextra $OFP_CXXFLAGS"
OFP_CXXFLAGS="-Wmissing-declarations  $OFP_CXXFLAGS"

##########################################################################
# Default include setup
##########################################################################
AM_CFLAGS="$AM_CFLAGS $OFP_CFLAGS"
AM_CXXFLAGS="$AM_CXXFLAGS $OFP_CXXFLAGS"

##########################################################################
# Checks for library functions.
##########################################################################
AC_FUNC_MALLOC
AC_CHECK_FUNCS([inet_ntoa malloc memset socket strerror strrchr])

AC_CONFIG_FILES([
	Makefile
	example/Makefile
	example/classifier/Makefile
	example/fpm/Makefile
	example/fpm_direct/Makefile
	example/fpm_power/Makefile
	example/ioctl_test/Makefile
	example/multicast/Makefile
	example/netwrap_crt/Makefile
	example/netwrap_proc/Makefile
	example/socket/Makefile
	example/tcpperf/Makefile
	example/udp_fwd_socket/Makefile
	example/udp_term_socket/Makefile
	example/udpecho/Makefile
	example/webserver/Makefile
	example/webserver2/Makefile
	example/multiprocess/Makefile
	scripts/Makefile
	test/Makefile
	])

AC_ARG_ENABLE([lto],
[AS_HELP_STRING([--enable-lto],[enable link-time optimization @<:@no@:>@])],
[want_lto="${enableval}"], [want_lto=no])
## FIXME: add a test for gcc >= 4.5.0
if test "x${want_lto}" == xyes; then
  AM_CFLAGS="$AM_CFLAGS -flto"
fi

##########################################################################
# adding the ODP library (e.g. with static name 'libodp-linux.a')
##########################################################################

# introduce the optional configure parameter for a non-standard install prefix of XXX
AC_ARG_WITH([odp],
    [AS_HELP_STRING([--with-odp=PREFIX],
	[non-standard install prefix of odp])],
    [# extend the compiler and linker flags according to the path set
     AM_CPPFLAGS="$AM_CPPFLAGS -I$with_odp/include"
     AM_LDFLAGS="$AM_LDFLAGS -L$with_odp/lib"
     export PKG_CONFIG_PATH="$with_odp/lib/pkgconfig:$PKG_CONFIG_PATH"],
    [])

# introduce optional configure parameter for odp library name
ODP_LIB=odp-linux
AC_ARG_WITH([odp-lib],
    [AS_HELP_STRING([--with-odp-lib=NAME],
	[odp library name [default=odp-linux]])],
    [ODP_LIB=$withval],
    [])

if test "x$enable_static" = "xyes" ; then
	export PKG_CONFIG="pkg-config --static"
fi

PKG_CHECK_MODULES([ODP], [lib$ODP_LIB >= 1.35.0.0], [], [])
AC_SUBST([ODP_CFLAGS])
AC_SUBST([ODP_LIBS])

PKG_CHECK_MODULES([ODPHELPER], [libodphelper], [], [])
AC_SUBST([ODPHELPER_CFLAGS])
AC_SUBST([ODPHELPER_LIBS])

AM_CPPFLAGS="$AM_CPPFLAGS $ODP_CFLAGS $ODPHELPER_CFLAGS"

# prepending lib to the files to link
LIBNFP_LIBS="$ODP_LIBS $ODPHELPER_LIBS $LIBNFP_LIBS"

AM_CPPFLAGS="$AM_CPPFLAGS -DINET -DINET6"

# Which architecture will be used
AS_CASE([$host],
  [x86*], [ARCH_TYPE=x86],
  [i686*], [ARCH_TYPE=x86],
  [aarch64*], [ARCH_TYPE=aarch64],
  [ARCH_TYPE=x86]
)
AC_SUBST([ARCH_TYPE])

LIBNFP_PATH="${ac_abs_confdir}/lib/x86/static"
if test "${ARCH_TYPE}" == "x86" ; then
    LIBNFP_PATH="${ac_abs_confdir}/lib/x86/static"
fi
if test "${ARCH_TYPE}" == "aarch64" ; then
    LIBNFP_PATH="${ac_abs_confdir}/lib/aarch64/static"
fi
AM_CONDITIONAL([NFP_AARCH64], [test x${ARCH_TYPE} == xaarch64])

# Enable/disable OFP_DEBUG
ofp_debug=no
AC_ARG_ENABLE([debug],
    [  --enable-debug         Enable OFP debug],
    [ofp_debug=$enableval])
if test "$ofp_debug" == "yes" ; then
    AM_CFLAGS="$AM_CFLAGS -DOFP_DEBUG"
fi

# Enable/disable build of power example(s).
# Power example(s) are targeting a reduce electric power consumption and
# require special ODP features
AC_ARG_ENABLE([power-example],
    [  --enable-power-example         Enable/disable build of power example(s)],
    [case "${enableval}" in
        yes) use_power=true ;;
        no)  use_power=false ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-power-example]) ;;
    esac],[use_power=false])
if test "${ARCH_TYPE}" != "x86" -a $use_power == true ; then
   echo "Warning: 'power-example' option is not supported for the selected architecture"
   use_power=false
fi

AM_CONDITIONAL([NFP_POWER], [test x$use_power = xtrue])
AM_COND_IF([NFP_POWER], [AM_CPPFLAGS="$AM_CPPFLAGS -DNFP_POWER"])
AM_COND_IF([NFP_POWER], [LIBNFP_PATH="${ac_abs_confdir}/lib/x86_power/static"])

##########################################################################
# NFP library name and path
##########################################################################

AC_ARG_WITH(
	[nfp-path],
	AC_HELP_STRING(
		[--with-nfp-path=PATH],
		[nfp library path]),
	[LIBNFP_PATH=$withval])
AC_SUBST([LIBNFP_PATH])

LIBNFP_NAME="nfp"
AC_ARG_WITH(
        [nfp-lib],
        AC_HELP_STRING(
                [--with-nfp-lib=NAME],
                [nfp library name [default=nfp]]),
        [LIBNFP_NAME=$withval])
AC_SUBST([LIBNFP_NAME])

##########################################################################
# adding the RumpKernel dir
##########################################################################

# introduce the optional configure parameter for a non-standard install prefix of XXX
AC_ARG_WITH([buildrump],
    [AS_HELP_STRING([--with-buildrump=prefix],
	[non-standard path prefix of the rump source directory])],
    [RUMPPATHSET=1],
    [RUMPPATHSET=0])

# if optional parameter used, extend path flags for compliler and linker
if test $RUMPPATHSET = 1 ; then
    # extend the compiler and linker flags according to the path set
    AM_CPPFLAGS="$AM_CPPFLAGS -I$with_buildrump/rump/include"
    AM_LDFLAGS="$AM_LDFLAGS -L$with_buildrump/rump/lib/"
    AC_DEFINE(HAVE_BUILDRUMP, [1], [Have buildrump libs ?])
    LIBNFP_LIBS="-lrumpclient -lrumpnet_shmif -lrumpnet_config -lrumpnet_netinet6 -lrumpnet_net -lrumpnet -lrump -lrumpuser $LIBNFP_LIBS"
else
    AC_CHECK_HEADERS([rump/rump.h], [
		    AC_DEFINE(HAVE_BUILDRUMP, [1], [Have buildrump libs ?])],
		    [])
    AC_SEARCH_LIBS([rumpclient_init],[rumpclient])
    AC_SEARCH_LIBS([rump_pub_shmif_create],[rumpnet_shmif])
    AC_SEARCH_LIBS([rumpns_if_init],[rumpnet_config])
    AC_SEARCH_LIBS([_init],[rumpnet_netinet6])
    AC_SEARCH_LIBS([_init],[rumpnet_netinet])
    AC_SEARCH_LIBS([rumpns_rt_init],[rumpnet_net])
    AC_SEARCH_LIBS([rumpns_rt_inithead],[rumpnet])
    AC_SEARCH_LIBS([rump_init],[rump])
    AC_SEARCH_LIBS([rumpuser_init],[rumpuser])
fi

##########################################################################
# distribute the changed variables among the Makefiles
AC_SUBST([LIBNFP_LIBS])
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_OUTPUT
AC_MSG_RESULT([
	$PACKAGE $VERSION
	========

	Library version:	${LIBNFP_VERSION}

	build:                  ${build}
	host:                   ${host}
	prefix:			${prefix}
	sysconfdir:		${sysconfdir}
	libdir:			${libdir}
	includedir:		${includedir}

	cc:                     ${CC}
        cppflags:               ${CPPFLAGS}
        am_cppflags:            ${AM_CPPFLAGS}
	cflags:			${CFLAGS}
	am_cflags:		${AM_CFLAGS}
        am_cxxflags:            ${AM_CXXFLAGS}
	ldflags:		${LDFLAGS}
	am_ldflags:		${AM_LDFLAGS}
	odp_library:		${ODP_LIB}
	odp_version:		$(pkg-config --modversion lib$ODP_LIB)
	odp_cflags:		${ODP_CFLAGS}
	odphelper_cflags:	${ODPHELPER_CFLAGS}
	odphelper_libs:		${ODPHELPER_LIBS}
	nfp_library:		${LIBNFP_NAME}
	nfp_path:		${LIBNFP_PATH}
	nfp_libs:		${LIBNFP_LIBS}
])
