#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define FIFO_NAME "server_fifo"

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int main() {

    key_t key;
    char request[0x64];
    int len = 0x65;
    char response[32];

    printf("Listenig for request..\n");

    if (mkfifo(FIFO_NAME, 0777) == -1){
        if(errno != EEXIST){
            printf("Could not create a FIFO file");
            return 1;
        }
    }
    int fd = open(FIFO_NAME,O_RDONLY);
    while(1){
        read(fd, request, sizeof(int) * 0x64-1);
        if(strlen(request) > 0){
            printf("%s \n",request);
            char *token = strtok (request, ":");
            char *params[2];
            int i =0;
            while (token != NULL)
            {
                params[i++] = token;
                token = strtok (NULL, ":");
            }

            char *service_type = params[0];
            char *client_name = params[1];
            //client_name[strlen(client_name) - 1] = 0;

            printf("Processing request %s from client: %s \n",service_type, client_name);

            char client_fifo_name[12] = "client_fifo";
            sprintf(client_fifo_name, "%s%s", client_fifo_name, client_name);

            int fd2;
            printf("Opening file %s \n",client_fifo_name);
            if ((fd2 = open(client_fifo_name, O_WRONLY))  < 0){
                fprintf(stderr, "%s: failed to open client_fifo_name %s\n","" , client_fifo_name);
                return 1;
            }
            int fd3[2]; // [0] parent read/write
            //int fd4[2]; // [0] child  read/write
            //process SS using PIPE
            if(strcmp(service_type, "ss") == 0) {
                if(pipe(fd3) == -1){
                    fprintf(stderr, "%s: Error to cresate SS fd3 process PIPE \n");
                    return 2;
                }
                int id = fork();
                if(id == 0){
                    //child code
                    int child_id = getpid();
                    int ss_id = 123987;
                    close(fd3[0]);
                    printf("Excuting %d child process, Getting 'Servicio social' for id  %s \n",child_id,client_name);
                    if(write(fd3[1], &ss_id, sizeof(int)*6)  == -1){
                        printf(" Error to write child PIPE \n");
                        return 10;
                    }
                    close(fd3[1]);
                }else{
                    //parent code
                    close(fd3[1]);
                    int nss_id;
                    if(read(fd3[0], &nss_id, sizeof(int)*6) == -1){
                        printf("Error to read from childt PIPE \n");
                        return 7;
                    }
                    close(fd3[0]);
                    sprintf(response,"Response: OK %d",nss_id);
                    sleep(1);
                    if (write(fd2, response, sizeof(char )*32) <= 0){
                        fprintf(stderr, "%s: failed to write client_fifo_name %s\n", "", client_fifo_name);
                        return 1;
                    }
                    printf("----------------------DONE----------------------------- \n");
                }
            }
            int comp = strcmp(service_type, "pp");
            if(comp== 0){
                int msgid;
                key = ftok("mserverfile2", 77);
                msgid = msgget(key, 0777 | IPC_CREAT);
                message.mesg_type = 1;

                strcpy(message.mesg_text, client_name);

                msgsnd(msgid, &message, sizeof(message), 0);
                printf("waiting for response...\n");

                msgrcv(msgid, &message, sizeof(message), 1, 0);
                sprintf(response,"Response: OK %s",message.mesg_text);
                printf("responding %s ...\n",response);
                if (write(fd2, response, sizeof(char )*32) <= 0){
                    fprintf(stderr, "%s: failed to write client_fifo_name %s\n", "", client_fifo_name);
                    return 1;
                }
                printf("----------------------DONE----------------------------- \n");

            }
        close(fd2);
        }
        memset(response, 0, 32);
        memset(request, 0, 64);
        sleep(1);
    }
    return 0;
}
