SUBDIRS = .

AM_CPPFLAGS += -I$(top_srcdir)/include/api

LIBS  += -L$(LIBNFP_PATH) -l$(LIBNFP_NAME) $(LIBNFP_LIBS)

noinst_PROGRAMS = nfp_cpp_test nfp_redefinitions_posix_test
nfp_cpp_test_LDFLAGS = $(AM_LDFLAGS) -static
nfp_redefinitions_posix_test_LDFLAGS = $(AM_LDFLAGS) -static

TESTS = ${noinst_PROGRAMS}

nfp_cpp_test_SOURCES = nfp_cpp_test.cpp
nfp_redefinitions_posix_test_SOURCES = nfp_redefinitions_posix_test.c
