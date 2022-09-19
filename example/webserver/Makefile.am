include $(top_srcdir)/example/Makefile.inc

bin_PROGRAMS = nfp_webserver

nfp_webserver_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_webserver_SOURCES = app_main.c httpd.c \
			 ../common/linux_sigaction.c \
			 ../common/linux_resources.c \
			../common/cli_arg_parse.c

noinst_HEADERS = ${srcdir}/httpd.h \
		 ../common/linux_sigaction.h \
		 ../common/linux_resources.h \
		 ../common/cli_arg_parse.h
