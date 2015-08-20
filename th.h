#ifndef TH_H
#define TH_H

#include <netinet/in.h>

////////////// types
struct connection
{
	int role; // 0 = listen socket, else is conn socket
	int fd; // fd of the socket
	struct sockaddr_in src; // listen address for listen socket or origin address for conn socket
	socklen_t src_addr_len;
	struct sockaddr_in dst; // real destination address (just for conn socket)
	socklen_t dst_addr_len;
	void * user; // user pointer
};


////////////// events

int th_evt_init(int argc, char* argv[]);
void th_evt_fini();
void th_evt_error(struct connection *, const char* domain);
void th_evt_newconnection(struct connection *);
void th_evt_data(struct connection*,char*,ssize_t);
void th_evt_closedconnection(struct connection *);

////////////// actions
void th_act_startlistening(char* addr, unsigned short int port);
void th_act_stoplistening(struct connection * c);
void th_act_createconnection(struct sockaddr_in * addr, void* user);
void th_act_send(struct connection*,char*,ssize_t);
void th_act_destroyconnection(struct connection * c);
void th_act_exit(int err);

/////////////// service
int th_service();

#endif /* TH_H */
