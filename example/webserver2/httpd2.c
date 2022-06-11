#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "nfp.h"
#include "httpd.h"

/* Set www_dir to point to your web directory. */
static const char *www_dir;
static __thread char bufo_in[512];
static __thread char bufo_out[1024];

/* Sending function with some debugging. */
static int mysend(int s, char *p, int len)
{
	int n;

	while (len > 0) {
		n = nfp_send(s, p, (nfp_size_t)len, 0);
		if (n < 0) {
			NFP_ERR("nfp_send failed n=%d, err='%s'",
				  n, nfp_strerror(nfp_errno));
			return n;
		}
		len -= n;
		p += n;
		if (len)
			NFP_WARN("Only %d bytes sent", n);
	}
	return len;
}

static int sendf(int fd, const char *fmt, ...)
{
	int ret, n;
	va_list ap;

	va_start(ap, fmt);
	n = vsnprintf(bufo_out, sizeof(bufo_out), fmt, ap);
	va_end(ap);
	ret = mysend(fd, bufo_out, n);
	return ret;
}

/* Send one file. */
static void get_file(int s, char *url)
{
	int n, w;

	const char *mime = NULL;
	const char *p = url;
	char *p2;

	if (*p == 0)
		p = "index.html";

	p2 = strrchr(p, '.');
	if (p2) {
		p2++;
		if (!strcmp(p2, "html"))
			mime = "text/html";
		else if (!strcmp(p2, "htm"))
			mime = "text/html";
		else if (!strcmp(p2, "css"))
			mime = "text/css";
		else if (!strcmp(p2, "txt"))
			mime = "text/plain";
		else if (!strcmp(p2, "png"))
			mime = "image/png";
		else if (!strcmp(p2, "jpg"))
			mime = "image/jpg";
		else if (!strcmp(p2, "class"))
			mime = "application/x-java-applet";
		else if (!strcmp(p2, "jar"))
			mime = "application/java-archive";
		else if (!strcmp(p2, "pdf"))
			mime = "application/pdf";
		else if (!strcmp(p2, "swf"))
			mime = "application/x-shockwave-flash";
		else if (!strcmp(p2, "ico"))
			mime = "image/vnd.microsoft.icon";
		else if (!strcmp(p2, "js"))
			mime = "text/javascript";
	}

	snprintf(bufo_in, sizeof(bufo_in), "%s/%s", www_dir, p);
	FILE *f = fopen(bufo_in, "rb");

	if (!f) {
		sendf(s, "HTTP/1.0 404 NOK\r\n\r\n");
		return;
	}

	int state = 1;
	/* disable push messages */
	nfp_setsockopt(s, NFP_IPPROTO_TCP, NFP_TCP_NOPUSH, &state, sizeof(state));

	sendf(s, "HTTP/1.0 200 OK\r\n");
	if (mime)
		sendf(s, "Content-Type: %s\r\n\r\n", mime);
	else
		sendf(s, "\r\n");

	while ((n = fread(bufo_in, 1, sizeof(bufo_in), f)) > 0) {
		w = mysend(s, bufo_in, n);
		if (w < 0)
			break;
	}
	/* flush the file */
	state = 0;
	nfp_setsockopt(s, NFP_IPPROTO_TCP, NFP_TCP_NOPUSH, &state, sizeof(state));

	fclose(f);
}

static int analyze_http(char *http, int s)
{
	char *url;
	char *p;

	if (!strncmp(http, "GET ", 4)) {
		url = http + 4;
		while (*url == ' ')
			url++;
		p = strchr(url, ' ');
		if (p)
			*p = 0;
		else
			return -1;
		get_file(s, url);
	} else if (!strncmp(http, "POST ", 5)) {
		/* Post is not supported. */
		NFP_INFO("%s", http);
	}

	return 0;
}

static void notify(union nfp_sigval *sv)
{
	struct nfp_sock_sigval *ss = (struct nfp_sock_sigval *)sv;
	int s = ss->sockfd;
	int event = ss->event;
	odp_packet_t pkt = ss->pkt;
	int r;
	char *buf, *tail;

	if (event == NFP_EVENT_ACCEPT) {
		struct nfp_sockaddr_in caller;
		nfp_socklen_t alen = sizeof(caller);
		int newfd = 0;

		/*
		 * ss->sockfd is the original listened socket.
		 * ss->sockfd2 is the new socket that is returned by accept.
		 * We don't need the returned socket, but accept
		 * must be called to set the data structures.
		 */
		newfd = nfp_accept(ss->sockfd,
				   (struct nfp_sockaddr *)&caller, &alen);
		(void)newfd;
		/* newfd == ss->sockfd2 */
		return;
	}

	if (event != NFP_EVENT_RECV)
		return;

	if (pkt == ODP_PACKET_INVALID)
		return;

	if (odp_unlikely(odp_packet_has_error(pkt))) {
		odp_packet_free(pkt);
		ss->pkt = ODP_PACKET_INVALID;
		return;
	}


	r = odp_packet_len(pkt);

	if (r > 0) {
		buf = odp_packet_data(pkt);
		/* Add 0 to the end */
		tail = odp_packet_push_tail(pkt, 1);
		*tail = 0;

		analyze_http(buf, s);

		if (nfp_close(s) < 0)
			NFP_ERR("nfp_close failed fd=%d err='%s'",
				s, nfp_strerror(nfp_errno));
	} else if (r == 0) {
		nfp_close(s);
	}

	odp_packet_free(pkt);
	/*
	 * Mark ss->pkt invalid to indicate it was released by us.
	 */
	ss->pkt = ODP_PACKET_INVALID;
}

int setup_webserver(char *root_dir, char *laddr, uint16_t lport)
{
	int serv_fd;
	struct nfp_sockaddr_in my_addr;
	struct nfp_sigevent ev;

	NFP_INFO("Setup webserver....");

	if (root_dir)
		www_dir = root_dir;
	else
		www_dir = DEFAULT_ROOT_DIRECTORY;

	serv_fd = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, NFP_IPPROTO_TCP);
	if (serv_fd  < 0) {
		NFP_ERR("nfp_socket failed");
		return -1;
	}

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = NFP_AF_INET;
	if (lport == 0)
		my_addr.sin_port = odp_cpu_to_be_16(DEFAULT_BIND_PORT);
	else
		my_addr.sin_port = odp_cpu_to_be_16(lport);
	if (laddr == NULL)
		my_addr.sin_addr.s_addr = DEFAULT_BIND_ADDRESS;
	else {
		struct in_addr laddr_lin;

		if (inet_aton(laddr, &laddr_lin) == 0) {
			NFP_ERR("Invalid local address.");
			nfp_close(serv_fd);
			return -1;
		}
		my_addr.sin_addr.s_addr = laddr_lin.s_addr;
	}
	my_addr.sin_len = sizeof(my_addr);

	if (nfp_bind(serv_fd, (struct nfp_sockaddr *)&my_addr,
		       sizeof(struct nfp_sockaddr)) < 0) {
		NFP_ERR("nfp_bind failed, err='%s'", nfp_strerror(nfp_errno));
		nfp_close(serv_fd);
		return 0;
	}

	nfp_listen(serv_fd, DEFAULT_BACKLOG);

	odp_memset(&ev, 0, sizeof(ev));
	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify;
	ev.sigev_value.sival_ptr = NULL;

	nfp_socket_sigevent(serv_fd, &ev);

	return 0;
}
