#include <omp.h>
#include <x86intrin.h>
#include <immintrin.h>
#include "compute.h"
#define THRESHOLD 90
#define OFFSET 8
#define REQ_DIFF 8
// Computes the dot product of vec1 and vec2, both of size n


void print_matrix(int32_t* matrix, uint32_t rows, uint32_t cols) {
    for (uint32_t i = 0; i < rows; i++) {
        for (uint32_t j = 0; j < cols; j++) {
            printf("%d ", *(matrix + i * cols + j));
        }
        printf("\n");
    }
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
void flip(uint32_t size, int32_t *row_ptr) { 
  uint32_t half = size / 2;
  for (uint32_t index = 0; index < half; index++) { 
    int32_t temp = row_ptr[index];
    row_ptr[index] = row_ptr[size - index - 1];
    row_ptr[size - index - 1] = temp;
  }
}


int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  __m256i res = _mm256_set_epi32 (0, 0, 0, 0, 0, 0, 0, 0);
  uint32_t cut_off = n - (n % 16);
  int idx = 0;
  __m256i vector1, vector2;
  uint32_t i = 0;
  for (; i < cut_off; i += 16) { 
    vector1 = _mm256_loadu_si256 ((__m256i const *) (vec1 + i));
    vector2 = _mm256_loadu_si256 ((__m256i const *) (vec2 + i));
    vector1 = _mm256_mullo_epi32 (vector1, vector2);
    res = _mm256_add_epi32(res, vector1);

    vector1 = _mm256_loadu_si256 ((__m256i const *) (vec1 + i + 8));
    vector2 = _mm256_loadu_si256 ((__m256i const *) (vec2 + i + 8));
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

void test_flip(int32_t *a, int32_t *b, uint32_t size) { 
  uint32_t index = 0;
  for (; index < size; index++) { 
    if (a[index] != b[size - index - 1]) { 
      printf("%s", "Doesn't match at index:");
      printf("%d", index);
      printf("%s", "\n");
    }
  }

}

void flip_horizantal_SIMD(int row, int num_cols, int32_t *row_ptr) { 
  int start = row * num_cols;
  int end = (row * num_cols) + num_cols - 8; //-8 for size of SIMD loads
  __m256i start_vec, end_vec, order_vector;
  order_vector = _mm256_set_epi32 (0, 1, 2, 3, 4, 5, 6, 7);
  while (end - start >= REQ_DIFF) { 
    printf("%d", start);
    printf("%s", "    ");
    printf("%d", end);
    printf("%s", "\n");
    start_vec = _mm256_loadu_si256 ((__m256i const *) (row_ptr + start));
    end_vec = _mm256_loadu_si256 ((__m256i const *) (row_ptr + end));

    start_vec = _mm256_permutevar8x32_epi32(start_vec, order_vector);
    end_vec = _mm256_permutevar8x32_epi32(end_vec, order_vector);

    _mm256_storeu_si256 ((__m256i*) (row_ptr + start), end_vec);
    _mm256_storeu_si256 ((__m256i*) (row_ptr + end), start_vec);
    end -= 8;
    start += 8;
  }
  printf("%d", start);
  printf("%d", end);
  printf("%s", "\n");
  int32_t temp;
  while (start < end) { 
    temp = row_ptr[end];
    row_ptr[end] = row_ptr[start];
    row_ptr[start] = temp;
    start += 1;
    end -= 1;
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
 
 
  int end_row = num_rows_b - 1;
 
  int row = 0;
  printf("%s", "before: \n");
  print_matrix(b_ptr, num_rows_b, num_cols_b);
  uint32_t size = (num_cols_b * num_rows_b);
  int32_t *temp = malloc(sizeof(int32_t) * size);
 
  for (int m = 0; m < size; m++) { 
    temp[m] = b_ptr[m];
  }

  for (; row < num_rows_b; row++) { 
    //flip_horizontal_naive(row, num_cols_b, b_ptr);
    flip_horizantal_SIMD(row, num_cols_b, b_ptr);
  }
  printf("%s", "after: \n");
  print_matrix(b_ptr, num_rows_b, num_cols_b);

  test_flip(temp, b_ptr, size);
  

  if (num_cols_b > THRESHOLD) { 
    #pragma omp parallel 
    {
      #pragma omp for
      for (int col = 0; col < num_cols_b; col++) { 
        int end = ((num_rows_b - 1) * num_cols_b) + col;
        int start = col;
        while (start < end) { 
          int32_t temp = b_ptr[end];
          b_ptr[end] = b_ptr[start];
          b_ptr[start] = temp;
          start += num_cols_b;
          end -= num_cols_b;
        }
      }
    }
  } else { 
    int col = 0;
    for (; col < num_cols_b; col++) { 
      flip_vertial(end_row, num_cols_b, col, b_ptr);
    }

  }
  
  uint32_t row_diff = num_rows_a - num_rows_b;
  uint32_t col_diff = num_cols_a - num_cols_b;
  uint32_t size_of_res = (col_diff + 1) * (row_diff + 1);
  int32_t *res;

  res = malloc(sizeof(int32_t) * size_of_res);

  uint32_t row_a = 0;
  int index = 0;
  int32_t local;
  int b_ptr_index;
  for (;row_a + num_rows_b <= num_rows_a; row_a++) { 
    int col = 0;
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
