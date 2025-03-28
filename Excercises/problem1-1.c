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

#define MAX_USERS 1000
#define MAX_MOVIES 2000

typedef struct {
    double total_ratings;
    int rating_count;
} MovieRating;

typedef struct {
    MovieRating movies[MAX_MOVIES];
    int initialized;
} SharedData;

void process_file(const char *filename, SharedData *shared) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    int userID, movieID, rating;
    long timeStamp;

    while (fscanf(file, "%d\t%d\t%d\t%ld", &userID, &movieID, &rating, &timeStamp) == 4) {
        // Ensure movieID is within bounds
        if (movieID < 0 || movieID >= MAX_MOVIES) {
            fprintf(stderr, "Invalid movieID: %d\n", movieID);
            continue;
        }

        shared->movies[movieID].total_ratings += rating;
        shared->movies[movieID].rating_count++;
    }

    fclose(file);
}

void print_averages(SharedData *shared) {
    printf("MovieID\tAverage Rating\n");
    printf("-------\t-------------\n");
    
    for (int i = 1; i < MAX_MOVIES; i++) {
        if (shared->movies[i].rating_count > 0) {
            double avg = shared->movies[i].total_ratings / shared->movies[i].rating_count;
            printf("%d\t%.2f\n", i, avg);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create shared memory segment
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory
    SharedData *shared = (SharedData *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    memset(shared, 0, sizeof(SharedData));
    shared->initialized = 1;

    // Create two child processes
    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            process_file(argv[i + 1], shared);
            shmdt(shared); // Detach shared memory
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for children to complete
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // Print the results
    print_averages(shared);

    // Clean up
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}



/*
typedef struct{
    double sum[MAX_MOVIES + 1];
    int count[MAX_USERS + 1];
} SharedDate;

void compute_averages(const char * fileName, SharedDate * data){
    FILE * file = fopen(fileName, "r");
    if(file == NULL){
        perror("Error opening file.");
        exit(EXIT_FAILURE);
    }

    int userID, movieID, rating;
    unsigned long timestamp;
    
    while( fscanf(file, "%d\t%d\t%d\t%lu", &userID, &movieID, &rating, &timestamp) == 4 ){
        if(movieID > 0 && movieID <= MAX_MOVIES) {
            data->sum[movieID] += rating;
            data->count[movieID]++;
        }
    }

    fclose(file);
}

int main(){
    const char * movieData1 = "Data/movie-100k_1.txt";
    const char * movieData2 = "Data/movie-100k_2.txt";

    int shmid = shmget(IPC_PRIVATE, sizeof(SharedDate), IPC_CREAT | 0666);
    if(shmid == -1) {
        perror("shmget failed.");
        exit(EXIT_FAILURE);
    }

    SharedDate * shared_date = (SharedDate *)shmat(shmid, NULL, 0);
    if(shared_date == (void *)-1){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
}
*/