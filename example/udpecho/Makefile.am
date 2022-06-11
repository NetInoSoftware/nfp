include $(top_srcdir)/example/Makefile.inc

bin_PROGRAMS = nfp_udpecho

nfp_udpecho_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_udpecho_SOURCES = app_main.c udp_server.c \
		       ../common/linux_sigaction.c \
		       ../common/linux_resources.c \
		       ../common/cli_arg_parse.c

noinst_HEADERS = ${srcdir}/udp_server.h \
		 ../common/linux_sigaction.h \
		 ../common/linux_resources.h \
		 ../common/cli_arg_parse.h
