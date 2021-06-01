#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

static char* PING = "ping\n";
static char* PONG = "pong\n";
#define _GNU_SOURCE
#define DATA_LEN strlen(PING)
#define DATA_SZ sizeof(char)*DATA_LEN

static volatile long t_ref = 0; 
static volatile long t_sum = 0;
static volatile int t_count = 0;
static volatile int _ctrl = 1;

void sighandler()
{
    _ctrl = 0;
}


char *answer_to(char *msg) {
    if (strcmp(msg, PING) == 0)
        return PONG;
    return PING;
}

/*
 * Return time in nanosecond resolution
 */
long timer()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long unsigned)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

void endless_chat(int *pread, int *pwrite)
{
    char *bread, *bwrite;
    while (_ctrl)
    {
        // Delta times the difference between when the thread last ran and now
        // when it is currently running.
        // It does NOT times how long it takes to run this current block of code.
        // Rather, it tries to time *time away*.
        long t_proc = timer();
        long t_delta = t_proc - t_ref;
        // Read pipe and resolve answer.
        read(pread[0], &bread, DATA_SZ);
        bwrite = answer_to(bread);
        printf("%i:%ld %s", getpid(), t_delta, bread);
        t_ref = t_proc;
        t_count++;
        t_sum += t_delta;
        // Write out to the other process to read.
        write(pwrite[1], &bwrite, DATA_SZ);
    }
}

void chat(int *p)
{
    char *msg = PING;
    write(p[1], &msg, DATA_SZ);
}

// I want to keep sending a ping-pong data using pipes
// Before loop, writes "ping" on p0
// 1. Parent reads on p0 and writes "ping" on p1
// 2. Child reads p1 and writes on p0

int main(int argc, char *argv[])
{
    int pin[2], pout[2], cpid;
    pipe(pin);
    pipe(pout);
    signal(SIGINT, sighandler);

    cpid = fork();

    // Start off
    write(pin[1], &PING, DATA_SZ);
    if (cpid == 0)
    {
        // Child
        endless_chat(pout, pin);
    } else
    {
        // Parent
        endless_chat(pin, pout);
    }
    double t_avg = (t_sum * 10e-9) / t_count;
    printf("%i: avg time sending %d msgs: %gµs\n", getpid(), t_count, t_avg * 10e-3);
    return 0;
}
