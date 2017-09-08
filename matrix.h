#ifndef NET_CODERODDE_LINEAR_ALGEBRA_MATRIX_H
#define NET_CODERODDE_LINEAR_ALGEBRA_MATRIX_H

#include <stdlib.h>

typedef struct matrix_t {
	size_t  m_rows;
	size_t  m_cols;
	double* m_data;
} matrix_t;

void      matrix_t_init(matrix_t* matrix, size_t rows, size_t cols);
void      matrix_t_free(matrix_t* matrix);
double    matrix_t_get(matrix_t* matrix, size_t x, size_t y);
void      matrix_t_set(matrix_t* matrix, size_t x, size_t y, double value);
matrix_t* matrix_t_copy(matrix_t* matrix);

#endif /* NET_CODERODDE_LINEAR_ALGEBRA_MATRIX_H */#pragma once
