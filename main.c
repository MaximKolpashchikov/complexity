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
    int flag = 0;

    if (argc > 1) size = strtoul(argv[1], NULL, 10);
    if (argc > 2) scale = strtod(argv[2], NULL);
    if (argc > 3) shift = strtod(argv[3], NULL);
    if (argc > 4) prec = strtol(argv[4], NULL, 10);
    if (argc > 5) flag = strtol(argv[5], NULL, 10);

    matrix a = mtx_new(size, size);
    mtx_fill(a, scale, shift);

    matrix b = mtx_new(size, size);
    mtx_fill(b, scale, shift);

    matrix c = mtx_new(size, size);

    if (flag) mtx_strassen(a, b, c);
    else mtx_mult(a, b, c);

    mtx_free(a);
    mtx_free(b);
    mtx_free(c);

    return 0;
}