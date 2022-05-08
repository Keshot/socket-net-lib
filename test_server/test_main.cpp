#include <core/core.h>
#include <utils/error_handler.h>

typedef struct sigaction ssigaction;

void printSigSet(const sigset_t *set)
{
    int cnt = 0;

    for(int sig = 1; sig < 64; ++sig) {
        if (sigismember(set, sig)) {
            ++cnt;
            fprintf(stdout, "signal:%d (%s)\n", sig, strsignal(sig));
        }
    }

    if(cnt <= 0) {
        fprintf(stdout, "set is empty\n");
    }
}

void sig_trap_handler(int signum)
{
    printf("signal SIGTRAP was triggered\n");
}

void sig_usr1_handler(int signum)
{
    printf("signal 35 was triggered\n");
}

void sig_int_handler(int signum)
{
    printf("SIGINT was triggered\n");
}

void sig_usr_handler(int signum)
{
    static int cnt = 0;

    sigset_t pendingSignals, blockedSigMask;

    printf("%d touch\n", ++cnt);

    sigemptyset(&pendingSignals);
    sigemptyset(&blockedSigMask);

    sigprocmask(SIG_SETMASK, NULL, &blockedSigMask);
    printf("blocked signals set\n");
    printSigSet(&blockedSigMask);

    printf("signal number - %d(%s)\n", signum, strsignal(signum));
    for(int j = 1; j <= 40; ++j) {
        sigpending(&pendingSignals);
        printf("%d: pending set\n", j);
        printSigSet(&pendingSignals);
        sigemptyset(&pendingSignals);
        printf("sleep(2)\n");
        sleep(2);
    }
}

int main(int argc, char *argv[])
{
    printf("Process ID %d\n", getpid());

    ssigaction sigact;

    sigemptyset(&sigact.sa_mask);
    
    sigact.sa_handler = &sig_usr_handler;
    sigaddset(&sigact.sa_mask, 35);
    sigaddset(&sigact.sa_mask, SIGINT);
    sigaddset(&sigact.sa_mask, SIGTRAP);
    sigact.sa_flags |= SA_NODEFER;

    sigaction(34, &sigact, NULL);
    signal(SIGINT, &sig_int_handler);
    signal(35, &sig_usr1_handler);
    signal(SIGTRAP, &sig_trap_handler);

    printf("pause()\n");
    pause();

    /*
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