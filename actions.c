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
#include "helpers.h"

void th_act_startlistening(char* addr, unsigned short int port)
{
	struct sockaddr_in a;

	memset(&a, 0, sizeof(struct sockaddr_in));
	a.sin_family = AF_INET;
	a.sin_port = htons(port);
	//a.sin_addr.s_addr = INADDR_ANY;
	inet_aton(addr,&a.sin_addr);

	struct connection * c = create_and_bind(&a);
	if (c)
    	th_evt_newconnection(c);


}

void th_act_stoplistening(struct connection * c)
{
	if(c->role == 0)
	{
	    close(c->fd);
	}
}

void th_act_createconnection(struct sockaddr_in * addr, void* user)
{

	struct connection * c = create(addr);
	if (c)
	{
		c->user = user;
    	th_evt_newconnection(c);
	}
}


void th_act_destroyconnection(struct connection * c)
{

	if(c->user != 0)
	{
		struct connection * otherEnd = (struct connection*)c->user;
		close(otherEnd->fd);
		c->user = 0;
	}
}

void th_act_send(struct connection* c,char* buf,ssize_t len)
{
	write(c->fd,buf,len);

	//add_epoll_event(c,EPOLLIN);

}

void th_act_exit(int err)
{
	exit(err);
}