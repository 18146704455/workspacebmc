#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H 1
#include <ev.h>
struct udp_server_st {
    ev_io watcher;
    struct ev_loop *loop;
    int fd;
};

struct udp_server_st* udp_init(char *host,char *port);
void set_udp_loop(struct udp_server_st *p,struct ev_loop *loop);
#endif /* ifndef _UDP_SERVER_H */

