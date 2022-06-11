DEFAULT_INCLUDES=-I.

AM_CPPFLAGS += \
	     $(DEFAULT_INCLUDES) \
	     -I$(srcdir) \
	     -I$(top_srcdir)/include/api \
	     -I$(top_srcdir)/example/common

LIBS  += -L$(LIBNFP_PATH) -l$(LIBNFP_NAME)  $(LIBNFP_LIBS)
