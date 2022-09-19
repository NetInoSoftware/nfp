include $(top_srcdir)/example/Makefile.inc

bin_PROGRAMS = nfp_multiprocess
nfp_multiprocess_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_multiprocess_SOURCES = \
multiprocess_main.c \
../common/cli_arg_parse.c \
../common/linux_sigaction.c

noinst_HEADERS = \
../common/cli_arg_parse.h

#		${srcdir}/multiprocess.h
