include $(top_srcdir)/example/Makefile.inc

noinst_PROGRAMS = nfp_tcpperf
nfp_tcpperf_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_tcpperf_SOURCES = app_main.c \
			   ../common/cli_arg_parse.c

noinst_HEADERS = ../common/cli_arg_parse.h
