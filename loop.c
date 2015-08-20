#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#ifdef HAS_ORIG_DST_VIA_NETFILTER
#include <linux/netfilter_ipv4.h>
#endif

#include "th.h"

#define MAXEVENTS 64
#define BUF_SIZE 512


int efd;


int th_service(int argc, char* argv[])
{

	struct epoll_event * events;

	efd = epoll_create1(0);
	events = (struct epoll_event*)calloc(MAXEVENTS,sizeof(struct epoll_event));

	int r = th_evt_init(argc, argv);
	if (r != 0) return r;

	atexit(th_evt_fini);

	while(1)
	{
		int n,i;
		struct connection * c;
		//struct epoll_event event;

		n = epoll_wait(efd, events, MAXEVENTS, -1);

		for (i = 0; i < n; i++)
		{
			c = (struct connection*)events[i].data.ptr;

			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
			{
				th_evt_error(c,"events");
				close(c->fd);
				continue;
			}
			else
			{
				if (c->role == 0) // is a listen socket, accept everything and send notification
				{
					struct connection * new_connection = (struct connection*)calloc(1,sizeof(struct connection));
					new_connection->role = 1;
					new_connection->src_addr_len = sizeof (struct sockaddr_in);
					new_connection->fd = accept(c->fd,(struct sockaddr*)&new_connection->src,&new_connection->src_addr_len);
					make_socket_non_blocking(new_connection->fd);
					bzero(&new_connection->dst,sizeof(struct sockaddr_in));

#ifdef HAS_ORIG_DST_VIA_NETFILTER
					new_connection->dst.sin_family = AF_INET;
					new_connection->dst_addr_len = sizeof(struct sockaddr_in);
					if (getsockopt(new_connection->fd,SOL_IP,SO_ORIGINAL_DST,&new_connection->dst,&new_connection->dst_addr_len) < 0)
						th_evt_error(new_connection,"getsockopt");
#endif
					th_evt_newconnection(new_connection);

					add_epoll_event(new_connection,EPOLLIN);


				}
				else // is a connect socket, read everything and send notification
				{
					int done = 0;

					while (1)
					{
						ssize_t count;
						char buf[BUF_SIZE];

						count = read(c->fd, buf, sizeof(buf));

						if (count == -1)
						{
							if (errno != EAGAIN) done = 1;
							break;
						}
						else if (count == 0)
						{
							done = 1;
							break;
						}

						th_evt_data(c,buf,count);

						add_epoll_event(c,EPOLLIN);


					}

					if (done)
					{
						th_evt_closedconnection(c);
						close(c->fd);
					}

				}

			}
		}

	}



	return EXIT_SUCCESS;
}