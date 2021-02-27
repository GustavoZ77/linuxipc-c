#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME "server_fifo"

int main() {
    char request[0x64];
    int len = 0x65;
    char *response = "zzsdfsdfg";

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
                    int ss_id = 123;
                    close(fd3[0]);
                    printf("Excuting %d child process, Getting 'Servicio social' for id  %s \n",child_id,client_name);
                    if(write(fd3[1], &ss_id, sizeof(int)*3)  == -1){
                        printf(" Error to write child PIPE \n");
                        return 10;
                    }
                    close(fd3[1]);
                return 0;
                }else{
                    //parent code
                    close(fd3[1]);
                    int nss_id;
                    if(read(fd3[0], &nss_id, sizeof(int)*3) == -1){
                        printf("Error to read from childt PIPE \n");
                        return 7;
                    }
                    printf("SSID: %d \n",&nss_id);
                    close(fd3[0]);
                }
            }
            if (write(fd2, response, strlen(response)) <= 0){
                fprintf(stderr, "%s: failed to write client_fifo_name %s\n", "", client_fifo_name);
                return 1;
            }
        printf("wrote %s in file %s \n",response, client_fifo_name);
        close(fd2);
        memset(request, 0, 64);
        }
        sleep(1);
    }
    return 0;
}
