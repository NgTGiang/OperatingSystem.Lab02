#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int start;
    int end;
    long long sum;
} ThreadData;

void *calculate_sum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->sum = 0;
    for (int i = data->start; i <= data->end; i++) {
        data->sum += i;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <numThreads> <n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numThreads = atoi(argv[1]);
    int n = atoi(argv[2]);
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    long long totalSum = 0;

    int range = n / numThreads;
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = i * range + 1;
        threadData[i].end = (i + 1) * range;
        if (i == numThreads - 1) {
            threadData[i].end = n;
        }
        pthread_create(&threads[i], NULL, calculate_sum, &threadData[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        totalSum += threadData[i].sum;
    }

    printf("Sum of 1 to %d is %lld\n", n, totalSum);

    return EXIT_SUCCESS;
}