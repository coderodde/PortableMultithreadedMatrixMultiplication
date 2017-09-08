#include "matrix.h"
#include "matrix_algorithm.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))

size_t get_number_of_processors()
{
#ifdef _WIN32
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (size_t)info.dwNumberOfProcessors;
#else
	return (size_t)sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

matrix_t* matrix_t_multiply(matrix_t* matrix1, matrix_t* matrix2)
{
	matrix_t* result;
	size_t x;
	size_t y;
	size_t i;
	double sum;

	if (!matrix1 || !matrix2)
	{
		return NULL;
	}

	if (!matrix1->m_data || !matrix2->m_data)
	{
		return NULL;
	}

	if (matrix1->m_cols != matrix2->m_rows)
	{
		return NULL;
	}

	result = malloc(sizeof(*result));
	matrix_t_init(result, matrix1->m_rows, matrix2->m_cols);

	for (y = 0; y != matrix1->m_rows; ++y)
	{
		for (x = 0; x != matrix2->m_cols; ++x)
		{
			sum = 0.0;

			for (i = 0; i != matrix1->m_cols; ++i)
			{
				sum += matrix_t_get(matrix1, i, y) *
					matrix_t_get(matrix2, x, i);
			}

			matrix_t_set(result, x, y, sum);
		}
	}

	return result;
}

static const size_t MINIMUM_THREAD_LOAD = 1; /*10 * 1000;*/

typedef struct thread_info {
	matrix_t* left_matrix;
	matrix_t* right_matrix;
	matrix_t* result_matrix;
	size_t    start_row;
	size_t    rows;
} thread_info;

#ifdef _WIN32
DWORD WINAPI ThreadProc(LPVOID arg)
#else
static void* thread_func(void* arg)
#endif
{
	size_t i;
	size_t x;
	size_t y;
	double sum;
	thread_info* info = (thread_info*) arg;

	for (y = info->start_row; y < info->start_row + info->rows; ++y)
	{
		for (x = 0; x < info->right_matrix->m_cols; ++x)
		{
			sum = 0.0;

			for (i = 0; i < info->left_matrix->m_cols; ++i)
			{
				sum += matrix_t_get(info->left_matrix, i, y) *
					matrix_t_get(info->right_matrix, x, i);
			}

			matrix_t_set(info->result_matrix, x, y, sum);
		}
	}

	return NULL;
}

matrix_t* matrix_t_multiply_parallel(matrix_t* left_matrix,
	matrix_t* right_matrix)
{
	size_t i;
	size_t rows_reserved;
	size_t work_load;
	size_t num_threads;
	size_t basic_rows_per_thread;
	thread_info* thread_info_structs;
	matrix_t* result_matrix = malloc(sizeof(*result_matrix));

	matrix_t_init(result_matrix,
		left_matrix->m_rows,
		right_matrix->m_cols);

	work_load = left_matrix->m_rows *
		right_matrix->m_cols *
		right_matrix->m_rows;

	num_threads = get_number_of_processors();
	num_threads = MIN(num_threads, work_load / MINIMUM_THREAD_LOAD);
	num_threads = MIN(num_threads, left_matrix->m_rows);
	num_threads = MAX(num_threads, 1);

	if (num_threads == 1)
	{
		return matrix_t_multiply(left_matrix, right_matrix);
	}

	basic_rows_per_thread = left_matrix->m_rows / num_threads;
	thread_info_structs = calloc(num_threads, sizeof(thread_info));

	for (i = 0, rows_reserved = 0;
		i != num_threads;
		i++, rows_reserved += basic_rows_per_thread)
	{
		thread_info_structs[i].left_matrix = left_matrix;
		thread_info_structs[i].right_matrix = right_matrix;
		thread_info_structs[i].result_matrix = result_matrix;
		thread_info_structs[i].rows = basic_rows_per_thread;
		thread_info_structs[i].start_row = rows_reserved;
	}

	thread_info_structs[num_threads - 1].rows +=
		left_matrix->m_rows % basic_rows_per_thread;

#ifdef _WIN32
	HANDLE* threads = calloc(num_threads, sizeof(HANDLE));
#else
	pthread_t* threads = calloc(num_threads, sizeof(pthread_t));
#endif

	for (i = 0; i < num_threads - 1; ++i)
	{
#ifdef _WIN32
		threads[i] = CreateThread(NULL,
								  1000000,
							      ThreadProc,
								  (void*) &thread_info_structs[i],
								  0, 
								  NULL);
#else
		pthread_create(&threads[i],
			NULL,
			thread_func,
			(void*)&thread_info_structs[i]);
#endif
	}

#ifdef _WIN32
	ThreadProc((void*) &thread_info_structs[num_threads - 1]);
#else
	thread_func((void*)&thread_info_structs[num_threads - 1]);
#endif

#ifdef _WIN32
	WaitForMultipleObjects(num_threads - 1, threads, TRUE, INFINITE);
#else 
	for (i = 0; i < num_threads - 1; ++i)
	{
		pthread_join(threads[i], NULL);
	}
#endif
	return result_matrix;
}

void matrix_t_print(matrix_t* matrix)
{
	for (size_t y = 0; y < matrix->m_rows; ++y)
	{
		for (size_t x = 0; x < matrix->m_cols; ++x)
		{
			printf("%f ", matrix_t_get(matrix, x, y));
		}

		puts("");
	}
}