#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

// test variation
int num_samples;
int num_threads;
double lower_bound;
double upper_bound;
double test_result;
long double result = 0.0;

pthread_mutex_t lock;
pthread_mutex_t lock2;

// thread function for Monte Carlo integration
void *monte_carlo_integration(void *arg)
{
    //pthread_mutex_lock(&lock2);
    //pthread_mutex_unlock(&lock2);
    int id = *(int *)arg;
    int samples_per_thread = num_samples / num_threads;
    int leftover_samples = num_samples % num_threads;
    int start = id * samples_per_thread;
    int end = start + samples_per_thread;
    if (id == num_threads - 1)
    {
        end += leftover_samples;
    }

    double sum = 0.0;
    for (int i = start; i < end; i++)
    {
        unsigned int seed = time(0)+clock();
        srand(seed);
        double x = (double)rand_r(&seed) / RAND_MAX * (upper_bound - lower_bound) + lower_bound;
        double y = pow(sin(x) * cos(2 * x), 2) - sqrt(6 * x);
        sum += y;
    }
    pthread_mutex_lock(&lock);
    result += sum;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}
double calculate(){
    pthread_mutex_init(&lock, NULL);
    //pthread_mutex_init(&lock2, NULL);
    //pthread_mutex_lock(&lock2);
    
        // create threads
        pthread_t threads[num_threads];
        int thread_ids[num_threads];
        for (int i = 0; i < num_threads; i++)
        {
            thread_ids[i] = i;
            pthread_create(&threads[i], NULL, monte_carlo_integration, &thread_ids[i]);
        }
        //pthread_mutex_unlock(&lock2);
    static struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);
        // join threads
        for (int i = 0; i < num_threads; i++)
        {
            pthread_join(threads[i], NULL);
        }
        gettimeofday(&end, NULL);
    double time_end = (end.tv_sec - start.tv_sec)+1.0e-6 * (end.tv_usec - start.tv_usec);
    printf("TIME: %lf\n", time_end);
        // calculate final result
        result *= (upper_bound - lower_bound) / num_samples;

        //printf("TEST #%d Result difference: %LF | %LF \n", test_id+1, test_result-result, result);

        // destroy mutex
        pthread_mutex_destroy(&lock);
        return test_result-result;
}

void test(int tests)
{
    char *filename = "test_file.txt";
    FILE *config_file = fopen(filename, "r");
    for (int test_id = 0; test_id < tests; test_id++)
    {
        double median = 0.0;
        fscanf(config_file, "%d %d %lf %lf %lf", &num_samples, &num_threads, &lower_bound, &upper_bound, &test_result);
        printf("TEST #%d with parameters: %d %d %lf %lf %lf\n", test_id + 1, num_samples, num_threads, lower_bound, upper_bound, test_result);
        // initialize mutex
        for (int it=0; it<1; it++){
            median+= calculate();
        }
        printf("TEST #%d Result median difference: %lf\n", test_id+1, median/10.0);
    }
    fclose(config_file);
}

int main(int argc, char *argv[])
{   
    if (argc == 2){
        test((int)atoi(argv[1]));
    }
    else {
        test(1);
    }
    return 0;
}
