
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int main()
{
    int msgid;
    printf("Listening messages... \n");
    while(1){
        sleep(1);
        key_t key;
        key = ftok("mserverfile2", 77);
        msgid = msgget(key, 0777 | IPC_CREAT);
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        printf("Request received for client : %s \n",
               message.mesg_text);
        printf("processing... \n");
        //sleep(2);
        strcpy(message.mesg_text, "123456");
        msgsnd(msgid, &message, sizeof(message), 0);
        strcpy(message.mesg_text, "");
        msgctl(msgid, IPC_RMID, NULL);

    }
    return 0;
} 
