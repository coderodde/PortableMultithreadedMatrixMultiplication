#include "matrix.h"
#include <stdlib.h>
#include <string.h>

static size_t data_index(matrix_t* matrix, size_t x, size_t y)
{
	return y * matrix->m_cols + x;
}

void matrix_t_init(matrix_t* matrix, size_t rows, size_t cols)
{
	matrix->m_rows = rows;
	matrix->m_cols = cols;
	matrix->m_data = malloc(sizeof(double) * rows * cols);
}

void matrix_t_free(matrix_t* matrix)
{
	free(matrix->m_data);
	matrix->m_rows = 0;
	matrix->m_cols = 0;
}

double matrix_t_get(matrix_t* matrix, size_t x, size_t y)
{
	return matrix->m_data[data_index(matrix, x, y)];
}

void matrix_t_set(matrix_t* matrix, size_t x, size_t y, double value)
{
	matrix->m_data[data_index(matrix, x, y)] = value;
}

matrix_t* matrix_t_copy(matrix_t* matrix)
{
	size_t data_len = sizeof(double) * matrix->m_rows * matrix->m_cols;
	matrix_t* copy = malloc(sizeof(*copy));
	copy->m_rows = matrix->m_rows;
	copy->m_cols = matrix->m_cols;
	copy->m_data = malloc(data_len);
	memcpy(copy->m_data, matrix->m_data, data_len);
	return copy;
}