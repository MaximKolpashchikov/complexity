#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "mtx.h"

void measure_time (struct timespec *a, struct timespec *b, struct timespec *c)
{
    c->tv_sec = b->tv_sec - a->tv_sec;
    c->tv_nsec = b->tv_nsec - a->tv_nsec;

    if (c->tv_nsec < 0)
    {
        c->tv_sec--;
        c->tv_nsec += 1e9;
    }
}

int main (int argc, char **argv)
{
    // Initialize random sequence
    srand(time(NULL));

    // Prepare program parameters
    unsigned size = 8;
    double scale = 10;
    double shift = 0;
    int prec = 2;
    int quiet = 0;

    // Prepare necessary variables
    struct timespec ts1, ts2, tr1, tr2;
    int opt;

    // Parse command line options
    while ((opt = getopt(argc, argv, "s:m:o:p:q")) != -1)
    {
        switch (opt)
        {
            case 's':
                size = strtoul(optarg, NULL, 10);
            break;
            case 'm':
                scale = strtod(optarg, NULL);
            break;
            case 'o':
                shift = strtod(optarg, NULL);
            break;
            case 'p':
                prec = strtol(optarg, NULL, 10);
            break;
            case 'q':
                quiet = 1;
            break;
            default:
                fprintf(stderr, "Usage: %s [-s size] [-m scale] [-o shift] [-p precision] [-q]\n", argv[0]);
            break;
        }
    }

    // Prepare all matrices
    matrix a = mtx_new(size, size);
    mtx_fill(a, scale, shift);

    matrix b = mtx_new(size, size);
    mtx_fill(b, scale, shift);

    matrix c = mtx_new(size, size);
    matrix d = mtx_new(size, size);

    // Run classic algorithm
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    mtx_mult(a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    measure_time(&ts1, &ts2, &tr1);

    // Run optimized algorithm
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    mtx_strassen(a, b, d);
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    measure_time(&ts1, &ts2, &tr2);

    // Print results if necessary
    if (!quiet)
    {
        mtx_print(a, prec);
        mtx_print(b, prec);
        mtx_print(c, prec);
        mtx_print(d, prec);
    }

    // Print execution time of algorithms
    printf("Execution time (classic):  %ld.%ld seconds\n", tr1.tv_sec, tr1.tv_nsec);
    printf("Execution time (Strassen): %ld.%ld seconds\n", tr2.tv_sec, tr2.tv_nsec);

    // Clean all used memory
    mtx_free(a);
    mtx_free(b);
    mtx_free(c);
    mtx_free(d);

    return 0;
}