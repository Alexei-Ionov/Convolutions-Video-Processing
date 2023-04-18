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

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  uint32_t size_of_b = b_matrix->cols; //num rows == 1 
  uint32_t size_of_a = a_matrix->cols;
  int end_ptr = size_of_b - 1;
  int start_ptr = 0;
  int32_t *a_ptr = a_matrix->data;
  int32_t *b_ptr = b_matrix->data;
  while (start_ptr < end_ptr) { 
    int32_t temp = b_ptr[end_ptr];
    b_ptr[end_ptr] = b_ptr[start_ptr];
    b_ptr[start_ptr] = temp;
    end_ptr -= 1;
    start_ptr += 1;
  }
  uint32_t size_diff = size_of_a - size_of_b;
  int32_t *res = malloc(sizeof(int32_t) * (size_diff + 1));
  int i = 0;
  for (; i <= size_diff; i++) { 
    res[i] = dot(size_of_b, a_ptr, b_ptr);
    a_ptr += 1;
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
