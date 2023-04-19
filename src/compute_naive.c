#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  int res = 0;
  int i = 0;
  for (; i < n; i++) { 
    res += (vec1[i] * vec2[i]);
  }
  return res;
}
int flip_horizontal(int row, int num_col, int32_t *b) { 
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
  return 0;
}
int flip_vertial(int num_rows, int num_col, int col, int32_t *b) {
  int start_ptr = col;
  int end_ptr = (num_rows * num_col) + col;
  int32_t temp;
  while (start_ptr < end_ptr) { 
    temp = b[end_ptr];
    b[end_ptr] = b[start_ptr];
    b[start_ptr] = temp;
    end_ptr -= num_col;
    start_ptr += num_col;
  }
  return 0;
}

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in

  uint32_t num_cols_b = b_matrix-> cols;
  uint32_t num_cols_a = a_matrix-> cols;
  uint32_t num_rows_a = a_matrix-> rows;
  uint32_t num_rows_b = b_matrix-> rows;
  int row = 0;
  int32_t *a_ptr = a_matrix->data;
  int32_t *b_ptr = b_matrix->data;
  for (;row < num_rows_b; row++) { 
    flip_horizontal(row, num_cols_b, b_ptr); 
  }
  int col = 0;
  for (; col < num_cols_b; col++) { 
    flip_vertial(num_rows_b, num_cols_b, col, b_ptr);
  }

  uint32_t row_diff = num_rows_a - num_rows_b;
  uint32_t col_diff = num_cols_a - num_cols_b;
  int size_of_res = (col_diff + 1) * (row_diff + 1);
  int32_t *res = malloc(sizeof(uint32_t) * (size_of_res));

  row, col = 0;
  int index, local = 0;
  int row_a, col_a = 0;
  
  for (; index < size_of_res; index++) { 
    local = 0;
    for (; col <= col_diff; col++) { 
      for (; row < num_rows_b; row++) {
        local += dot(num_cols_b, &(a_ptr[(row_a * num_cols_a) + col_a]), b_ptr);
        row_a += 1;
        b_ptr += num_cols_b;
      }
      b_ptr = b_matrix-> data;
      col_a += 1;
    }
    col = 0;
    col_a = 0;
    res[index] = local;
    row_a += 1;
  }
  (*output_matrix)->data = res;

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
