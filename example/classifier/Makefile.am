include $(top_srcdir)/example/Makefile.inc

noinst_PROGRAMS = nfp_classifier
nfp_classifier_LDFLAGS = $(AM_LDFLAGS) -static

dist_nfp_classifier_SOURCES = classifier_main.c \
			      classifier.c \
			      classifier_term.c \
			      ../common/linux_sigaction.c \
			      ../common/cli_arg_parse.c

noinst_HEADERS = classifier.h \
		 classifier_term.h \
		../common/linux_sigaction.h \
		../common/cli_arg_parse.h
