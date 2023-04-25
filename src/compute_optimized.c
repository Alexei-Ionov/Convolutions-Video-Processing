#include <omp.h>
#include <x86intrin.h>
#include <immintrin.h>
#include "compute.h"
#define THRESHOLD 40
#define OFFSET 8

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  __m256i res = _mm256_set_epi32 (0, 0, 0, 0, 0, 0, 0, 0);
  uint32_t cut_off = n - (n % 8);
  int idx = 0;
  __m256i vector1, vector2;
  uint32_t i = 0;
  for (; i < cut_off; i += 8) { 
    vector1 = _mm256_loadu_si256 ((__m256i const *) (vec1 + i));
    vector2 = _mm256_loadu_si256 ((__m256i const *) (vec2 + i));
    vector1 = _mm256_mullo_epi32 (vector1, vector2);
    res = _mm256_add_epi32(res, vector1);
  }

  uint32_t j = cut_off;
  int32_t final = 0;
  for (; j < n; j++) { 
    final += (vec1[j] * vec2[j]);
  }

  int32_t temp[8];
  _mm256_storeu_si256((__m256i *) temp, res);
return (int) (final + temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5] + temp[6] + temp[7]);
}


void flip_horizontal_naive(int row, int num_col, int32_t *b) { 
  int end_ptr = (row * num_col) + num_col - 1;
  int start_ptr = (row * num_col);
  int32_t temp;
  while (start_ptr < end_ptr) { 
    temp = b[end_ptr];
    b[end_ptr] = b[start_ptr];
    b[start_ptr] = temp;
    end_ptr -= 1;
    start_ptr += 1;
  }
}


void flip_horizontal_threaded(int n, int32_t *b_ptr) { 
  // n == half size of the arrray
  #pragma omp parallel 
  {
    int thread_num = omp_get_thread_num(); 
    int num_threads = omp_get_num_threads();
    int work = (n / num_threads);
    int start = thread_num * work;
    int finish = start + work;
    if (finish > n) { 
      finish = n;
    }
    for (; start < finish; start++) { 
      int diff = n - start;
      int temp = b_ptr[n + diff];
      b_ptr[n + diff] = b_ptr[start];
      b_ptr[start] = temp;
    }
  
  }
}

void flip_vertial(int row, int num_col, int col, int32_t *b) {
  int start_ptr = col;
  int end_ptr = (row * num_col) + col;
  int32_t temp;
  while (start_ptr < end_ptr) { 
    temp = b[end_ptr];
    b[end_ptr] = b[start_ptr];
    b[start_ptr] = temp;
    end_ptr -= num_col;
    start_ptr += num_col;
  }
}


// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix

  matrix_t *output = malloc(sizeof(matrix_t));
  uint32_t num_cols_a = a_matrix-> cols;
  uint32_t num_rows_a = a_matrix-> rows;
  uint32_t num_cols_b = b_matrix-> cols;
  uint32_t num_rows_b = b_matrix-> rows;

  int32_t *a_ptr = a_matrix->data;
  int32_t *b_ptr = b_matrix->data;
 
 
  // print_matrix(b_ptr, num_rows_b, num_cols_b);

  int row = 0;
  uint32_t half = num_cols_b >> 1;

  for (;row < num_rows_b; row++) { 
    if (num_cols_b < THRESHOLD) { 
      flip_horizontal_naive(row, num_cols_b, b_ptr); //overhead of starting threaded isn't worth so just do naive implementatino
    } else { 
      flip_horizontal_threaded(half, &(b_ptr[row * num_cols_b])); 
    }
  }
  int col = 0;
  int end_row = num_rows_b - 1;

  for (; col < num_cols_b; col++) { 
    flip_vertial(end_row, num_cols_b, col, b_ptr);
  }
  
  uint32_t row_diff = num_rows_a - num_rows_b;
  uint32_t col_diff = num_cols_a - num_cols_b;
  uint32_t size_of_res = (col_diff + 1) * (row_diff + 1);
  int32_t *res;

  res = malloc(sizeof(int32_t) * size_of_res);

  uint32_t row_a = 0;
  col = 0;
  int index = 0;
  int32_t local;
  int b_ptr_index;

  // for (;row_a + num_rows_b <= num_rows_a; row_a++) { 
  //   col = 0;
  //   for (; col <= col_diff; col++) { 
      
  //     local = 0;
  //     int row_a2 = row_a;
  //     b_ptr_index = 0;
  //     #pragma omp parallel 
  //     { 
  //       #pragma omp for reduction(+:local,row_a2,b_ptr_index)  
  //         local += dot(num_cols_b, &(a_ptr[(row_a2 * num_cols_a) + col]), &(b_ptr[b_ptr_index]));
  //         row_a2 += 1;
  //         b_ptr_index += num_cols_b;
  
  //     }
      
  //   }
  //   res[index] = local;
  //   index += 1;
  // }
  for (;row_a + num_rows_b <= num_rows_a; row_a++) { 
    col = 0;
    for (; col <= col_diff; col++) { 
      row = 0;
      local = 0;
      int row_a2 = row_a;
      b_ptr_index = 0;
      for (; row < num_rows_b; row++) {
        local += dot(num_cols_b, &(a_ptr[(row_a2 * num_cols_a) + col]), &(b_ptr[b_ptr_index]));
        row_a2 += 1;
        b_ptr_index += num_cols_b;
      }
      res[index] = local;
      index += 1;
    }
  }
  output->data = res;
  output->cols = col_diff + 1;
  output->rows = row_diff + 1;
  (*output_matrix) = output;

  return 0;
}

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  if (read_matrix(get_a_matrix_path(task), &a_matrix))
    return -1;
  if (read_matrix(get_b_matrix_path(task), &b_matrix))
    return -1;

  if (convolve(a_matrix, b_matrix, &output_matrix))
    return -1;

  if (write_matrix(get_output_matrix_path(task), output_matrix))
    return -1;

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}
