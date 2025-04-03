#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int calc_hcf(long int a, long int b) {
    while (b != 0) {
        long int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

long int euler_len(long int n) {
    long int count = 0;
    for (long int i = 1; i <= n; i++) {
        if (calc_hcf(n, i) == 1) {
            count++;
        }
    }
    return count;
}

int main(int argc, char** argv) {
    int p, id;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Comm_size(MPI_COMM_WORLD, &id);

    if (argc < 2) {
        if (id == 0) {
            printf("Error no params passed to euler_range\n");
        }
        MPI_Finalize();
        return 1;
    }

    int euler_range = atoi(argv[1]);
    double start_time = MPI_Wtime();
    long int total_totient = 0;

    // If main process
    if (p == 0) {
        int next_task = 1;
        int num_workers = id - 1;
        MPI_Status status;

        // Assigning tasks for each other available process
        for (int i = 1; i <= num_workers; i++) {
            if (next_task <= euler_range) {
                MPI_Send(&next_task, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                next_task++;
            } else {  // Terminating if the next_task is close to euler_range
                int term = 0;
                MPI_Send(&term, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }

        // Dynamic distribution for longer euler_ranges
        while (next_task <= euler_range) {
            long int result;
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            // Incrementing the totient based on the result
            total_totient += result;

            int task = next_task;
            MPI_Send(&task, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            next_task++;
        }

        // Terminating the other processes
        for (int i = 1; i <= num_workers; i++) {
            long int result;
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            // Incrementing the totient for the final results
            total_totient += result;

            int term = 0;
            MPI_Send(&term, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
        }

        double end_time = MPI_Wtime();
        printf("%ld -> %.2fs\n", total_totient, end_time - start_time);
    }
    
    else {
        while (1) {
            int task;
            MPI_Status status;

            // Retrieving euler_range for each task
            MPI_Recv(&task, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Exit condition
            if (status.MPI_TAG == 1) {
                break;
            }

            // Actual calculation process
            long int totient = euler_len(task);

            // Sharing the result
            MPI_Send(&totient, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
