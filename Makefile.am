ACLOCAL_AMFLAGS=-I m4
AUTOMAKE_OPTIONS = foreign
SUBDIRS = scripts example test

include_HEADERS = \
		$(top_srcdir)/include/api/nfp.h \
		$(top_srcdir)/include/api/nfp_config.h \
		$(top_srcdir)/include/api/nfp_init.h \
		$(top_srcdir)/include/api/nfp_pkt_processing.h \
		$(top_srcdir)/include/api/nfp_api_cli.h \
		$(top_srcdir)/include/api/nfp_thread_proc.h \
		$(top_srcdir)/include/api/nfp_log.h \
		$(top_srcdir)/include/api/nfp_timer.h \
		$(top_srcdir)/include/api/nfp_types.h \
		$(top_srcdir)/include/api/nfp_socket_types.h \
		$(top_srcdir)/include/api/nfp_socket_sigevent.h \
		$(top_srcdir)/include/api/nfp_socket.h \
		$(top_srcdir)/include/api/nfp_in.h \
		$(top_srcdir)/include/api/nfp_in6.h \
		$(top_srcdir)/include/api/nfp_errno.h \
		$(top_srcdir)/include/api/nfp_hook.h \
		$(top_srcdir)/include/api/nfp_route_arp.h \
		$(top_srcdir)/include/api/nfp_debug.h \
		$(top_srcdir)/include/api/nfp_stat.h \
		$(top_srcdir)/include/api/nfp_ioctl.h \
		$(top_srcdir)/include/api/nfp_queue.h \
		$(top_srcdir)/include/api/nfp_sysctl.h \
		$(top_srcdir)/include/api/nfp_utils.h \
		$(top_srcdir)/include/api/nfp_ethernet.h \
		$(top_srcdir)/include/api/nfp_ip.h \
		$(top_srcdir)/include/api/nfp_ip6.h \
		$(top_srcdir)/include/api/nfp_icmp.h \
		$(top_srcdir)/include/api/nfp_icmp6.h \
		$(top_srcdir)/include/api/nfp_ifnet.h \
		$(top_srcdir)/include/api/nfp_ifnet_portconf.h \
		$(top_srcdir)/include/api/nfp_if_vlan.h \
		$(top_srcdir)/include/api/nfp_udp.h \
		$(top_srcdir)/include/api/nfp_ip_var.h \
		$(top_srcdir)/include/api/nfp_tcp.h \
		$(top_srcdir)/include/api/nfp_epoll.h \
		$(top_srcdir)/include/api/nfp_ipsec.h \
		$(top_srcdir)/include/api/nfp_ipsec_init.h \
		$(top_srcdir)/include/api/nfp_print.h \
		$(top_srcdir)/include/api/nfp_time.h \
		$(top_srcdir)/include/api/nfp_select.h \
		$(top_srcdir)/include/api/nfp_pkt_channel.h \
		$(top_srcdir)/include/api/nfp_pkt_vector.h \
		$(top_srcdir)/include/api/nfp_ifaddrs.h \
		$(top_srcdir)/include/api/nfp_netdb.h \
		$(top_srcdir)/include/api/nfp_dns.h \
		$(top_srcdir)/include/api/nfp_dns_common.h \
		$(top_srcdir)/include/api/nfp_nd6.h

confdir = $(prefix)/config
conf_DATA = \
	    $(top_srcdir)/config/README \
	    $(top_srcdir)/config/nfp_flv_default.conf \
	    $(top_srcdir)/config/nfp_flv_netwrap_default.conf \
	    $(top_srcdir)/config/nfp_flv_netwrap_webserver.conf \
	    $(top_srcdir)/config/nfp_flv_webserver.conf \
	    $(top_srcdir)/config/nfp_flv_memory_footprint.conf

scriptsdir = $(prefix)/scripts
scripts_DATA = \
	       $(top_srcdir)/scripts/nfp_netwrap.cli \
	       $(top_srcdir)/scripts/nfp.cli

binscriptdir = $(prefix)/scripts
binscript_SCRIPTS = \
	      $(top_srcdir)/scripts/nfp_linux_interface_acquire.sh \
              $(top_srcdir)/scripts/nfp_linux_interface_release.sh \
              $(top_srcdir)/scripts/nfp_netwrap.sh

docsdir = $(prefix)/docs
docs_DATA = \
	    $(top_srcdir)/docs/nfp-user-guide.adoc \
	    $(top_srcdir)/docs/nfp-user-guide.html \
	    $(top_srcdir)/docs/NFP_performance_results.odt \
	    $(top_srcdir)/docs/OpenFastPath_master_deck.pdf

libnfpdir = $(prefix)/lib

if NFP_AARCH64 
libnfp_DATA = \
	$(top_srcdir)/lib/aarch64/static/libnfp.a \
	$(top_srcdir)/lib/aarch64/shared/libnfp.so \
	$(top_srcdir)/lib/aarch64/shared/libnfp.so.8 \
	$(top_srcdir)/lib/aarch64/shared/libnfp.so.8.1.0

else
if NFP_POWER
libnfp_DATA = \
	$(top_srcdir)/lib/x86_power/static/libnfp.a \
	$(top_srcdir)/lib/x86_power/shared/libnfp.so \
	$(top_srcdir)/lib/x86_power/shared/libnfp.so.8 \
	$(top_srcdir)/lib/x86_power/shared/libnfp.so.8.1.0
else
libnfp_DATA = \
	$(top_srcdir)/lib/x86/static/libnfp.a \
	$(top_srcdir)/lib/x86/shared/libnfp.so \
	$(top_srcdir)/lib/x86/shared/libnfp.so.8 \
	$(top_srcdir)/lib/x86/shared/libnfp.so.8.1.0
endif
endif

EXTRA_DIST = bootstrap .scmversion $(conf_DATA) $(scripts_DATA) $(docs_DATA) $(libnfp_DATA)
