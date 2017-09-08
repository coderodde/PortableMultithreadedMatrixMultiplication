#ifndef NET_CODERODDE_LINEAR_ALGEBRA_MATRIX_ALGORITHM_H
#define NET_CODERODDE_LINEAR_ALGEBRA_MATRIX_ALGORITHM_H

#include "matrix.h"

matrix_t* matrix_t_multiply(matrix_t* matrix1, matrix_t* matrix2);
matrix_t* matrix_t_multiply_parallel(matrix_t* matrix1, matrix_t* matrix2);
void      matrix_t_print(matrix_t* matrix); 

size_t get_number_of_processors();

#endif /* NET_CODERODDE_LINEAR_ALGEBRA_MATRIX_ALGORITHM_H */