#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  int res, i = 0;
  for (; i < n; i++) { 
    res += (vec1[i] * vec2[i]);
  }
  return res;
}

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix a and matrix b, and store the resulting matrix in
  int size_of_b = b_matrix->cols; //num rows == 1 
  int end_ptr = size_of_b - 1;
  int start_ptr = 0;
  while (start_ptr != end_ptr) { 
    int temp = b_matrix->data[end_ptr];
    b_matrix->data[end_ptr] = b_matrix->data[start_ptr];
    b_matrix->data[start_ptr] = temp;
    end_ptr -= 1;
    start_ptr += 1;
  }

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
