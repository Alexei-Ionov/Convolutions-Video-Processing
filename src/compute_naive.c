#include "compute.h"

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  // TODO: implement dot product of vec1 and vec2, both of size n
  int32_t res = 0;
  int i = 0;
  for (; i < n; i++) { 
    res += (vec1[i] * vec2[i]);
  }
  return res;
}
void flip_horizontal(int row, int num_col, int32_t *b) { 
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

void print_matrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", *(matrix + i * cols + j));
        }
        printf("\n");
    }
}


// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  // TODO: convolve matrix 
  matrix_t *output = malloc(sizeof(matrix_t));

  



  uint32_t num_cols_a = a_matrix-> cols;
  uint32_t num_rows_a = a_matrix-> rows;
  uint32_t num_cols_b = b_matrix-> cols;
  uint32_t num_rows_b = b_matrix-> rows;

  int32_t *a_ptr = a_matrix->data;
  int32_t *b_ptr = b_matrix->data;
 
 
  // print_matrix(b_ptr, num_rows_b, num_cols_b);

  int row = 0;
  for (;row < num_rows_b; row++) { 
    flip_horizontal(row, num_cols_b, b_ptr); 
  }
  // print_matrix(b_ptr, num_rows_b, num_cols_b);

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
  
  int i, j, m, n;
  int kcenter_row = num_rows_b / 2;
  int kcenter_col = num_cols_b / 2;
  int sum;

  for (i = 0; i < num_rows_a - num_rows_b + 1; i++) {
      for (j = 0; j < num_cols_a - num_cols_b + 1; j++) {
          sum = 0;
          for (m = 0; m < num_rows_b; m++) {
              for (n = 0; n < num_cols_b; n++) {
                  sum += a_ptr[(i + m)*num_rows_a + (j + n)] * b_ptr[(m*num_rows_b) + n];
              }
          }
          res[i*(row_diff + 1) + j] = sum;
      }
  }

  // for (;row_a + num_rows_b <= num_rows_a; row_a++) { 
  //   col = 0;
  //   for (; col <= col_diff; col++) { 
  //     row = 0;
  //     local = 0;
  //     int row_a2 = row_a;
  //     b_ptr_index = 0;
  //     for (; row < num_rows_b; row++) {
  //       local += dot(num_cols_b, &(a_ptr[(row_a2 * num_cols_a) + col]), &(b_ptr[b_ptr_index]));
  //       row_a2 += 1;
  //       b_ptr_index += num_cols_b;
  //     }
  //     res[index] = local;
  //     index += 1;
  //   }
  // }
  
  output->data = res;
  (*output_matrix) = output;
  // printf("%d", size_of_res);
  // printf("%s", "\n");
  print_matrix((*output_matrix)->data, row_diff + 1, col_diff + 1);
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
