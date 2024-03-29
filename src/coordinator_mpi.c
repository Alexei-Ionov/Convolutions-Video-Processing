#include <mpi.h>

#include "coordinator.h"

#define READY 0
#define NEW_TASK 1
#define TERMINATE -1

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Error: not enough arguments\n");
    printf("Usage: %s [path_to_task_list]\n", argv[0]);
    return -1;
  }
  int num_tasks;
  task_t **tasks;
  if (read_tasks(argv[1], &num_tasks, &tasks)) { 
    return -1;
  }
  MPI_Init(&argc, &argv);
  int procID, totalProcs;
  MPI_Comm_size(MPI_COMM_WORLD, &totalProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);

  if (procID == 0) {          //manager mode
    int nextTask = 0;
    MPI_Status status;
    int32_t message;

    // TODO: loop until we've completed `numTasks`
    while (num_tasks) {
        // TODO: receive a message from any source (so we know that this node is done with its task)
      MPI_Recv(&message, 1, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      // TODO: get the source process using the `status` struct
      int source = status.MPI_SOURCE;
      message = nextTask;
      MPI_Send(&message, 1, MPI_INT32_T, source, 0, MPI_COMM_WORLD);
      // TODO: send `nextTask` as the message to the process we just received a message from
      nextTask += 1;
      num_tasks -= 1;
    }
    for (int i = 0; i < totalProcs - 1; i++) {
          // TODO: receive a message from any source (so we know that this node is done with its task)
      MPI_Recv(&message, 1, MPI_INT32_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      int source = status.MPI_SOURCE;
      message = TERMINATE;
      MPI_Send(&message, 1, MPI_INT32_T, source, 0, MPI_COMM_WORLD);
    }
  } else {          //worker mode
    int32_t message;
    while (true) {
      message = READY;
      
      MPI_Send(&message, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD);

      MPI_Recv(&message, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      // TODO: receive 1 message from the manager and store it in `message`
      // Hint: use MPI_Recv to receive a message
      if (message == TERMINATE) { 
          break;
      }
      if (execute_task(tasks[message])) {
        printf("Task %d failed\n", message);
        return -1;
      }
      free(tasks[message]->path);
      }
  }
  free(tasks);
  MPI_Finalize();
  // TODO: implement Open MPI coordinator
}
