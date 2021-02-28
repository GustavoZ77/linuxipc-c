
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


struct mesg_buffer {
    long mesg_type;
    char mesg_text[4];
} message;

int search_in_file();
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
        if(search_in_file() == 1){
            strcpy(message.mesg_text, "1");
        }else{
            strcpy(message.mesg_text, "0");
        }
        //sleep(2);
        printf("processing %s... \n",message.mesg_text);
        msgsnd(msgid, &message, sizeof(message), 0);
        strcpy(message.mesg_text, "");
        msgctl(msgid, IPC_RMID, NULL);

    }
    return 0;
}

int search_in_file() {

    FILE *file;
    char buffer[5];
    file = fopen("db.txt","r");

    if (file == NULL){
        printf("Error to read from db \n");
    }
    else{
        while (feof(file) == 0){
            fgets(buffer,5,file);
            int comp = strcmp(buffer, message.mesg_text);
            if(comp == -3){
                printf("found!! %d \n",comp);
                return 1;
            }
        }
    }
    fclose(file);
    return 0;
}
