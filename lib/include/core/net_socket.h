#ifndef _NET_SOCKET_H_
#define _NET_SOCKET_H_

#include "core.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define LISTEN_QUEUE 1024

#define ADDR_FAMILY_IPV4 AF_INET
#define ADDR_FAMILY_IPV6 AF_INET6

#define PROTO_FAMILY_IPV4 PF_INET
#define PROTO_FAMILY_IPV6 PF_INET6

#define IPV4P_STRLEN (sizeof("255.255.255.255:65535"))
#define IPV6P_STRLEN (sizeof("[ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535"))

#define TCP_PROTO IPPROTO_TCP
#define UDP_PROTO IPPROTO_UDP
#define SCTP_PROTO IPPROTO_SCTP

#define FAM_ADDR_SIZE(family) \
    (family == ADDR_FAMILY_IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6))

#define tcp_socket(family) \
    socket(family, SOCK_STREAM, TCP_PROTO)

#define udp_socket(family) \
    socket(family, SOCK_DGRAM, UDP_PROTO)

#define sctp_scoket(family) \
    socket(family, SOCK_SEQPACKET, SCTP_PROTO)

#define MAX(x,y) \
    (x > y ? x : y)

struct sockaddr_unify {
    u8 data[sizeof(sockaddr_in6)];
};

/* 
@ptr is pointer to memory and @ln it is the size of memory
area which we want to set zero
*/
void btzero(u8* ptr, size_t ln);

/* 
writes to @dst buffer with @len length, the string representation
of the address from @addr, caller should free buffer
return:
 0 if @len not enough
 1 if succes
-1 if occured error
*/
i32 sock_ntop(char *dst, socklen_t len, const sockaddr *addr);
/*
bind socket @sockfd with random IP addres with addres family @addrFamily and random port
return:
 -1 if occured error
 >0 if succes(return port number)
*/
i32 sock_bind_wild(SOCKET sockfd, i32 addrFamily);

i32 sock_pton(char *dst, socklen_t len, const sockaddr *addr);

i32 sock_cmp_addr(const sockaddr* frs, const sockaddr* sec);

u16 sock_get_port(const sockaddr* addr);

i32 sock_get_local_addr(SOCKET sockfd, char *dst, socklen_t len);

i32 sock_get_peer_addr(SOCKET sockfd, char *dst, socklen_t len);

i64 readn(SOCKET sockfd, char *buffer, i64 len);

i64 writen(SOCKET sockfd, const char *buffer, i64 len);

#endif