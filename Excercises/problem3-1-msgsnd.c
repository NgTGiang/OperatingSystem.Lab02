/*Conventionally, message queue in the practice is used in a one-way communication method. However, we
 still can have some tricks to adapt it for two-way communication by using multi-thread mechanism.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define PERMS 0644
#define MSG_KEY_SEND 0x123   // Key for sending messages
#define MSG_KEY_RECV 0x124   // Key for receiving messages

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

void *receiver_thread(void *arg) {
    int msqid = *(int *)arg;
    struct my_msgbuf buf;
    int toend;

    printf("Receiver thread ready\n");
    
    while(1) {
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 2, 0) == -1) {  // Type 2 for received messages
            perror("msgrcv in receiver");
            break;
        }
        printf("Received: \"%s\"\n", buf.mtext);
        
        toend = strcmp(buf.mtext, "end");
        if (toend == 0) {
            break;
        }
    }
    
    return NULL;
}

int main(void) {
    struct my_msgbuf buf;
    int send_msqid, recv_msqid;
    pthread_t thread_id;
    
    // Create message queues
    if ((send_msqid = msgget(MSG_KEY_SEND, PERMS | IPC_CREAT)) == -1) {
        perror("msgget for send");
        exit(1);
    }
    if ((recv_msqid = msgget(MSG_KEY_RECV, PERMS | IPC_CREAT)) == -1) {
        perror("msgget for receive");
        exit(1);
    }

    // Start receiver thread
    if (pthread_create(&thread_id, NULL, receiver_thread, &recv_msqid) != 0) {
        perror("pthread_create");
        exit(1);
    }

    printf("Enter lines of text, 'end' to quit:\n");
    buf.mtype = 1;  // Type 1 for sent messages
    
    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        int len = strlen(buf.mtext);
        if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
        
        if (msgsnd(send_msqid, &buf, len+1, 0) == -1) {
            perror("msgsnd");
        }
        
        if (strcmp(buf.mtext, "end") == 0) {
            break;
        }
    }

    // Wait for receiver thread to finish
    pthread_join(thread_id, NULL);
    
    // Clean up message queues
    if (msgctl(send_msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl send");
    }
    if (msgctl(recv_msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl recv");
    }
    
    printf("Program exiting...\n");
    return 0;
}
