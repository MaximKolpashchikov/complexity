#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "types.h"

int main (int argc, char **argv)
{
    srand(time(NULL));

    unsigned size = 10;
    double scale = 10;
    double shift = 0;
    int prec = 2;

    if (argc > 1) size = strtoul(argv[1], NULL, 10);
    if (argc > 2) scale = strtod(argv[2], NULL);
    if (argc > 3) shift = strtod(argv[3], NULL);
    if (argc > 4) prec = strtol(argv[4], NULL, 10);

    matrix a = mtx_new(size);
    mtx_fill(a, scale, shift);

    matrix b = mtx_cpy(a, size / 2, 0, 0);
    matrix c = mtx_cpy(a, size / 2, size / 2, size / 2);

    matrix d, e, f;
    d = e = f = NULL;

    mtx_sum(b, c, &d);
    mtx_sub(b, c, &e);
    mtx_mult(b, c, &f);

    mtx_print(a, prec);
    puts("----------------------------------------------------------------");
    mtx_print(b, prec);
    puts("----------------------------------------------------------------");
    mtx_print(c, prec);
    puts("----------------------------------------------------------------");
    mtx_print(d, prec);
    puts("----------------------------------------------------------------");
    mtx_print(e, prec);
    puts("----------------------------------------------------------------");
    mtx_print(f, prec);

    mtx_free(a);
    mtx_free(b);
    mtx_free(c);
    mtx_free(d);
    mtx_free(e);
    mtx_free(f);

    return 0;
}