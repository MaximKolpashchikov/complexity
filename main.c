#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void load_data (FILE *fp, matrix *a, matrix *b, unsigned *n)
{
    fscanf(fp, "%u", n);
    *a = mtx_load(fp, *n, *n);
    *b = mtx_load(fp, *n, *n);
}

void print_help (const char *prog)
{
    printf("Использование: %s [опции]\n", prog);
    puts("\nОпции генератора:");
    puts("-s <число>\t задаёт размер матриц, по умолчанию 8");
    puts("-m <число>\t задаёт максимальное значение элементов, по умолчанию 10");
    puts("-o <число>\t задаёт смещение значений элементов, по умолчанию 0");
    puts("\nОпции ввода:");
    puts("-g\t\t включает автоматическую генерацию входных данных, по умолчанию отключено");
    puts("-f <путь>\t включает чтение входных данных из файла, по умолчанию читает стандартный ввод");
    puts("\nОпции вывода:");
    puts("-q\t\t включает тихий режим, по умолчанию отключено");
    puts("-p <число>\t задаёт число знаков после запятой, по умолчанию 2");
    exit(0);
}

int main (int argc, char **argv)
{
    // Initialize random sequence
    srand(time(NULL));

    // Default generator parameters
    unsigned size = 8;
    double scale = 10;
    double shift = 0;

    // Input parameters
    int gene = 0;
    char *path = NULL;

    // Output parameters
    int quiet = 0;
    int prec = 2;

    // Prepare necessary variables
    matrix a, b, c, d;
    struct timespec ts1, ts2, tr1, tr2;
    int opt;

    // Parse command line options
    while ((opt = getopt(argc, argv, "s:m:o:gf:qp:h")) != -1)
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
            case 'g':
                gene = 1;
            break;
            case 'f':
                path = strdup(optarg);
            break;
            case 'q':
                quiet = 1;
            break;
            case 'p':
                prec = strtol(optarg, NULL, 10);
            break;
            case 'h':
                print_help(argv[0]);
            break;
        }
    }

    // Prepare input matrices
    if (gene)
    {
        a = mtx_new(size, size);
        b = mtx_new(size, size);
        mtx_fill(a, scale, shift);
        mtx_fill(b, scale, shift);
    }
    else
    if (path)
    {
        FILE *io = fopen(path, "rt");
        load_data(io, &a, &b, &size);
        fclose(io);
        free(path);
    }
    else
    {
        load_data(stdin, &a, &b, &size);
    }

    // Prepare result matrices
    c = mtx_new(size, size);
    d = mtx_new(size, size);

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