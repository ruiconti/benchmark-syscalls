#include <stdio.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>


#define false 0
#define true 1
#define TICKS_PER_SEC sysconf(_SC_CLK_TCK)
#define RUNS 1e7
#define DEBUG false

long timer()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long unsigned)ts.tv_sec * 1000000000 + ts.tv_nsec;
}


int main(int argc, char *argv[])
{
    int fd = open("./bench-read.c", O_RDONLY), runc;
    double start = timer(), secs = 0, end;
    if (argv[1] != NULL)
        runc = strtol(argv[1], NULL, 10);
    else 
        runc = RUNS;

    for(int i = 0; i < runc; i++)
    {
        double t0 = timer();
        read(fd, NULL, 0);  // Numb system call 
        double t = timer() - t0;
        secs += t;
    }
    end = timer() - start;
    // Reporting
    printf("avg time: %gns (%g read() calls in %gs (systime))\n", secs/runc, (double)runc, end * 1e-9);
    return 0;
}
