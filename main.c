#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "mtx.h"

int main (int argc, char **argv)
{
    srand(time(NULL));

    unsigned size = 8;
    double scale = 10;
    double shift = 0;
    int flag = 0;

    struct timespec ts1, ts2, ts3;

    if (argc > 1) size = strtoul(argv[1], NULL, 10);
    if (argc > 2) scale = strtod(argv[2], NULL);
    if (argc > 3) shift = strtod(argv[3], NULL);
    if (argc > 4) flag = strtol(argv[4], NULL, 10);

    matrix a = mtx_new(size, size);
    mtx_fill(a, scale, shift);

    matrix b = mtx_new(size, size);
    mtx_fill(b, scale, shift);

    matrix c = mtx_new(size, size);

    clock_gettime(CLOCK_MONOTONIC, &ts1);

    if (flag) mtx_strassen(a, b, c);
    else mtx_mult(a, b, c);

    clock_gettime(CLOCK_MONOTONIC, &ts2);

    ts3.tv_sec = ts2.tv_sec - ts1.tv_sec;
    ts3.tv_nsec = ts2.tv_nsec - ts1.tv_nsec;

    if (ts3.tv_nsec < 0)
    {
        ts3.tv_sec--;
        ts3.tv_nsec += 10e9;
    }

    printf("Execution time: %ld.%ld seconds\n", ts3.tv_sec, ts3.tv_nsec);

    mtx_free(a);
    mtx_free(b);
    mtx_free(c);

    return 0;
}