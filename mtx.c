#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mtx.h"

struct _buf
{
    // Reference count
    unsigned ref;
    // arr has row breakpoint brp
    unsigned brp;
    // Actual array
    double arr[];
};

struct _mtx
{
    // Virtual matrix size
    unsigned b_rows;
    unsigned b_cols;
    // Matrix offset
    unsigned r_off;
    unsigned c_off;
    // Matrix body
    struct _buf *b;
};

#define buf_len(r, c) (r) * (c) * sizeof(double)
#define buf_get(b, i, j) (b)->arr[(i) * (b)->brp + (j)]

struct _buf *buf_new (unsigned rows, unsigned cols)
{
    struct _buf *ptr = malloc(sizeof(struct _buf) + buf_len(rows, cols));
    ptr->ref = 1;
    ptr->brp = cols;
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

matrix mtx_new (unsigned rows, unsigned cols)
{
    matrix m = malloc(sizeof(struct _mtx));
    m->b_rows = rows;
    m->b_cols = cols;
    m->r_off = 0;
    m->c_off = 0;
    m->b = buf_new(rows, cols);
    // DEBUG
    // fprintf(stderr, "Created new matrix sized %u x %u\n", rows, cols);
    return m;
}

matrix mtx_cpy (matrix m, unsigned rows, unsigned cols, unsigned roff, unsigned coff)
{
    matrix n = malloc(sizeof(struct _mtx));
    n->b_rows = rows;
    n->b_cols = cols;
    n->r_off = m->r_off + roff;
    n->c_off = m->c_off + coff;
    n->b = buf_cpy(m->b);
    // DEBUG
    // fprintf(stderr, "Created new matrix copy sized %u x %u\n", rows, cols);
    return n;
}

double mtx_get (matrix m, unsigned i, unsigned j)
{
    return buf_get(m->b, i + m->r_off, j + m->c_off);
}

double mtx_set (matrix m, unsigned i, unsigned j, double x)
{
    return buf_get(m->b, i + m->r_off, j + m->c_off) = x;
}

void mtx_free (matrix m)
{
    buf_free(m->b);
    free(m);
}

void mtx_null (matrix m)
{
    memset(m->b->arr, 0, buf_len(m->b_rows, m->b_cols));
}

void mtx_fill (matrix m, double scale, double shift)
{
    unsigned i, j;

    for (i = 0; i < m->b_rows; ++i)
    {
        for (j = 0; j < m->b_cols; ++j)
        {
            mtx_set(m, i, j, scale * rand() / RAND_MAX + shift);
        }
    }
}

void mtx_print (matrix m, int p)
{
    unsigned i, j;

    for (i = 0; i < m->b_rows; ++i)
    {
        for (j = 0; j < m->b_cols; ++j)
        {
            printf("%*.*lf", p + 6, p, mtx_get(m, i, j));
        }
        printf("\n");
    }
}

void mtx_blit (matrix s, matrix d, double k)
{
    unsigned i, j;
    double t;

    for (i = 0; i < d->b_rows; ++i)
    {
        for (j = 0; j < d->b_cols; ++j)
        {
            if (i < s->b_rows && j < s->b_cols)
            {
                t = mtx_get(d, i, j) + k * mtx_get(s, i, j);
                mtx_set(d, i, j, t);
            }
        }
    }
}

void mtx_sum (matrix a, matrix b, matrix c, double k, double l)
{
    unsigned i, j;
    double t;

    for (i = 0; i < c->b_rows; ++i)
    {
        for (j = 0; j < c->b_cols; ++j)
        {
            t = 0;
            if (i < a->b_rows && j < a->b_cols)
            {
                t += k * mtx_get(a, i, j);
            }
            if (i < b->b_rows && j < b->b_cols)
            {
                t += l * mtx_get(b, i, j);
            }
            mtx_set(c, i, j, t);
        }
    }
}

void mtx_mult (matrix a, matrix b, matrix c)
{
    unsigned i, j, k;
    double t;

    assert(a->b_cols == b->b_rows);

    for (i = 0; i < a->b_rows; ++i)
    {
        for (j = 0; j < b->b_cols; ++j)
        {
            for (t = 0, k = 0; k < a->b_cols; ++k)
            {
                t += mtx_get(a, i, k) * mtx_get(b, k, j);
            }
            mtx_set(c, i, j, t);
        }
    }
}

matrix mtx_quarter (matrix m, unsigned h, unsigned x, unsigned y)
{
    unsigned rs = x == 0 ? h : m->b_rows - h;
    unsigned cs = y == 0 ? h : m->b_cols - h;
    return mtx_cpy(m, rs, cs, h * x, h * y);
}

static const int SK[] = {
    0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, -1,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, -1, 1, 1,
    1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, -1, 1, 1,
    0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, -1,
    1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, -1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, -1, 1, 1, 1, 0
};

void mtx_strassen (matrix a, matrix b, matrix c)
{
    matrix a1, a2, b1, b2, c1, c2;
    matrix s1, s2, m;
    unsigned i, q;
    const int *k;

    assert(a->b_cols == b->b_rows);

    q = a->b_rows / 2;
    if (a->b_rows % 2) ++q;

    if (q <= 100)
    {
        mtx_mult(a, b, c);
        return;
    }

    s1 = mtx_new(q, q);
    s2 = mtx_new(q, q);
    m = mtx_new(q, q);

    mtx_null(c);

    for (i = 0; i < 7; ++i)
    {
        k = SK + 18 * i;

        a1 = mtx_quarter(a, q, k[0], k[1]);
        a2 = mtx_quarter(a, q, k[2], k[3]);
        b1 = mtx_quarter(b, q, k[4], k[5]);
        b2 = mtx_quarter(b, q, k[6], k[7]);
        c1 = mtx_quarter(c, q, k[8], k[9]);
        c2 = mtx_quarter(c, q, k[10], k[11]);

        mtx_sum(a1, a2, s1, k[12], k[13]);
        mtx_sum(b1, b2, s2, k[14], k[15]);
        mtx_strassen(s1, s2, m);
        mtx_blit(m, c1, k[16]);
        mtx_blit(m, c2, k[17]);

        mtx_free(a1);
        mtx_free(a2);
        mtx_free(b1);
        mtx_free(b2);
        mtx_free(c1);
        mtx_free(c2);
    }

    mtx_free(s1);
    mtx_free(s2);
    mtx_free(m);
}