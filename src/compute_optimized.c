#include <omp.h>
#include <x86intrin.h>
#include <immintrin.h>
#include "compute.h"
#define THRESHOLD 64
#define REQ_DIFF 8
#define NUM_THREADS 8
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


int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  __m256i res = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
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
  int cut_off = num_cols / 16;
  order_vector = _mm256_set_epi32 (0, 1, 2, 3, 4, 5, 6, 7);
  while (end - start >= REQ_DIFF) { 
    start_vec = _mm256_loadu_si256 ((__m256i const *) (row_ptr + start));
    end_vec = _mm256_loadu_si256 ((__m256i const *) (row_ptr + end));

    start_vec = _mm256_permutevar8x32_epi32(start_vec, order_vector);
    end_vec = _mm256_permutevar8x32_epi32(end_vec, order_vector);

    _mm256_storeu_si256 ((__m256i*) (row_ptr + start), end_vec);
    _mm256_storeu_si256 ((__m256i*) (row_ptr + end), start_vec);
    start += 8;
    if (end - start < 16) { 
      end -= 1;
      break;
    }
    end -= 8;
  }
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


int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  // output_matrix  
  uint32_t num_cols_a = a_matrix-> cols;
  uint32_t num_rows_a = a_matrix-> rows;
  uint32_t num_cols_b = b_matrix-> cols;
  uint32_t num_rows_b = b_matrix-> rows;

  int32_t *a_ptr = a_matrix->data;
  int32_t *b_ptr = b_matrix->data;
  matrix_t *output = malloc(sizeof(matrix_t));
  int32_t *res;
  if (num_cols_a <= 0 || num_cols_b <= 0 || num_rows_a <= 0 || num_rows_b <= 0) {
    printf("%d", num_rows_a);
    printf("%d", num_cols_a);
    printf("%d", num_rows_b);
    printf("%d", num_cols_b);
    printf("%s", "\n");
    
    res = malloc(sizeof(int32_t) * 0);
    output->data = res;
    output->cols = 0;
    output->rows = 0;
    (*output_matrix) = output;
    return 0; 
  }
 
 
  uint32_t end_row = num_rows_b - 1;
  uint32_t row = 0;
 
  for (; row < num_rows_b; row++) { 
    if (num_cols_b >= THRESHOLD) { 
      int start = row * num_cols_b;
      int end = (row * num_cols_b) + num_cols_b - 8; //-8 for size of SIMD loads
      __m256i start_vec, end_vec, order_vector;
      order_vector = _mm256_set_epi32 (0, 1, 2, 3, 4, 5, 6, 7);
      while (end - start >= REQ_DIFF) { 
        start_vec = _mm256_loadu_si256 ((__m256i const *) (b_ptr + start));
        end_vec = _mm256_loadu_si256 ((__m256i const *) (b_ptr + end));
        start_vec = _mm256_permutevar8x32_epi32(start_vec, order_vector);
        end_vec = _mm256_permutevar8x32_epi32(end_vec, order_vector);
        _mm256_storeu_si256 ((__m256i*) (b_ptr + start), end_vec);
        _mm256_storeu_si256 ((__m256i*) (b_ptr + end), start_vec);
        start += 8;
        if (end - start < 16) { 
          end -= 1;
          break;
        }
        end -= 8;
      }
      int32_t temp;
      while (start < end) { 
        temp = b_ptr[end];
        b_ptr[end] = b_ptr[start];
        b_ptr[start] = temp;
        start += 1;
        end -= 1;
      }
    } else { 
      flip_horizontal_naive(row, num_cols_b, b_ptr);
    }
    
  }
  
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
  res = malloc(sizeof(int32_t) * size_of_res);
  uint32_t num_threads = 8;

  if (row_diff >= 7) { 
    #pragma omp parallel 
    {
      int thread_num = omp_get_thread_num();
      int num_threads = omp_get_num_threads();
      uint32_t work = (row_diff + 1) / num_threads;           //might not divide perfectly so need to do manual work afterword
      uint32_t start = work * thread_num;
      uint32_t finish = start + work;
      
      if (finish > (row_diff + 1)) {
        finish = row_diff + 1;
      }
      for (; start < finish; start++) {
        uint32_t col = 0;
        for (; col <= col_diff; col++) { 
          uint32_t b_ptr_index = 0; 
          int32_t local = 0;
          uint32_t row = 0; 
          uint32_t a_ptr_index = start * num_cols_a;
          for (; row < num_rows_b; row++) {
            local += dot(num_cols_b, &(a_ptr[a_ptr_index + col]), &(b_ptr[b_ptr_index]));
            b_ptr_index += num_cols_b;
            a_ptr_index += num_cols_a;
            
          }
          res[(start * (col_diff + 1)) + col] = local;
        }   
      }
    }
    uint32_t leftover = ((row_diff + 1) / 8) * 8;
    for (; leftover < row_diff + 1; leftover++) {
      uint32_t col = 0;
      for (; col <= col_diff; col++) { 
        uint32_t b_ptr_index = 0; 
        int32_t local = 0;
        uint32_t row = 0; 
        uint32_t a_ptr_index = leftover * num_cols_a;
        for (; row < num_rows_b; row++) {
          local += dot(num_cols_b, &(a_ptr[a_ptr_index + col]), &(b_ptr[b_ptr_index]));
          b_ptr_index += num_cols_b;
          a_ptr_index += num_cols_a;
        }
        res[(leftover * (col_diff + 1)) + col] = local;
      }   
    }
  } else { 
    #pragma omp parallel num_threads(row_diff + 1)
    {
      int thread_num = omp_get_thread_num();
      int num_threads = omp_get_num_threads();
      uint32_t work = (row_diff + 1) / num_threads;           //might not divide perfectly so need to do manual work afterword
      uint32_t start = work * thread_num;
      uint32_t finish = start + work;
      
      if (finish > (row_diff + 1)) {
        finish = row_diff + 1;
      }
      for (; start < finish; start++) {
        uint32_t col = 0;
        for (; col <= col_diff; col++) { 
          uint32_t b_ptr_index = 0; 
          int32_t local = 0;
          uint32_t row = 0; 
          uint32_t a_ptr_index = start * num_cols_a;
          for (; row < num_rows_b; row++) {
            local += dot(num_cols_b, &(a_ptr[a_ptr_index + col]), &(b_ptr[b_ptr_index]));
            b_ptr_index += num_cols_b;
            a_ptr_index += num_cols_a;
          }
          res[(start * (col_diff + 1)) + col] = local;
        }   
      }
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
