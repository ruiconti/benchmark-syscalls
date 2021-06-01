#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define DEBUG 1

static volatile long __end = 0; 
/*
 * struct timespec
 * time_t tv_sec
 *      The number of whole seconds elapsed since the epoch (for a simple calendar time) or since some other starting point (for an elapsed time).
 * long int tv_nsec
 *      The number of nanoseconds elapsed since the time given by the tv_sec member.
 *
 * In order to have a full second and nanosecond resolution, we could
 *  i.  Multiply tv_nsec by 10e-9 and sum to tv_sec
 *  ii. Multiply tv_sec by 10e9 and sum to tv_nsec
 * We opt to option ii. in order to avoid floating points approximation errors.
 */

long timer()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    if (DEBUG) 
        printf("tv_sec: %ld\ntv_nsec: %ld\n", (long)ts.tv_sec, ts.tv_nsec);
    long nsec = (long long unsigned)ts.tv_sec * 1000000000 + ts.tv_nsec;
    if (DEBUG)
        printf("tv_sec + tv_nsec: %ld\n", nsec);
    return nsec;
}

long clock_resolution()
{
    struct timespec r;
    clock_getres(CLOCK_REALTIME, &r);
    double quartz_f = 1/(r.tv_nsec * 10e-9);
    printf("CLOCK_REALTIME resolution of %ldns = %ldµs\n", r.tv_nsec, r.tv_nsec/1000);
    printf("Quartz oscillation = %gHz\n", quartz_f);
    return r.tv_nsec;
}

void ctrlchandler() 
{
    __end = timer();
}


int main(void)
{
    long start = timer();
    signal(SIGINT, ctrlchandler);
    clock_resolution();
    printf("Press CTRL-C to stop timer\n");
    while (__end == 0)
    {
        ;
    }
    printf("Time ran: %ld\n", __end - start); 

    return 0;
}
