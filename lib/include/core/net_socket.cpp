#include "net_socket.h"
#include <string.h>
#include <errno.h>

#define PORTF_STRLEN (sizeof("]:65535"))

void btzero(u8* ptr, size_t ln)
{
    for(; ln; *ptr++ = 0, --ln);
}

i64 readn(SOCKET sockfd, char *buffer, i64 len)
{
    i64 readed = 0;

    while(true) {
        i64 local_readed = read(sockfd, buffer, len - readed);

        if(local_readed < 0) {
            if(errno == EINTR) {
                local_readed = 0;
            }
            else {
                printf("error while read: %s\n", strerror(errno));
                return -1;
            }
        }
        else if(local_readed == 0) {
            break;
        }

        readed += local_readed;
        buffer += local_readed;
        if(local_readed < len - readed) {
            break;
        }
    }

    return readed;
}

i64 writen(SOCKET sockfd, const char *buffer, i64 len)
{
    i64 writed = 0;

    while(true) {
        i64 local_write = write(sockfd, buffer, len - writed);

        if(local_write < 0) {
            if(errno == EINTR) {
                local_write = 0;
            }
            else {
                return -1;
            }
        }
        else if(local_write == 0) {
            break;
        }

        writed += local_write;
        buffer += local_write;
    }

    return writed;
}

u16 sock_get_port(const sockaddr* addr)
{
    if(!addr) {
        errno = EINVAL;
        return -1;
    }

    switch (addr->sa_family)
    {

    case ADDR_FAMILY_IPV4: {
        return ntohs(ccast(const sockaddr_in*, addr)->sin_port);
    } break;

    case ADDR_FAMILY_IPV6: {
        return ntohs(ccast(const sockaddr_in6*, &addr)->sin6_port);
    } break;

    default:{
        errno = EAFNOSUPPORT;
    }

    }
    return -1;
}

i32 sock_cmp_addr(const sockaddr* frs, const sockaddr* sec)
{
    if((!frs || !sec) || (frs->sa_family != sec->sa_family)) {
        errno = EINVAL;
        return -1;
    }

    switch (frs->sa_family)
    {

    case ADDR_FAMILY_IPV4: {
        const sockaddr_in *frst_sock_addr = ccast(const sockaddr_in*, frs);
        const sockaddr_in *sec_sock_addr = ccast(const sockaddr_in*, sec); 
        return memcmp(&frst_sock_addr->sin_addr, &sec_sock_addr->sin_addr, sizeof(frst_sock_addr->sin_addr));
    } break;

    case ADDR_FAMILY_IPV6: {
        const sockaddr_in6 *frst_sock_addr = ccast(const sockaddr_in6*, frs);
        const sockaddr_in6 *sec_sock_addr = ccast(const sockaddr_in6*, sec); 
        return memcmp(&frst_sock_addr->sin6_addr, &sec_sock_addr->sin6_addr, sizeof(frst_sock_addr->sin6_addr));
    } break;

    default:{
        errno = EAFNOSUPPORT;
    }

    }
    return -1;
}

i32 sock_bind_wild(SOCKET sockfd, i32 addrFamily)
{
    socklen_t tmpLen = FAM_ADDR_SIZE(addrFamily);
    switch (addrFamily)
    {

    case ADDR_FAMILY_IPV4: {
        sockaddr_in addr;    

        addr.sin_family = ADDR_FAMILY_IPV4;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(0);

        if (bind(sockfd, ccast(const sockaddr*, &addr), sizeof(sockaddr_in)) < 0) {
            return -1;
        }
        if (getsockname(sockfd, ccast(sockaddr*, &addr), &tmpLen) < 0) {
            return -1;
        }
        return addr.sin_port;
    } break;

    case ADDR_FAMILY_IPV6: {
        sockaddr_in6 addr;    

        addr.sin6_family = ADDR_FAMILY_IPV6;
        addr.sin6_addr = in6addr_any;
        addr.sin6_port = htons(0);

        if (bind(sockfd, ccast(const sockaddr*, &addr), sizeof(sockaddr_in6)) < 0) {
            return -1;
        }
        if (getsockname(sockfd, ccast(sockaddr*, &addr), &tmpLen) < 0) {
            return -1;
        }
        return addr.sin6_port;
    } break;

    default:{
        errno = EAFNOSUPPORT;
    }

    }
    return -1;
}

i32 sock_ntop(char *dst, socklen_t len, const sockaddr *addr)
{
    char portstr[PORTF_STRLEN];

    if(!dst) {
        errno = EINVAL;
        return -1;
    }

    switch (addr->sa_family)
    {

    case ADDR_FAMILY_IPV4: {
        if(len < IPV4P_STRLEN) {
            return 0;
        }

        const sockaddr_in *inaddr = ccast(const sockaddr_in*, addr);
        if(inet_ntop(ADDR_FAMILY_IPV4, ccast(const void*, &inaddr->sin_addr), dst, len)) {
            snprintf(portstr, PORTF_STRLEN, ":%d", ntohs(inaddr->sin_port));
            strncat(dst, portstr, PORTF_STRLEN - 1);
            return 1;
        }
    } break;

    case ADDR_FAMILY_IPV6: {
        if(len < IPV6P_STRLEN) {
            return 0;
        }

        const sockaddr_in6 *in6addr = ccast(const sockaddr_in6*, addr);
        dst[0] = '[';
        if(inet_ntop(ADDR_FAMILY_IPV6, ccast(const void*, &in6addr->sin6_addr), dst + 1, len - 1)) {
            snprintf(portstr, PORTF_STRLEN, "]:%d", ntohs(in6addr->sin6_port));
            strncat(dst, portstr, PORTF_STRLEN);
            return 1;
        }
    } break;

    default: {
        snprintf(dst, len, "AF no support.");
        errno = EAFNOSUPPORT;
    }

    }
    return -1;
}

i32 sock_pton(char *src, sockaddr *addr)
{
    if (!addr) {
        errno = EINVAL;
        return -1;
    }

    return inet_pton(addr->sa_family, src, addr);
}

i32 sock_get_local_addr(SOCKET sockfd, char *dst, socklen_t len)
{
    sockaddr_unify addr;
    socklen_t addrLen = sizeof(addr);
    sockaddr *addrp = ccast(sockaddr*, &addr);

    if (getsockname(sockfd, addrp, &addrLen) < 0) {
        return -1;
    }

    return sock_ntop(dst, len, addrp);
}

i32 sock_get_peer_addr(SOCKET sockfd, char *dst, socklen_t len)
{
    sockaddr_unify addr;
    socklen_t addrLen = sizeof(addr);
    sockaddr *addrp = ccast(sockaddr*, &addr);

    if (getpeername(sockfd, addrp, &addrLen) < 0) {
        return -1;
    }

    return sock_ntop(dst, len, addrp);
}