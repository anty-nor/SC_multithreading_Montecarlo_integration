#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <math.h>

double func(double x)
{
    return (double)(pow(sin(x) * cos(2 * x), 2) - sqrt(6 * x));
}

int main(int argc, char **argv)
{
    char* filename;
    int num_threads = 0; 
   
    if (argc < 2){
            filename = "config.txt";
            num_threads = 4;
        }
    else {
            if (argc == 2){
                filename = "config.txt";
                num_threads = atoi(argv[1]);
            }
            else if (argc == 3){
                filename = argv[2];
                num_threads = atoi(argv[1]);
            }
            else {
                filename = "config.txt";
                num_threads = 4;
            }
    } 
    double lower_bound, upper_bound;
    long long int num_samples, num_local_samples, i;
    double global_sum = 0.0;
    // double rand_x, rand_y;
    double test_res;
    // Set the number of threads to use
    //int num_threads = 2;
    //omp_set_num_threads(num_threads);

    srand(time(NULL)); // Seed the random number generator

    // Read in the configuration from file
    FILE *config_file = fopen(filename, "r");
    fscanf(config_file, "%lld", &num_samples);
    fscanf(config_file, "%lf", &lower_bound);
    fscanf(config_file, "%lf", &upper_bound);
    fscanf(config_file, "%lf", &test_res);
    fclose(config_file);

    // num_local_samples = num_samples/ num_threads;
     // Seed the random number generator for each thread

    omp_set_num_threads(num_threads);
    //clock_t time_start= clock();
    static struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);
    #pragma omp parallel 
    {
        int thread_id = omp_get_thread_num();
        srand(time(0) + clock()+thread_id);
    #pragma omp for reduction(+:global_sum)
       for (i = 0; i < num_samples; i++)
        {
            double rand_x = (double)rand() / RAND_MAX * (upper_bound - lower_bound) + lower_bound;
            double rand_y = func(rand_x);
            // sleep(rand()%10);
            global_sum += rand_y;
        }
    }
    
    //clock_t time_end = clock() - time_start;
    
    double result = global_sum * (upper_bound - lower_bound) / num_samples;
    gettimeofday(&end, NULL);
    double time_end = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
    printf("Time: %f\n ", ((double)time_end / CLOCKS_PER_SEC));
    printf("Result, tesst_result = %lf  %lf\n", result, test_res);
    return 0;
}

