#ifndef MTX_H
#define MTX_H

typedef struct _mtx *matrix;

// Create a new matrix sized (rows, cols)
matrix mtx_new (unsigned rows, unsigned cols);

// Extract a submatrix sized (rows, cols) at offset (roff, coff)
matrix mtx_cpy (matrix m, unsigned rows, unsigned cols, unsigned roff, unsigned coff);

// Extract a submatrix based on quarter size h and its indexes x and y
matrix mtx_quarter (matrix m, unsigned h, unsigned x, unsigned y);

// Get a matrix element m(i, j)
double mtx_get (matrix m, unsigned i, unsigned j);

// Set a matrix element m(i, j) to x
double mtx_set (matrix m, unsigned i, unsigned j, double x);

// Free all memory occupied by matrix m
void mtx_free (matrix m);

// Set all elements of matrix m to 0
void mtx_null (matrix m);

// Fill a matrix with random values: [0; 1] * scale + shift
void mtx_fill (matrix m, double scale, double shift);

// Load a matrix from a file
matrix mtx_load (FILE *fp, unsigned rows, unsigned cols);

// Print out matrix m with precision p
void mtx_print (matrix m, int p);

// Sum two matrices in-place: d(i, j) += k * s(i, j)
void mtx_blit (matrix s, matrix d, double k);

// Sum two matrices: c(i, j) = k * a(i, j) + l * b(i, j)
void mtx_sum (matrix a, matrix b, matrix c, double k, double l);

// Multiply two matrices
void mtx_mult (matrix a, matrix b, matrix c);

// Multiply two matrices
void mtx_strassen (matrix a, matrix b, matrix c);

#endif