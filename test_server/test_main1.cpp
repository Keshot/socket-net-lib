#include <core/core.h>
#include <utils/error_handler.h>

typedef struct sigaction ssigaction;

void printSigSet(const sigset_t *set)
{
    int cnt = 0;

    if(!sigisemptyset(set)) {
        for(int sig = 1; sig < NSIG; ++sig) {
            if (sigismember(set, sig)) {
                ++cnt;
                fprintf(stdout, "signal:%d (%s)\n", sig, strsignal(sig));
            }
        }
    }
    else {
        fprintf(stdout, "set is empty\n");
    }
}

void sig_int_handler(int signum)
{
    printf("SIGINT was triggered\n");
}

void sig_usr_handler(int signum)
{
    sigset_t pendingSignals;
    sigemptyset(&pendingSignals);
    printf("signal number - %d(%s)\n", signum, strsignal(signum));
    for(int j = 1; j <= 40; ++j) {
        sigpending(&pendingSignals);
        printf("%d: pending set\n");
        printSigSet(&pendingSignals);
        sigemptyset(&pendingSignals);
        sleep(2);
    }
}

int strtoint(const char *str)
{
    int res = 0;

    for(; *str != '\0' ; ++str) {
        if(*str < '0' || *str > '9') {
            return -1;
        }
        res *= 10;
        res += (*str - '0');
    }

    return res;
}

int main(int argc, char *argv[])
{
    if(argc <= 1) {
        printf("Usage: ./sigsender (target)PID\n");
        return 1;
    }

    pid_t target_pid = strtoint(argv[1]);
    printf("target PID is %d\n", target_pid);

    kill(target_pid, 34);
    printf("signal 34 send(%s)\n", strsignal(34));

    printf("sleep(5)\n");
    sleep(2);

    kill(target_pid, 34);
    printf("signal 34 send(%s)\n", strsignal(34));

    for(int j = 0; j < 10; ++j) {
        kill(target_pid, 35);
        printf("%d:signal 35 send(%s)\n", j, strsignal(35));
        sleep(2);
    }
    
    for(int j = 0; j < 5; ++j) {
        kill(target_pid, SIGINT);
        printf("%d:signal SIGINT send(%s)\n", j, strsignal(SIGINT));
        sleep(2);
    }

    for(int j = 0; j < 5; ++j) {
        kill(target_pid, SIGTRAP);
        printf("%d:signal SIGTRAP send(%s)\n", j, strsignal(SIGTRAP));
        sleep(2);
    }

    /*
    printf("Process ID %d", getpid());

    ssigaction sigact;

    sigemptyset(&sigact.sa_mask);

    sigact.sa_handler = &sig_usr_handler;
    sigaddset(&sigact.sa_mask, 35);

    sigaction(34, &sigact, NULL);

    pause();

    
    sigset_t blockMask, prevMask;

    sigemptyset(&blockMask);
    sigemptyset(&prevMask);

    sigaddset(&blockMask, SIGINT);
    printSigSet(&blockMask);
    sigprocmask(SIG_BLOCK, &blockMask, &prevMask);

    printf("prev blocking mask\n");
    printSigSet(&prevMask);

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        err_crit("signal call");
    }

    for(int j = 0;;) {
        printf("%d and sleep\n", j++);
        sleep(5);
        if(j == 10) {
            break;
        }
    }

    sigprocmask(SIG_SETMASK, &prevMask, NULL);

    for(int j = 0;;) {
        printf("%d and sleep\n", j++);
        sleep(5);
        if(j == 10) {
            break;
        }
    }
    */

    return 0;
}