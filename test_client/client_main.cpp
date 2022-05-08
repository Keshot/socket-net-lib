#include "core/core.h"
#include "core/net_socket.h"
#include "utils/error_handler.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 1024
#define BUFFER_LENGTH 100000

int main(int argc, char *argv[])
{
    bool stdineof = false;
    char *readbuff = new char[BUFFER_LENGTH];
    SOCKET conn = INVALID_SOCKET;
    sockaddr_in sa, connaddr;

    btzero(reinterpret_cast<u8*>(&sa), sizeof(sockaddr_in));

    sa.sin_family = ADDR_FAMILY_IPV4;
    sa.sin_port = htons(5643);

    if (inet_pton(ADDR_FAMILY_IPV4, "127.0.0.1", reinterpret_cast<void*>(&sa.sin_addr)) <= 0) {
        err_crit("inet_pton call failed");
    }

    if ( (conn = socket(ADDR_FAMILY_IPV4, SOCK_STREAM, TCP_PROTO)) <= INVALID_SOCKET) {
        err_crit("socket system call failed");
    }

    int opt = 1;
    if (setsockopt(conn, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        err_crit("setsockopt failed");
    }

    connaddr.sin_family = ADDR_FAMILY_IPV4;
    connaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    connaddr.sin_port = htons(10258);

    if (bind(conn, ccast(const sockaddr*,&connaddr), sizeof(connaddr)) < 0) {
        err_crit("bind system call failed");
    }

    if (connect(conn, reinterpret_cast<sockaddr*>(&sa), sizeof(sockaddr_in)) < 0) {
        err_crit("connect system call failed");
    }

    if (sock_get_local_addr(conn, readbuff, BUFFER_LENGTH) != 1) {
        err_log("getsockname failed");
    }
    else {
        printf("server local ip: %s\n", readbuff);
    }
    
    fd_set readfds;
    int infd = fileno(stdin);
    
    while(true) {
        FD_ZERO(&readfds);
        if(!stdineof) {
            FD_SET(infd, &readfds);
        }
        FD_SET(conn, &readfds);

        SOCKET maxNum = MAX(conn, infd);

        if (select(maxNum + 1, &readfds, NULL, NULL, NULL) < 0) {
            if(errno == EINTR) {
                continue;
            }
            err_crit("failed select system call");
        }

        if (FD_ISSET(infd, &readfds)) {
            i64 counter = readn(infd, readbuff, BUFFER_LENGTH);
            if(counter < 0) {
                printf("Occured error on read from stdin error: %s\n", strerror(errno));
                stdineof = true;
                shutdown(conn, SHUT_WR);
                continue;
            }
            else if (counter == 0) {
                printf("All data readed from stdin\n");
                stdineof = true;
                shutdown(conn, SHUT_WR);
                continue;
            }
            printf("from stdin readed %ld\n", counter);

            counter = writen(conn, readbuff, counter);
            printf("%ld writed\n", counter);
        }
        else if(FD_ISSET(conn, &readfds)) {
            i64 counter = readn(conn, readbuff, BUFFER_LENGTH);
            if(counter == 0) {
                if(stdineof) {
                    printf("read FIN from server and FIN accepted from stdin leave\n");
                    break;
                }
                err_crit("Server send FIN but stdin not eof");
            }
            printf("%ld readed\n", counter);

            counter = writen(fileno(stdout), readbuff, counter);
            printf("writed to stdout %ld\n", counter);
        }
    }

    close(conn);
    return 0;
}