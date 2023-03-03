#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


double func(double x){
    x = pow(sin(x) * cos(2*x), 2) - sqrt(6*x);
}

int main(int argc, char *argv[])
{
    int rank, size;
    double lower_bound, upper_bound;
    long int num_samples, num_local_samples, i;
    double local_sum = 0.0, global_sum = 0.0;
    double rand_x, rand_y;
    double test_res;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //srand(time(0) + rank + clock()); // Seed the random number generator

    if (rank == 0) {
        // Read in the configuration from file
        FILE *config_file = fopen("config.txt", "r");
        fscanf(config_file, "%ld", &num_samples);
        fscanf(config_file, "%lf", &lower_bound);
        fscanf(config_file, "%lf", &upper_bound);
        fscanf(config_file, "%lf", &test_res);
        fclose(config_file);
    }
    double time_taken = -1 * MPI_Wtime();
    // Broadcast the configuration data to all processes
    MPI_Bcast(&num_samples, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&lower_bound, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&upper_bound, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    num_local_samples = num_samples / size;
    srand(time(0) + rank + clock());
    for (i = 0; i < num_local_samples; i++) {
        rand_x = (double)rand() / RAND_MAX * (upper_bound - lower_bound) + lower_bound;
        rand_y =  func(rand_x);
        local_sum+=rand_y;
    }

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    time_taken += MPI_Wtime();
    if (rank == 0) {
        double result = global_sum * (upper_bound - lower_bound) / num_samples;
        printf("Result, tesst_results:  time:  %lf  value: %lf  test: %lf\n", time_taken, result, test_res);
    }

    MPI_Finalize();

    return 0;
}
