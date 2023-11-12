#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"

struct _buf
{
    // Reference count
    unsigned ref;
    // arr has size of len * len
    unsigned len;
    // Actual array
    double arr[];
};

struct _mtx
{
    // Virtual matrix size
    unsigned b_len;
    // Matrix row offset
    unsigned r_off;
    // Matrix column offset
    unsigned c_off;
    // Matrix body
    struct _buf *b;
};

struct _buf *buf_new (unsigned width)
{
    struct _buf *ptr = malloc(sizeof(struct _buf) + width * width * sizeof(double));
    ptr->ref = 1;
    ptr->len = width;
    memset(ptr->arr, 0, width * width * sizeof(double));
    return ptr;
}

struct _buf *buf_cpy (struct _buf *ptr)
{
    ++ptr->ref;
    return ptr;
}

void buf_free (struct _buf *ptr)
{
    if (--ptr->ref == 0) free(ptr);
}

matrix mtx_new (unsigned width)
{
    matrix m = malloc(sizeof(struct _mtx));
    m->b_len = width;
    m->r_off = 0;
    m->c_off = 0;
    m->b = buf_new(width);
    return m;
}

matrix mtx_cpy (matrix m, unsigned width, unsigned r_off, unsigned c_off)
{
    matrix n = malloc(sizeof(struct _mtx));
    n->b_len = width;
    n->r_off = m->r_off + r_off;
    n->c_off = m->c_off + c_off;
    n->b = buf_cpy(m->b);
    return n;
}

unsigned mtx_size (matrix m)
{
    return m->b_len;
}

double mtx_get (matrix m, unsigned i, unsigned j)
{
    return m->b->arr[(i + m->r_off) * m->b->len + (j + m->c_off)];
}

double mtx_set (matrix m, unsigned i, unsigned j, double x)
{
    return m->b->arr[(i + m->r_off) * m->b->len + (j + m->c_off)] = x;
}

void mtx_free (matrix m)
{
    buf_free(m->b);
    free(m);
}

void mtx_fill (matrix m, double scale, double shift)
{
    unsigned i, j;

    for (i = 0; i < m->b_len; ++i)
    {
        for (j = 0; j < m->b_len; ++j)
        {
            mtx_set(m, i, j, scale * rand() / RAND_MAX + shift);
        }
    }
}

void mtx_print (matrix m, int p)
{
    unsigned i, j;

    for (i = 0; i < m->b_len; ++i)
    {
        for (j = 0; j < m->b_len; ++j)
        {
            printf("%*.*lf", p + 6, p, mtx_get(m, i, j));
        }
        printf("\n");
    }
}

void mtx_blit (matrix a, matrix b, double k)
{
    unsigned i, j;

    for (i = 0; i < b->b_len; ++i)
    {
        for (j = 0; j < b->b_len; ++j)
        {
            mtx_set(a, i, j, mtx_get(a, i, j) + k * mtx_get(b, i, j));
        }
    }
}

void mtx_sum (matrix a, matrix b, matrix *c, double k, double l)
{
    unsigned i, j;

    if (*c == NULL) *c = mtx_new(a->b_len);

    for (i = 0; i < a->b_len; ++i)
    {
        for (j = 0; j < a->b_len; ++j)
        {
            mtx_set(*c, i, j, k * mtx_get(a, i, j) + l * mtx_get(b, i, j));
        }
    }
}

void mtx_mult (matrix a, matrix b, matrix *c)
{
    unsigned i, j, k;
    double t;

    if (*c == NULL) *c = mtx_new(a->b_len);

    for (i = 0; i < a->b_len; ++i)
    {
        for (j = 0; j < a->b_len; ++j)
        {
            for (t = 0, k = 0; k < a->b_len; ++k)
            {
                t += mtx_get(a, i, k) * mtx_get(b, k, j);
            }
            mtx_set(*c, i, j, t);
        }
    }
}

static int SK[] = {
    0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, -1,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, -1, 1, 1,
    1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, -1, 1, 1,
    0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, -1,
    1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, -1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, -1, 1, 1, 1, 0
};

void mtx_strassen (matrix a, matrix b, matrix *c)
{
    matrix a1, a2, b1, b2, c1, c2;
    matrix s1, s2, m;
    unsigned i, q;
    int *k;

    q = a->b_len / 2;
    s1 = s2 = m = NULL;

    if (q == 1)
    {
        mtx_mult(a, b, c);
        return;
    }

    if (*c == NULL) *c = mtx_new(a->b_len);

    for (i = 0; i < 7; ++i)
    {
        k = SK + 18 * i;

        a1 = mtx_cpy(a, q, q * k[0], q * k[1]);
        a2 = mtx_cpy(a, q, q * k[2], q * k[3]);
        b1 = mtx_cpy(b, q, q * k[4], q * k[5]);
        b2 = mtx_cpy(b, q, q * k[6], q * k[7]);
        c1 = mtx_cpy(*c, q, q * k[8], q * k[9]);
        c2 = mtx_cpy(*c, q, q * k[10], q * k[11]);

        mtx_sum(a1, a2, &s1, k[12], k[13]);
        mtx_sum(b1, b2, &s2, k[14], k[15]);
        mtx_strassen(s1, s2, &m);
        mtx_blit(c1, m, k[16]);
        mtx_blit(c2, m, k[17]);

        mtx_free(a1);
        mtx_free(a2);
        mtx_free(b1);
        mtx_free(b2);
        mtx_free(c1);
        mtx_free(c2);
        mtx_free(m);
        m = NULL;
    }

    mtx_free(s1);
    mtx_free(s2);
}