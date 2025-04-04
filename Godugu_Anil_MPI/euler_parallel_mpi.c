#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function Preprocessors
int calc_hcf(long int a, long int b);
long int euler_len(long int n);

int main(int argc, char** argv) {
    int p, id;

    // Initialising MPI
    MPI_Init(&argc, &argv);

    // Setting up MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &p);
    MPI_Comm_size(MPI_COMM_WORLD, &id);

    if (argc < 2) {
        if (id == 0) {
            printf("Error no params passed to euler_range.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Acquiring the Euler Range from CLI
    int euler_range = atoi(argv[1]);
    long int total_totient = 0;

    // Start time
    double start_time = MPI_Wtime();

    // Checks for Master Process
    if (p == 0) {
        int terminate = 0;
        int next_range = 1;
        int num_workers = id - 1;
        MPI_Status status;

        // Initial allocation of work to all the sub processes
        for (int i = 1; i <= num_workers; i++) {
            if (next_range <= euler_range) {
                MPI_Send(&next_range, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                next_range++;
            } else {
                MPI_Send(&terminate, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }

        // Dynamic allocation of tasks before end point
        while (next_range <= euler_range) {
            long int result;
            int task = next_range;

            // Retrieving results from current processes
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            // Incrementing the Totient
            total_totient += result;

            // Sending new task
            MPI_Send(&task, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            next_range++;
        }

        // Terminating tasks on end point
        for (int i = 1; i <= num_workers; i++) {
            long int result;

            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total_totient += result;
            MPI_Send(&terminate, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
        }

        // End time
        double end_time = MPI_Wtime();

        // Display Result
        printf("%ld -> %.2fs\n", total_totient, end_time - start_time);
    }
    
    // If not master process
    else {
        while (1) {
            int task;
            long int totient;
            MPI_Status status;
            
            // Recieve task for execution
            MPI_Recv(&task, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //  Termination Condition
            if (status.MPI_TAG == 1 && task == 0) {
                break;
            }

            // Euler Totient Calculation
            totient = euler_len(task);

            // Sending the result to Master
            MPI_Send(&totient, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}

// Helper function to calculate the hcf
int calc_hcf(long int a, long int b) {
    while (b != 0) {
        long int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

// Helper function to calculate the Euler Totient
long int euler_len(long int n) {
    long int count = 0;
    for (long int i = 1; i <= n; i++) {
        if (calc_hcf(n, i) == 1) {
            count++;
        }
    }
    return count;
}