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
    int prec = 2;

    if (argc > 1) size = strtoul(argv[1], NULL, 10);
    if (argc > 2) scale = strtod(argv[2], NULL);
    if (argc > 3) shift = strtod(argv[3], NULL);
    if (argc > 4) prec = strtol(argv[4], NULL, 10);

    matrix a = mtx_new(size, size);
    mtx_fill(a, scale, shift);
    mtx_print(a, prec);
    printf("\n");

    matrix b = mtx_new(size, size);
    mtx_fill(b, scale, shift);
    mtx_print(b, prec);
    printf("\n");

    matrix c = mtx_new(size, size);
    mtx_strassen(a, b, c);
    mtx_print(c, prec);
    printf("\n");

    matrix d = mtx_new(size, size);
    mtx_mult(a, b, d);
    mtx_print(d, prec);
    printf("\n");

    mtx_free(a);
    mtx_free(b);
    mtx_free(c);
    mtx_free(d);

    return 0;
}