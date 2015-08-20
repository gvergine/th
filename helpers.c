#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "th.h"

extern int efd;

void add_epoll_event(struct connection * c, int flags)
{
	struct epoll_event event;
	event.data.ptr = c;
	event.events = flags;
	epoll_ctl(efd,EPOLL_CTL_ADD, c->fd, &event);
}

int make_socket_non_blocking (int sfd)
{
	int flags;

	flags = fcntl (sfd, F_GETFL, 0);

	flags |= O_NONBLOCK;
	fcntl (sfd, F_SETFL, flags);

	return 0;
}

struct connection* create_and_bind (struct sockaddr_in * addr)
{
	struct connection * listen_connection = (struct connection*)calloc(1,sizeof(struct connection));

	listen_connection->role = 0;
	listen_connection->fd = socket(AF_INET, SOCK_STREAM, 0);

	memcpy(&listen_connection->src,addr,sizeof(struct sockaddr_in));

	if (bind(listen_connection->fd,(struct sockaddr*)&listen_connection->src,sizeof(struct sockaddr_in)) < 0)
	{
		th_evt_error(listen_connection,"bind");
		free(listen_connection);
		return 0;
	}

	listen(listen_connection->fd,1);

	add_epoll_event(listen_connection,EPOLLIN);

	return listen_connection;
}

struct connection* create (struct sockaddr_in * addr)
{
	struct connection * c = (struct connection*)calloc(1,sizeof(struct connection));

	c->role = 1;
	c->fd = socket(AF_INET, SOCK_STREAM, 0);

	memcpy(&c->dst,addr,sizeof(struct sockaddr_in));
	c->dst_addr_len = sizeof(struct sockaddr_in);

	connect(c->fd,(struct sockaddr*)&c->dst,c->dst_addr_len);
	make_socket_non_blocking(c->fd);


	add_epoll_event(c,EPOLLIN);


	return c;
}

