#include "core/core.h"
#include "core/net_socket.h"
#include "utils/error_handler.h"

#include <stdio.h>

#define MAXLINE 1024
#define BUFFER_LENGTH 100000

static void getSendText(char *&text, i64 &len)
{
    static char static_text[] = "hello world how are you man hello sepper mere asd acxcasdasdkamxc casdfasfas asdwww dkasodkasodmmm nmcaksdsada";
    text = static_text;
    len = sizeof(static_text);
}

int main(int argc, char *argv[])
{
    char *readbuff = new char[BUFFER_LENGTH];
    SOCKET conn = INVALID_SOCKET;
    sockaddr_in sa;

    btzero(reinterpret_cast<u8*>(&sa), sizeof(sockaddr_in));

    sa.sin_family = ADDR_FAMILY_IPV4;
    sa.sin_port = htons(5643);

    if (inet_pton(ADDR_FAMILY_IPV4, "127.0.0.1", reinterpret_cast<void*>(&sa.sin_addr)) <= 0) {
        err_crit("inet_pton call failed");
    }

    if ( (conn = socket(ADDR_FAMILY_IPV4, SOCK_STREAM, TCP_PROTO)) <= INVALID_SOCKET) {
        err_crit("socket system call failed");
    }

    if (connect(conn, reinterpret_cast<sockaddr*>(&sa), sizeof(sockaddr_in)) < 0) {
        err_crit("connect system call failed");
    }

    sockaddr_unify addr;
    socklen_t len = sizeof(addr);
    if (getsockname(conn, ccast(sockaddr*, &addr), &len) < 0) {
        err_log("getsockname failed");
    }
    else {
        i32 ret = sock_ntop(readbuff, IPV4P_STRLEN, ccast(const sockaddr*, &addr));
        if(ret != 1) {
            err_log("sock_ntop failed");
        }
        else {
            printf("server local ip: %s\tport: %u\n", readbuff, sock_get_port(ccast(const sockaddr*, &sa)));
        }
    }

    while(true) {
        char *textp;
        i64 text_len;
        getSendText(textp, text_len);

        i64 counter = writen(conn, textp, text_len);
        printf("%ld writed\n", counter);

        counter = readn(conn, readbuff, BUFFER_LENGTH);
        printf("%ld readed\n", counter);

        close(conn);
        break;
    }

    return 0;
}