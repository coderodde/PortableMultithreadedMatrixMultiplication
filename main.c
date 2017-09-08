#include "matrix.h"
#include "matrix_algorithm.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

static matrix_t* create_random_matrix(const size_t rows, const size_t cols)
{
	size_t x;
	size_t y;
	matrix_t* m = matrix_t_alloc(rows, cols);

	if (!m)
	{
		return NULL;
	}

	for (x = 0; x < cols; ++x)
	{
		for (y = 0; y < rows; ++y)
		{
			matrix_t_set(m, x, y, ((double)rand()) / RAND_MAX);
		}
	}

	return m;
}

static size_t get_milliseconds()
{
#ifdef _WIN32
	return (size_t) GetTickCount64();
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec * 1000 + time.tv_usec / 1000;
#endif
}

static int matrix_equals(const matrix_t const* a, const matrix_t const* b)
{
	size_t x;
	size_t y;

	if (a->m_cols != b->m_cols || a->m_rows != b->m_rows)
	{
		return 0;
	}

	for (y = 0; y < a->m_rows; ++y)
	{
		for (x = 0; x < a->m_cols; ++x)
		{
			if (matrix_t_get(a, x, y) != matrix_t_get(b, x, y))
			{
				return 0;
			}
		}
	}

	return 1;
}

int main() {
	size_t start_time;
	size_t end_time;
	matrix_t* a;
	matrix_t* b;
	matrix_t* ab1;
	matrix_t* ab2;

	srand((unsigned int)time(NULL));

	a = create_random_matrix(500, 500);
	b = matrix_t_copy(a);

	start_time = get_milliseconds();
	ab1 = matrix_t_multiply(a, b);
	end_time = get_milliseconds();

	printf("Single-threaded multiplication in %zu milliseconds.\n",
		end_time - start_time);

	start_time = get_milliseconds();
	ab2 = matrix_t_multiply_parallel(a, b);
	end_time = get_milliseconds();

	printf("%zu-threaded multiplication in %zu milliseconds.\n",
		get_number_of_processors(),
		end_time - start_time);

	printf("Algorithms agree: %d\n", matrix_equals(ab1, ab2));

	matrix_t_free(a);
	matrix_t_free(b);
	matrix_t_free(ab1);
	matrix_t_free(ab2);

#ifdef _WIN32
	Sleep(5000);
#endif

	return 0;
}