#ifndef _UDP_CALL_H
#define _UDP_CALL_H 1

enum{
    UDP_FRAME_MASTER_ACK=2,
    UDP_FRAME_MASTER_REQ=3,
};
#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

void frame_master_ack(struct sockaddr_in *,void *,int );

#endif /* ifndef _UDP_CALL_H */

