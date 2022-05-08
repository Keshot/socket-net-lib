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
    char ipaddrPres[IPV4P_STRLEN];
    char *rdBuffer = new char[BUFFER_LENGTH];
    SOCKET serverfd = INVALID_SOCKET, acceptedfd = INVALID_SOCKET;
    
    pid_t processID = getpid();
    printf("Server process ID is %d\n", processID);

    int64_t max = __INT64_MAX__;
    printf("%ld\n", max);

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

    union ttttuuu
    {
        int a;
        char b[sizeof(int)];
    };
    
    ttttuuu oi;
    oi.a = -22;

    if( (serverfd = socket(PROTO_FAMILY_IPV4, SOCK_STREAM, TCP_PROTO)) <= INVALID_SOCKET) {
        err_crit("socket system call failed");
    }

    if( bind(serverfd, reinterpret_cast<sockaddr*>(&sa), sizeof(sockaddr_in)) < 0) {
        err_crit("bind system call failed");
    }

    if( listen(serverfd, LISTEN_QUEUE) < 0) {
        err_crit("listen system call failed");
    }

    if (sock_get_local_addr(serverfd, ipaddrPres, IPV4P_STRLEN) != 1) {
        err_log("sock_get_local_addr failed");
    }
    else {
        printf("server local ip: %s\tport: %hu\n", ipaddrPres, sock_get_port(ccast(const sockaddr*, &sa)));
    }

    while(true) {
        sockaddr_unify addr;
        socklen_t len = sizeof(sockaddr_unify);
        acceptedfd = accept(serverfd, ccast(sockaddr*, &addr), &len);
        if (acceptedfd < 0) {
            if(errno == EINTR || errno == ECONNABORTED) {
                continue;
            }
            err_crit("accept system call failed");
        }

        processID = fork();
        if (processID == 0) {
            printf("Process start by child process with ID %d\n", getpid());
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

            while(true) {
                i64 readed = readn(acceptedfd, rdBuffer, BUFFER_LENGTH);
                printf("From client readed %ld bytes\n", readed);        
                if(readed == 0) {
                    break;
                }

                writen(acceptedfd, rdBuffer, readed);
            }

            printf("Child process complete processing exit\n");
            exit(0);
        }
        printf("Start processing clinet in process with ID %d\n", processID);
        close(acceptedfd);
    }

    return 0;
}