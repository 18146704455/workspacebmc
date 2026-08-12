#include <stdio.h>
#include <sys/socket.h>
#include "usock.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ev.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "common_log.h"


#define UDP_SERVER_PORT 3689
#define MSG_MAX_SIZE 1024
#define MSG_VERSION 0x8000



struct udp_server_st {
    ev_io watcher;
    struct ev_loop *loop;
    int fd;
    struct list_head server;
};

static struct udp_server_st  local_server;



struct udp_msg{
    unsigned int version;
    unsigned int msg_id;
    unsigned int msg_type;
    char msg[MSG_MAX_SIZE];
};
struct udp_msg_head_st{
    unsigned int version;
    unsigned int msg_id;
    unsigned int msg_type;
};
struct udp_client_st {
    struct sockaddr_in client;
    unsigned int len;
    struct udp_msg data;
};

struct udp_server_call_st{
    struct list_head next;
    unsigned int msg_type;
    void (*msg_call)(struct sockaddr_in*,void *, int);
};

int reg_udp_server_call(struct udp_server_st* server, unsigned int msg_type, void *msg_call)
{
    FUNC_START();
    struct udp_server_call_st *call_st=malloc(sizeof(struct udp_server_call_st));
    memset(call_st,0,sizeof(struct udp_server_call_st));
    if(call_st)
    {
        call_st->msg_type=msg_type;
        call_st->msg_call=msg_call;
        list_add(&call_st->next,&server->server);
    }
    FUNC_END();
}

void unreg_udp_server_call(struct udp_server_st *server,unsigned int msg_type, void *msg_call)
{
    FUNC_START();
    struct udp_server_call_st *head, *p;
    list_for_each_entry_safe(p,head,&server->server,next)
    {
        if(p->msg_type==msg_type && p->msg_call == msg_call)
        {
            list_del(&p->next);
            free(p);
        }
    }
    FUNC_END();
}

static struct udp_client_st *init_msg(unsigned type, void *msg, int len)
{
    FUNC_START();
    static unsigned int msg_id=0;
    struct udp_client_st  *cliaddr =  malloc(sizeof(struct udp_client_st));
    memset(cliaddr, 0, sizeof(struct udp_client_st));
    memcpy(cliaddr->data.msg,msg,len);
    cliaddr->len=len+sizeof(struct udp_msg_head_st);
    cliaddr->data.version=MSG_VERSION;
    cliaddr->data.msg_type=type;
    cliaddr->data.msg_id=msg_id;

    msg_id++;
    FUNC_END();
    return cliaddr;
}
static int send_to_client(struct udp_client_st *cliaddr)
{
    return sendto(local_server.fd,&cliaddr->data,cliaddr->len,0,(struct sockaddr *)&cliaddr->client,sizeof(struct sockaddr));
}
int send_ack(struct udp_client_st* cliaddr, void *msg, int len)
{
    FUNC_START();
    cliaddr->len=len+sizeof(struct udp_msg_head_st);
    cliaddr->data.msg_type &= ~(1<<0);
    memcpy(cliaddr->data.msg,msg,len);
    send_to_client(cliaddr);
    FUNC_END();

}
int send_broadcast(unsigned int type, void *msg, int len)
{
    FUNC_START();
    struct udp_client_st *cliaddr = init_msg(type,msg,len);
    cliaddr->client.sin_family=AF_INET;
    inet_aton("172.16.3.255",&cliaddr->client.sin_addr);
    cliaddr->client.sin_port=htons(UDP_SERVER_PORT);
    FUNC_END();
    return send_to_client(cliaddr);
}
void recv_callback(struct ev_loop *loop, ev_io *w, int revents)
{
    FUNC_START();
    struct udp_server_call_st *p;
    socklen_t len = sizeof(struct sockaddr);
    struct udp_client_st  *cliaddr =  malloc(sizeof(struct udp_client_st));
    memset(cliaddr, 0, sizeof(struct udp_client_st));
    int n = recvfrom(w->fd, &cliaddr->data, sizeof(struct udp_msg), 0, (struct sockaddr *)&cliaddr->client, &len);
    if(n)
    {
        pr_debug("recvfrom[%s:%d]:%u\n",(char*)inet_ntoa(cliaddr->client.sin_addr),htons(cliaddr->client.sin_port),cliaddr->data.version);
        if(cliaddr->data.version != MSG_VERSION) goto end;
        //check version
        //check msg id
        list_for_each_entry(p,&local_server.server,next)
        {
            if(p->msg_call&& p->msg_type == cliaddr->data.msg_type) 
            {
                p->msg_call(&cliaddr->client,cliaddr->data.msg, cliaddr->len-sizeof(struct udp_msg_head_st));
            }
        }
    }
end:
    free(cliaddr);
    FUNC_END();
}
struct udp_server_st* udp_init(char *host,char *port)
{
    FUNC_START();
    int type= USOCK_UDP | USOCK_SERVER |USOCK_BROADCAST | USOCK_NONBLOCK;
    local_server.fd = usock(type,"0.0.0.0",usock_port(UDP_SERVER_PORT));
    ev_io_init(&local_server.watcher, recv_callback, local_server.fd, EV_READ);
    local_server.watcher.data=&local_server;
    INIT_LIST_HEAD(&local_server.server);
    FUNC_END();
    return &local_server;
}
void set_udp_loop(struct udp_server_st *p,struct ev_loop *loop)
{
    FUNC_START();
    p->loop = loop;
    ev_io_start(p->loop,&p->watcher);
    FUNC_END();
}

#if 0
char msg[1024]={};
int main(int argc,char *argv[])
{
    socklen_t len = sizeof(struct sockaddr);
    struct sockaddr_in cliaddr;
    char *p=&cliaddr;
    int i=0;
    //int s_fd = init_udp_socket(0,argv[1],12354);
    //int c_fd = init_udp_socket(0,"192.168.1.97",12356); 
    int s_fd = udp_server_init("",argv[2]);
    int c_fd = udp_client_init("192.168.1.97","12388");
    const int opt=1;
    printf("%d,c%d\n",s_fd,c_fd);
    while(1)
    {
        int n = recvfrom(s_fd, msg, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        printf("recvfrom[");
        printf("%s:%d",(char*)inet_ntoa(cliaddr.sin_addr),htons(cliaddr.sin_port));
#if 0
        for(i=0;i<len;i++)
        {
            printf("%x ",p++);
        }
#endif
        printf("]: [%s]\n",msg);
#if 1
        msg[0]+=1;
        sendto(s_fd,msg,120,0,(struct sockaddr*)&cliaddr,len);
        send_board(s_fd,12355,msg,12);

#endif
        //printf("%s\n",msg);
        memset(msg,1024,0);
        sleep(1);
    }
}
#endif
