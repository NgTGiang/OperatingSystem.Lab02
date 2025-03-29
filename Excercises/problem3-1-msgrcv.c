/*Conventionally, message queue in the practice is used in a one-way communication method. However, we
 still can have some tricks to adapt it for two-way communication by using multi-thread mechanism.*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>

#define PERMS 0644
#define MSG_KEY_SEND 0x123   // Key for receiving messages (opposite of sender)
#define MSG_KEY_RECV 0x124   // Key for sending messages (opposite of sender)

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

void *sender_thread(void *arg) {
    int msqid = *(int *)arg;
    struct my_msgbuf buf;
    
    printf("Sender thread ready. Enter responses ('end' to quit):\n");
    buf.mtype = 2;  // Type 2 for responses
    
    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        int len = strlen(buf.mtext);
        if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
        
        if (msgsnd(msqid, &buf, len+1, 0) == -1) {
            perror("msgsnd in sender");
            break;
        }
        
        if (strcmp(buf.mtext, "end") == 0) {
            break;
        }
    }
    
    return NULL;
}

int main(void) {
    struct my_msgbuf buf;
    int send_msqid, recv_msqid;
    pthread_t thread_id;
    int toend;
    
    // Create message queues
    if ((recv_msqid = msgget(MSG_KEY_SEND, PERMS | IPC_CREAT)) == -1) {
        perror("msgget for receive");
        exit(1);
    }
    if ((send_msqid = msgget(MSG_KEY_RECV, PERMS | IPC_CREAT)) == -1) {
        perror("msgget for send");
        exit(1);
    }

    // Start sender thread
    if (pthread_create(&thread_id, NULL, sender_thread, &send_msqid) != 0) {
        perror("pthread_create");
        exit(1);
    }

    printf("Receiver ready\n");
    
    while(1) {
        if (msgrcv(recv_msqid, &buf, sizeof(buf.mtext), 1, 0) == -1) {  // Type 1 for received messages
            perror("msgrcv");
            break;
        }
        printf("Received: \"%s\"\n", buf.mtext);
        
        toend = strcmp(buf.mtext, "end");
        if (toend == 0) {
            break;
        }
    }

    // Wait for sender thread to finish
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
