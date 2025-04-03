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
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            printf("Error: No parameter passed to euler_range\n");
        }
        MPI_Finalize();
        return 1;
    }

    int euler_range = atoi(argv[1]);
    long int total_totient = 0;

    if (rank == 0) {
        // Processus maître
        double start_time = MPI_Wtime();
        int next_task = 1;
        int num_workers = size - 1;
        MPI_Status status;

        // Envoi initial des tâches aux travailleurs
        for (int i = 1; i <= num_workers; i++) {
            if (next_task <= euler_range) {
                MPI_Send(&next_task, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                next_task++;
            } else {
                int term = 0;
                MPI_Send(&term, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }

        // Gestion dynamique des tâches
        while (next_task <= euler_range) {
            long int result;
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total_totient += result;

            int task = next_task;
            MPI_Send(&task, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            next_task++;
        }

        // Récupération des derniers résultats et envoi du signal d'arrêt
        for (int i = 1; i <= num_workers; i++) {
            long int result;
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total_totient += result;

            int term = 0;
            MPI_Send(&term, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
        }

        double end_time = MPI_Wtime();
        printf("Totient sum: %ld -> Execution time: %.2fs\n", total_totient, end_time - start_time);
    } else {
        // Processus esclaves
        while (1) {
            int task;
            MPI_Status status;
            MPI_Recv(&task, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 1) {
                break; // Arrêt du travailleur
            }

            long int totient = euler_len(task);
            MPI_Send(&totient, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
