#include "core.h"
#include <sys/wait.h>
#include <errno.h>

i32 check_byte_order()
{
    short t = 0x0102;
    u8 *tx = ccast(u8*, &t);
    return *tx == 0x01 ? BIG_ENDIAN : LITTLE_ENDIAN;
}

void sigchild_handler(int signum)
{
    int olderrno = errno;
    pid_t childID = 0;
    int stat = 0;

    while(true) {
        if( (childID = waitpid(-1, &stat, WNOHANG)) > 0) {
            printf("child process with ID %d terminated %s\n", childID, WIFEXITED(stat) ? "normaly" : "not normaly");
            continue;
        }
        else if(childID == 0) {
            printf("all child process are handled\n");
        }
        else {
            printf("error %s!\n", strerror(errno));
        }
        break;
    }

    errno = olderrno;
    return;
}