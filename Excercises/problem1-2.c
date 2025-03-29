/*
There are two files  contains the 100000 ratings of 943 users for 1682 movies in the following format:
 userID <tab> movieID <tab> rating <tab> timeStamp
 userID <tab> movieID <tab> rating <tab> timeStamp
 ...
 Requirement: 
    write a C program that spawns two child processes, 
    and each of them will read a file and compute the average ratings of movies in the file. 
    You implement the program by using shared memory method. 
*/

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
    int movieCount[MAX_MOVIES];
    double movieSum[MAX_MOVIES];
} SharedData;

void readFileAndCalculateAverageRatings(const char *filename, SharedData *sharedData) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int userID, movieID;
    double rating;
    char timestamp[9]; // to read the timestamp, though not used

    while (fgets(timestamp, sizeof(timestamp), file) != NULL) {
        sscanf(timestamp, "%d\t%d\t%lf", &userID, &movieID, &rating);
        sharedData->movieCount[movieID]++;
        sharedData->movieSum[movieID] += rating;
    }

    fclose(file);
}

void printAverageRatings(SharedData *sharedData) {
    for (int i = 1; i <= MAX_MOVIES; i++) { // Assuming movie IDs start from 1
        if (sharedData->movieCount[i] > 0) {
            double average = sharedData->movieSum[i] / sharedData->movieCount[i];
            printf("Movie ID: %d, Average Rating: %.2f\n", i, average);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Mismatch input data. Usage: %s <file1> <file2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        return EXIT_FAILURE;
    }

    // Attach shared memory
    SharedData *sharedData = (SharedData *)shmat(shmid, NULL, 0);
    if (sharedData == (SharedData *)(-1)) {
        perror("shmat");
        return EXIT_FAILURE;
    }

    memset(sharedData, 0, sizeof(SharedData)); // Initialize shared data

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid1 == 0) {
        // First child process
        readFileAndCalculateAverageRatings(argv[1], sharedData);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid2 == 0) {
        // Second child process
        readFileAndCalculateAverageRatings(argv[2], sharedData);
        exit(EXIT_SUCCESS);
    }

    // Parent process
    wait(NULL); // Wait for both children to finish
    wait(NULL);

    // Print average ratings
    printAverageRatings(sharedData);

    // Detach and delete shared memory
    shmdt(sharedData);
    shmctl(shmid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
