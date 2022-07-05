#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "nfp.h"
#include "httpd.h"

int sigreceived = 0;

#define NUM_EPOLL_EVT 10

/* Table of concurrent connections */
#define NUM_CONNECTIONS 1024

typedef struct {
	int fd;
	uint32_t addr;
} connection_t;

static connection_t connections[NUM_CONNECTIONS];

/* Webserver thread argument */
static webserver_arg_t web_arg;

static int webserver_select(int server_fd);
static int webserver_epoll(int server_fd);

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
		if (len) {
			NFP_WARN("Only %d bytes sent", n);
		}
	}
	return len;
}

static int sendf(int fd, const char *fmt, ...)
{
	char buf[1024];
	int ret;
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ret = mysend(fd, buf, n);
	return ret;
}

/* Send one file. */
static void get_file(int s, char *url)
{
	char bufo[512];
	int n, w;

	const char *mime = NULL;
	const char *file_name = url;

	if (!file_name || *file_name == '\0')
		file_name = "index.html";
	else if (*file_name == '/')
		file_name++;

	char *p2 = strrchr(file_name, '.');
	if (p2) {
		p2++;
		if (!strcmp(p2, "html")) mime = "text/html";
		else if (!strcmp(p2, "htm")) mime = "text/html";
		else if (!strcmp(p2, "css")) mime = "text/css";
		else if (!strcmp(p2, "txt")) mime = "text/plain";
		else if (!strcmp(p2, "png")) mime = "image/png";
		else if (!strcmp(p2, "jpg")) mime = "image/jpg";
		else if (!strcmp(p2, "class")) mime = "application/x-java-applet";
		else if (!strcmp(p2, "jar")) mime = "application/java-archive";
		else if (!strcmp(p2, "pdf")) mime = "application/pdf";
		else if (!strcmp(p2, "swf")) mime = "application/x-shockwave-flash";
		else if (!strcmp(p2, "ico")) mime = "image/vnd.microsoft.icon";
		else if (!strcmp(p2, "js")) mime = "text/javascript";
	}

	snprintf(bufo, sizeof(bufo), "%s/%s", web_arg.root_dir, file_name);
	FILE *f = fopen(bufo, "rb");

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

	while ((n = fread(bufo, 1, sizeof(bufo), f)) > 0)
		if ((w = mysend(s, bufo, n)) < 0)
			break;

	/* flush the file */
	state = 0;
	nfp_setsockopt(s, NFP_IPPROTO_TCP, NFP_TCP_NOPUSH, &state, sizeof(state));

	fclose(f);
}

static int analyze_http(char *http, int s) {
	char *url;

	if (!strncmp(http, "GET ", 4)) {
		url = http + 4;
		while (*url == ' ')
			url++;
		char *p = strchr(url, ' ');
		if (p)
			*p = 0;
		else
			return -1;
		NFP_INFO("GET %s (fd=%d)", url, s);
		get_file(s, url);
	} else if (!strncmp(http, "POST ", 5)) {
		/* Post is not supported. */
		NFP_INFO("%s", http);
	}

	return 0;
}

static void monitor_connections(nfp_fd_set *fd_set)
{
	int i;

	for (i = 0; i < NUM_CONNECTIONS; ++i)
		if (connections[i].fd != -1)
			NFP_FD_SET(connections[i].fd, fd_set);
}

static inline int accept_connection(int serv_fd, int *con_id)
{
	int tmp_fd, i;
	struct nfp_sockaddr_in caller;
	unsigned int alen = sizeof(caller);

	for (i = 0; i < NUM_CONNECTIONS; i++)
		if (connections[i].fd == -1)
			break;

	if (i >= NUM_CONNECTIONS) {
		NFP_ERR("Node cannot accept new connections!");
		return -1;
	}

	tmp_fd = nfp_accept(serv_fd, (struct nfp_sockaddr *)&caller, &alen);
	if (tmp_fd < 0)
		return -1;

	NFP_INFO("accept fd=%d", tmp_fd);

#if 0
	struct nfp_linger so_linger;

	so_linger.l_onoff = 1;
	so_linger.l_linger = 0;
	int r1 = nfp_setsockopt(tmp_fd,
					NFP_SOL_SOCKET,
					NFP_SO_LINGER,
					&so_linger,
					sizeof(so_linger));
	if (r1)
		NFP_ERR("SO_LINGER failed!");
#endif
	struct nfp_timeval tv;

	tv.tv_sec = 3;
	tv.tv_usec = 0;
	int r2 = nfp_setsockopt(tmp_fd,
					NFP_SOL_SOCKET,
					NFP_SO_SNDTIMEO,
					&tv,
					sizeof(tv));
	if (r2)
		NFP_ERR("SO_SNDTIMEO failed!");

	connections[i].fd = tmp_fd;
	connections[i].addr = caller.sin_addr.s_addr;

	if (con_id)
		*con_id = i;

	return tmp_fd;
}

static int handle_connection(int i)
{
	int r;
	static char buf[1024];

	if (connections[i].fd == -1)
		return -1;

	r = nfp_recv(connections[i].fd, buf, sizeof(buf)-1, 0);

	if (r < 0)
		return -1;

	if (r > 0) {
		buf[r] = 0;
		NFP_INFO("recv data: %s", buf);

		if (!strncmp(buf, "GET", 3))
			analyze_http(buf, connections[i].fd);
		else
			NFP_INFO("Not an HTTP GET request");

		NFP_INFO("closing %d\n", connections[i].fd);

		while (nfp_close(connections[i].fd) < 0) {
			if (NFP_ECONNRESET == nfp_errno)
				break;
			NFP_ERR("nfp_close failed, fd=%d err='%s'",
				connections[i].fd,
				nfp_strerror(nfp_errno));
			sleep(1);
		}
		NFP_INFO("closed fd=%d", connections[i].fd);
		connections[i].fd = -1;
	} else if (r == 0) {
		nfp_close(connections[i].fd);
		connections[i].fd = -1;
	}

	return 0;
}

static int webserver_select(int serv_fd)
{
	int tmp_fd, nfds, i, r;
	nfp_fd_set read_fd;
	struct nfp_timeval timeout;
	odp_bool_t *is_running = NULL;

	NFP_INFO("Using nfp_select");

	NFP_FD_ZERO(&read_fd);
	nfds = serv_fd;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	while (*is_running) {
		timeout.tv_sec = 0;
		timeout.tv_usec = 200000;

		NFP_FD_SET(serv_fd, &read_fd);
		monitor_connections(&read_fd);
		r = nfp_select(nfds + 1, &read_fd, NULL, NULL, &timeout);

		if (r <= 0) {
			if (r == 0)
				continue;
			else
				break;
		}

		if (NFP_FD_ISSET(serv_fd, &read_fd)) {
			tmp_fd = accept_connection(serv_fd, NULL);

			if (tmp_fd > nfds)
				nfds = tmp_fd;
		}

		for (i = 0; i < NUM_CONNECTIONS; i++)
			if (connections[i].fd != -1 &&
			    NFP_FD_ISSET(connections[i].fd, &read_fd)) {
				tmp_fd = connections[i].fd;

				handle_connection(i);

				NFP_FD_CLR(tmp_fd, &read_fd);
			}
	}

	/* cleanup */
	for (i = 0; i < NUM_CONNECTIONS; i++) {
		if (connections[i].fd == -1)
			continue;

		NFP_FD_CLR(connections[i].fd, &read_fd);

		nfp_close(connections[i].fd);
		connections[i].fd = -1;
	}

	nfp_close(serv_fd);

	return 0;
}

static int webserver_epoll(int serv_fd)
{
	int i, r;
	int tmp_fd, conn_id;
	int epfd = -1;
	struct nfp_epoll_event events[NUM_EPOLL_EVT];
	struct nfp_epoll_event e;
	connection_t *conn = NULL;
	odp_bool_t *is_running = NULL;

	NFP_INFO("Using nfp_epoll");

	epfd = nfp_epoll_create(1);
	if (epfd == -1) {
		NFP_ERR("Cannot create epoll (%s)!", nfp_strerror(nfp_errno));
		return -1;
	}

	conn = &connections[0];
	conn->fd = serv_fd;
	e.events = NFP_EPOLLIN;
	e.data.ptr = conn;
	nfp_epoll_ctl(epfd, NFP_EPOLL_CTL_ADD, serv_fd, &e);

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	while (*is_running) {
		r = nfp_epoll_wait(epfd, events, NUM_EPOLL_EVT, 200);
		if (r < 0)
			break;

		for (i = 0; i < r; ++i) {
			conn = (connection_t *)events[i].data.ptr;

			if (conn->fd == serv_fd) {
				tmp_fd = accept_connection(serv_fd, &conn_id);
				if (tmp_fd == -1)
					continue;

				e.events = NFP_EPOLLIN;
				e.data.ptr = &connections[conn_id];

				nfp_epoll_ctl(epfd, NFP_EPOLL_CTL_ADD,
					      tmp_fd, &e);
			} else {
				tmp_fd = conn->fd;
				conn_id = conn - connections;

				handle_connection(conn_id);

				nfp_epoll_ctl(epfd, NFP_EPOLL_CTL_DEL,
					      tmp_fd, NULL);
			}
		}
	}

	/* cleanup */
	for (i = 0; i < NUM_CONNECTIONS; i++) {
		if (connections[i].fd == -1)
			continue;

		nfp_epoll_ctl(epfd, NFP_EPOLL_CTL_DEL, connections[i].fd, NULL);

		nfp_close(connections[i].fd);
		connections[i].fd = -1;
	}

	nfp_close(epfd);

	return 0;
}

static int create_server_socket(void)
{
	int serv_fd = -1;
	uint32_t myaddr;
	struct nfp_sockaddr_in my_addr;
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;

	ifnet = nfp_ifport_ifnet_get(0, NFP_IFPORT_NET_SUBPORT_ITF);
	if (ifnet == NFP_IFNET_INVALID) {
		NFP_ERR("Interface not found.");
		return -1;
	}

	if (nfp_ifnet_ipv4_addr_get(ifnet, NFP_IFNET_IP_TYPE_IP_ADDR,
				    &myaddr)) {
		NFP_ERR("Faile to get IP address.");
		return -1;
	}

	if ((serv_fd = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, NFP_IPPROTO_TCP)) < 0) {
		NFP_ERR("nfp_socket failed");
		return -1;
	}

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = NFP_AF_INET;
	my_addr.sin_port = odp_cpu_to_be_16(web_arg.lport);
	my_addr.sin_addr.s_addr = myaddr;
	my_addr.sin_len = sizeof(my_addr);

	if (nfp_bind(serv_fd, (struct nfp_sockaddr *)&my_addr,
		       sizeof(struct nfp_sockaddr)) < 0) {
		NFP_ERR("Cannot bind http socket (%s)!", nfp_strerror(nfp_errno));
		nfp_close(serv_fd);
		return -1;
	}

	if (nfp_listen(serv_fd, 10)) {
		NFP_ERR("Cannot listen http socket (%s)!",
			nfp_strerror(nfp_errno));
		nfp_close(serv_fd);
		return -1;
	}

	return serv_fd;
}

static int webserver(void *arg)
{
	int serv_fd, i;
	int ret = 0;

	if (!arg)
		return -1;

	NFP_INFO("HTTP thread started");

	sleep(1);

	web_arg = *(webserver_arg_t *)arg;

	for (i = 0; i < NUM_CONNECTIONS; i++)
		connections[i].fd = -1;

	serv_fd = create_server_socket();
	if (serv_fd == -1) {
		NFP_ERR("Error: create_server_socket() failed.\n");
		return -1;
	}

	if (web_arg.use_epoll)
		ret = webserver_epoll(serv_fd);
	else
		ret = webserver_select(serv_fd);

	NFP_INFO("httpd exiting");
	return ret;
}

int nfp_start_webserver_thread(nfp_thread_t *webserver_pthread, int core_id,
			       webserver_arg_t *arg)
{
	odp_cpumask_t cpumask;
	nfp_thread_param_t thread_param = {0};

	odp_cpumask_zero(&cpumask);
	odp_cpumask_set(&cpumask, core_id);

	nfp_thread_param_init(&thread_param);
	thread_param.start = webserver;
	thread_param.arg = arg;
	thread_param.thr_type = ODP_THREAD_CONTROL;
	thread_param.description = "web";

	return nfp_thread_create(webserver_pthread, 1,
			       &cpumask, &thread_param);
}
