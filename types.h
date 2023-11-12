#ifndef MTX_H
#define MTX_H

typedef struct _mtx *matrix;

// Create a new matrix sized (width, width)
matrix mtx_new (unsigned width);

// Extract a submatrix sized (width, width) at offset (r_off, c_off)
matrix mtx_cpy (matrix m, unsigned width, unsigned r_off, unsigned c_off);

// Get a size of matrix m
unsigned mtx_size (matrix m);

// Get a matrix element m(i, j)
double mtx_get (matrix m, unsigned i, unsigned j);

// Set a matrix element m(i, j) to x
double mtx_set (matrix m, unsigned i, unsigned j, double x);

// Free all memory occupied by matrix m
void mtx_free (matrix m);

// Fill a matrix with random values: [0; 1] * scale + shift
void mtx_fill (matrix m, double scale, double shift);

// Print out matrix m with precision p
void mtx_print (matrix m, int p);

// Sum two matrices in-place: a(i, j) += b(i, j)
void mtx_blit (matrix a, matrix b);

// Sum two matrices into c
void mtx_sum (matrix a, matrix b, matrix *c);

// Subtract two matrices into c
void mtx_sub (matrix a, matrix b, matrix *c);

// Multiply two matrices into c
void mtx_mult (matrix a, matrix b, matrix *c);

#endif