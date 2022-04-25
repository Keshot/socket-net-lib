#include "core/core.h"
#include "core/net_socket.h"
#include "utils/error_handler.h"

#include <time.h>
#include <string.h>

#define BUFFER_LENGTH 100000

int main(int argc, char *argv[])
{
    char ipaddrPres[IPV4P_STRLEN];
    char *rdBuffer = new char[BUFFER_LENGTH];
    SOCKET serverfd = INVALID_SOCKET, acceptedfd = INVALID_SOCKET;
    
    pid_t processID = getpid();
    pid_t parentProcessID = getppid();

    i64 pagesize = sysconf(_SC_PAGE_SIZE);

    sockaddr_in sa;
    btzero(reinterpret_cast<u8*>(&sa), sizeof(sockaddr_in));

    sa.sin_family = ADDR_FAMILY_IPV4;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(5643);

    if( (serverfd = socket(PROTO_FAMILY_IPV4, SOCK_STREAM, TCP_PROTO)) <= INVALID_SOCKET) {
        err_crit("socket system call failed");
    }

    if( bind(serverfd, reinterpret_cast<sockaddr*>(&sa), sizeof(sockaddr_in)) < 0) {
        err_crit("bind system call failed");
    }

    if( listen(serverfd, LISTEN_QUEUE) < 0) {
        err_crit("listen system call failed");
    }

    socklen_t len = sizeof(sa);
    if (getsockname(serverfd, ccast(sockaddr*, &sa), &len) < 0) {
        err_log("getsockname failed");
    }
    else {
        i32 ret = sock_ntop(ipaddrPres, IPV4P_STRLEN, ccast(const sockaddr*, &sa));
        if(ret != 1) {
            err_log("sock_ntop failed");
        }
        else {
            printf("server local ip: %s\tport: %u\n", ipaddrPres, sock_get_port(ccast(const sockaddr*, &sa)));
        }
    }

    while(true) {
        sockaddr_unify addr;
        len = sizeof(sockaddr_unify);
        if ( (acceptedfd = accept(serverfd, ccast(sockaddr*, &addr), &len)) < 0) {
            err_log("accept system call failed");
            continue;
        }
        i32 ret = sock_ntop(ipaddrPres, IPV4P_STRLEN, ccast(const sockaddr*, &addr));
        if(ret != 1) {
            err_log("sock_ntop failed");
        }
        else {
            printf("accepted ip: %s\tport: %u\n", ipaddrPres, sock_get_port(ccast(const sockaddr*, &addr)));

            len = sizeof(sockaddr_unify);
            if (getsockname(acceptedfd, ccast(sockaddr*, &addr), &len) < 0) {
                err_log("getsockname failed");
            }
            else {
                i32 ret = sock_ntop(ipaddrPres, IPV4P_STRLEN, ccast(const sockaddr*, &addr));
                if(ret != 1) {
                    err_log("sock_ntop failed");
                }
                else {
                    printf("accepted local ip: %s\tport: %u\n", ipaddrPres, sock_get_port(ccast(const sockaddr*, &addr)));
                }
            }
        }

        while(true) {
            i64 readed = readn(acceptedfd, rdBuffer, BUFFER_LENGTH);
            
            if(readed == 0) {
                break;
            }

            writen(acceptedfd, rdBuffer, readed);

            if(readed < BUFFER_LENGTH) {
                break;
            }
        }
 
        close(acceptedfd);
    }

    return 0;
}