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

    printf("Listenig request..\n");

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

            printf("Processing request from client: %s \n", client_name);

            char client_fifo_name[12] = "client_fifo";
            sprintf(client_fifo_name, "%s%s", client_fifo_name, client_name);

            int fd2;
            printf("Opening file %s \n",client_fifo_name);
            if ((fd2 = open(client_fifo_name, O_WRONLY))  < 0){
                fprintf(stderr, "%s: failed to open client_fifo_name %s\n","" , client_fifo_name);
                return 1;
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
