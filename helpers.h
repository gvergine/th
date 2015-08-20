#ifndef HELPERS_H
#define HELPERS_H

void add_epoll_event(struct connection * c, int flags);
int make_socket_non_blocking (int sfd);

struct connection* create_and_bind (struct sockaddr_in * addr);

struct connection* create (struct sockaddr_in * addr);
#endif