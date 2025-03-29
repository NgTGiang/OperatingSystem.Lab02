#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <unistd.h>

#define PERMS 0644
#define MSG_KEY 0x123

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

void *send_messages(void *arg) {
    struct my_msgbuf buf;
    int msqid = *((int *)arg);
    int len;

    buf.mtype = 1; /* message type */
    printf("Enter lines of text, ^D to quit:\n");

    while (fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        len = strlen(buf.mtext);
        /* Remove newline at end, if it exists */
        if (buf.mtext[len - 1] == '\n') buf.mtext[len - 1] = '\0';
        
        if (msgsnd(msqid, &buf, len + 1, 0) == -1) { /* +1 for '\0' */
            perror("msgsnd");
        }
    }

    strcpy(buf.mtext, "end");
    len = strlen(buf.mtext);
    if (msgsnd(msqid, &buf, len + 1, 0) == -1) /* +1 for '\0' */
        perror("msgsnd");

    return NULL;
}

int main(void) {
    struct my_msgbuf buf;
    int msqid;

    if ((msqid = msgget(MSG_KEY, PERMS | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

    pthread_t sender_thread;
    pthread_create(&sender_thread, NULL, send_messages, &msqid);

    /* Receiver logic in the main thread */
    printf("Message queue: ready to receive messages.\n");
    for (;;) {
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Received: \"%s\"\n", buf.mtext);
        if (strcmp(buf.mtext, "end") == 0)
            break;
    }

    pthread_join(sender_thread, NULL);
    
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    
    printf("Message queue: done sending messages.\n");
    return 0;
}
