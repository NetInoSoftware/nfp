LIB   = $(top_builddir)/lib

AM_CPPFLAGS +=  -I.
AM_CPPFLAGS +=  -I$(top_srcdir)/include/api
AM_CPPFLAGS +=  -I../common

lib_LTLIBRARIES = $(LIB)/libnfp_netwrap_proc.la

__LIB__libnfp_netwrap_proc_la_LDFLAGS = -shared \
	-version-number '$(LIBNFP_VERSION)'

__LIB__libnfp_netwrap_proc_la_SOURCES = app_main.c \
	 ../common/cli_arg_parse.c

