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

void readFileAndCalculateAverageRatings(const char *filename, SharedData *shared) {
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

void printAverages(SharedData *shared) {
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
        fprintf(stderr, "Mismatch input data. Usage: %s <file1> <file2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create shared memory segment
    int shmid = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    /*

    */



    // Attach shared memory
    SharedData *shared = (SharedData *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    /*
    1. Meaning of 0 in shmat(). The 0 in this context means:
        - No special attachment flags are being used.
        - The shared memory segment will be attached with default behavior:
            - Read/write access (unless the segment was created read-only)
            - Let the OS choose the attachment address in the process's address space (since NULL is passed as the second argument)

    2. shmflg Values and Their Purposes:
        The shmflg argument in shmat() controls how the shared memory is attached. Here are the possible flags (defined in <sys/shm.h>):

        Flag	        Value (Hex)	    Purpose
        SHM_RDONLY	    0x1000	        Attach the segment for read-only access. Without this, the attachment is read-write.
        SHM_RND	        0x2000	        Round down the attach address to a page boundary (used when specifying a non-NULL address).
        SHM_REMAP	    0x4000	        (Linux-specific) Override any existing mapping at the requested address.
        SHM_EXEC	    0x8000	        (Linux-specific) Allow execution of code in the shared memory segment.

    Common Usage Examples:
        - Default attachment (read/write, OS picks address)
            shmat(shmid, NULL, 0);

        - Attach read-only
            shmat(shmid, NULL, SHM_RDONLY);

        - Attach at a specific address (with rounding)
            void *addr = (void *)0x10000; // Example address
            shmat(shmid, addr, SHM_RND); // Rounds down to nearest page boundary

        - Force remap at a specific address (Linux)
            shmat(shmid, addr, SHM_REMAP);

    3. Key Takeaways
        - shmat() attaches shared memory to a process's address space.
        - shmflg modifies how the attachment happens.
            - 0 → Default (read/write, no special behavior).
            - SHM_RDONLY → Read-only access.
            - SHM_RND → Round address to a page boundary.
            - SHM_REMAP → Override existing mappings (Linux).
        - Passing NULL lets the OS pick the address.
        - Permissions (read/write) depend on shmflg and how the segment was created (shmget).
    */




    // Initialize shared memory
    memset(shared, 0, sizeof(SharedData));
    shared->initialized = 1;
    /*
    The memset() used to fill a block of memory with a specific value:
        void *memset(void *ptr, int value, size_t num);
            ptr:    Pointer to the block of memory to fill.
            value:  The value to set (converted to an unsigned char).
            num:    The number of bytes to be set to the value.
    Ex:
        #include <stdio.h>
        #include <string.h>

        int main() {
            char str[50];
            
            // Initialize the string with some values
            strcpy(str, "Hello, World!");
            
            printf("Before memset: %s\n", str);
            
            // Use memset to clear the string
            memset(str, '*', 5); // Set the first 5 bytes to '*'
            
            printf("After memset: %s\n", str);
            
            return 0;
        }
    Output:
        Before memset: Hello, World!
        After memset: *****, World!

    => Thay gia tri 0 vao shared voi so luong = sizeof(SharedData)
    */

    // Create two child processes
    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            readFileAndCalculateAverageRatings(argv[i + 1], shared);
            shmdt(shared); // Detach shared memory
            exit(EXIT_SUCCESS);
        }
    }

    // Parent process waits for children to complete
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // Print the results
    printAverages(shared);

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