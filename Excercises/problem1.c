#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_MOVIES 1682
#define MAX_USERS 943

typedef struct{
    double sum[MAX_MOVIES + 1];
    int count[MAX_USERS + 1];
} SharedDate;

void compute_averages(const char * fileName, SharedDate * date){
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
            date->count[movieID]++;
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