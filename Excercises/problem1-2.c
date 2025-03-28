#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_MOVIES 1682
#define MAX_RATINGS 100000
#define LINE_LENGTH 256

typedef struct {
    int movie_count[MAX_MOVIES];
    double movie_sum[MAX_MOVIES];
} SharedData;

void calculateAverageRatings(const char *filename, SharedData *shared_data) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int userID, movieID;
    double rating;
    char timestamp[20]; // to read the timestamp, though not used

    while (fgets(timestamp, sizeof(timestamp), file) != NULL) {
        sscanf(timestamp, "%d\t%d\t%lf", &userID, &movieID, &rating);
        shared_data->movie_count[movieID]++;
        shared_data->movie_sum[movieID] += rating;
    }

    fclose(file);
}

void printAverageRatings(SharedData *shared_data) {
    for (int i = 1; i < MAX_MOVIES; i++) { // Assuming movie IDs start from 1
        if (shared_data->movie_count[i] > 0) {
            double average = shared_data->movie_sum[i] / shared_data->movie_count[i];
            printf("Movie ID: %d, Average Rating: %.2f\n", i, average);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        return EXIT_FAILURE;
    }

    // Attach shared memory
    SharedData *shared_data = (SharedData *)shmat(shmid, NULL, 0);
    if (shared_data == (SharedData *)(-1)) {
        perror("shmat");
        return EXIT_FAILURE;
    }

    memset(shared_data, 0, sizeof(SharedData)); // Initialize shared data

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid1 == 0) {
        // First child process
        calculateAverageRatings(argv[1], shared_data);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid2 == 0) {
        // Second child process
        calculateAverageRatings(argv[2], shared_data);
        exit(EXIT_SUCCESS);
    }

    // Parent process
    wait(NULL); // Wait for both children to finish
    wait(NULL);

    // Print average ratings
    printAverageRatings(shared_data);

    // Detach and delete shared memory
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
