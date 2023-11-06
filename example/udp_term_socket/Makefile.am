include $(top_srcdir)/example/Makefile.inc

bin_PROGRAMS = nfp_udp_term_socket

nfp_udp_term_socket_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_udp_term_socket_SOURCES = app_main.c udp_term_socket.c \
			      ../common/linux_sigaction.c \
			      ../common/cli_arg_parse.c

noinst_HEADERS = ${srcdir}/udp_term_socket.h \
		 ../common/linux_sigaction.h \
		 ../common/cli_arg_parse.h
