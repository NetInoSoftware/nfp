include $(top_srcdir)/example/Makefile.inc

bin_PROGRAMS = nfp_fpm_power
nfp_fpm_power_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_fpm_power_SOURCES = app_main.c \
			     ../common/linux_sigaction.c \
			     ../common/cli_arg_parse.c

noinst_HEADERS = ../common/linux_sigaction.h \
		 ../common/cli_arg_parse.h
