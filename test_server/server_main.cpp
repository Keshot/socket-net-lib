#include "core/core.h"
#include "core/net_socket.h"
#include "utils/error_handler.h"

#include <time.h>
#include <string.h>
#include <errno.h>

#define BUFFER_LENGTH 100000

int main(int argc, char *argv[])
{
    struct sigaction act;
    int maxfd, maxi = -1;
    int clients[FD_SETSIZE] { -1 };
    fd_set allfds, readfds;
    char ipaddrPres[IPV4P_STRLEN];
    char *rdBuffer = new char[BUFFER_LENGTH];
    SOCKET serverfd = INVALID_SOCKET;
    
    pid_t processID = getpid();
    printf("Server process ID is %d\n", processID);

    act.sa_handler = &sigchild_handler;
    act.sa_flags = 0;
    act.sa_flags |= SA_RESTART;
    sigemptyset(&act.sa_mask);
    sigfillset(&act.sa_mask);

    sigaction(SIGCHLD, &act, NULL);

    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    
    sigaction(SIGPIPE, &act, NULL);

    // i64 pagesize = sysconf(_SC_PAGE_SIZE);

    sockaddr_in sa;
    btzero(reinterpret_cast<u8*>(&sa), sizeof(sockaddr_in));

    sa.sin_family = ADDR_FAMILY_IPV4;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(5643);

    FD_ZERO(&allfds);

    if( (serverfd = socket(PROTO_FAMILY_IPV4, SOCK_STREAM, TCP_PROTO)) <= INVALID_SOCKET) {
        err_crit("socket system call failed");
    }

    if( bind(serverfd, reinterpret_cast<sockaddr*>(&sa), sizeof(sockaddr_in)) < 0) {
        err_crit("bind system call failed");
    }

    if( listen(serverfd, LISTEN_QUEUE) < 0) {
        err_crit("listen system call failed");
    }

    maxfd = serverfd;
    FD_SET(serverfd, &allfds);

    if (sock_get_local_addr(serverfd, ipaddrPres, IPV4P_STRLEN) != 1) {
        err_log("sock_get_local_addr failed");
    }
    else {
        printf("server local ip: %s\tport: %hu\n", ipaddrPres, sock_get_port(ccast(const sockaddr*, &sa)));
    }

    printf("Start maxfd %d\n", maxfd);
    while(true) {
        readfds = allfds;
        int readyfd = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(serverfd, &readfds)) {
            sockaddr_unify addr;
            socklen_t len = sizeof(sockaddr_unify);
            SOCKET acceptedfd;

            while (true) {
                len = sizeof(sockaddr_unify);
                acceptedfd = accept(serverfd, ccast(sockaddr*, &addr), &len);
                if (acceptedfd < 0) {
                    if(errno == EINTR || errno == ECONNABORTED || errno == EPROTO) {
                        continue;
                    }
                    err_crit("accept system call failed");
                }
                break;
            }

            if(sock_ntop(ipaddrPres, IPV4P_STRLEN, ccast(const sockaddr*, &addr)) != 1) {
                err_log("sock_ntop failed");
            }
            else {
                printf("accepted ip: %s\tport: %hu\n", ipaddrPres, sock_get_port(ccast(const sockaddr*, &addr)));

                len = sizeof(addr);
                
                if (sock_get_local_addr(acceptedfd, ipaddrPres, IPV4P_STRLEN) != 1) {
                    err_log("sock_get_local_addr failed");
                }
                else {
                    printf("accepted local ip: %s\n", ipaddrPres);
                }
            }

            int i = 0;
            for (; i < FD_SETSIZE; ++i) {
                if (clients[i] < 0) {
                    clients[i] = acceptedfd;
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                printf("Too many connection accpeted disconnect new client\n");
                close(acceptedfd); // dangerous
            }
            else {
                printf("New client id %d\n", i);
                FD_SET(acceptedfd, &allfds);
                if (acceptedfd > maxfd) {
                    maxfd = acceptedfd;
                    printf("New max fd set %d\n", maxfd);
                }

                if (i > maxi) {
                    maxi = i;
                    printf("New max client ID set %d\n", maxi);
                }
            }

            if (--readyfd <= 0) {
                printf("All ready fd processed continue select\n");
                continue;
            }
        }

        for (int i = 0; i <= maxi; ++i) {
            SOCKET sock = clients[i];
            if (sock < 0) {
                continue;
            }
            if (FD_ISSET(sock, &readfds)) {
                i64 readed = readn(sock, rdBuffer, BUFFER_LENGTH);
                printf("From client ID %d readed %ld bytes\n", sock, readed);
                if(readed == 0) {
                    clients[i] = -1;
                    FD_CLR(sock, &allfds);

                    if (i == maxi) {
                        int j = i;
                        for (; j >= 0; --j) {
                            if(clients[j] >= 0) {
                                maxi = j;
                                break;
                            }
                        }

                        if (j < 0) {
                            maxi = -1;
                        }

                        printf("New maxi %d\n", maxi);
                    }

                    if (sock == maxfd) {
                        maxfd = serverfd;
                        for (int k = 0; k < maxi; ++k) {
                            SOCKET sck = clients[k];
                            if (sck > maxfd) {
                                maxfd = sck;
                            }
                        }

                        printf("New maxfd %d\n", maxfd);
                    }

                    close(sock);
                }
                else {
                    readed = writen(sock, rdBuffer, readed);
                    printf("Writed %ld to client\n", readed);
                }

                if (--readyfd <= 0) {
                    continue;
                }
            }
        }
    }

    return 0;
}