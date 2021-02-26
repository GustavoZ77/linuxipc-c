#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd,fd2;
    char s[5] = "";
    char input[3];
    char response[32] ;

    char server_fifo_name[11] = "server_fifo";
    char client_fifo_name[15] = "client_fifo";
    int client= getpid();;
    do{
        char client_fifo_name[15] = "client_fifo";
        printf("Please enter the service you need: \n");
        printf("Type pp for Practicas profesionales or  ss for Servicios profesiones: \n");
        scanf("%s", &input);
        if(strcmp(input, "ss") == 0 || strcmp(input, "pp") ==0){
            sprintf(client_fifo_name, "%s%d", client_fifo_name, client);

            if (mkfifo(client_fifo_name, 0777) == -1){
                if(errno != EEXIST){
                    printf("Could not create a FIFO file");
                    return 1;
                }
            }

            sprintf(s,"%s%s",s,input);
            sprintf(s,"%s%s",s,":");
            sprintf(s,"%s%d",s,client);
            printf("SENDIND TO SERVER!!...  %s \n",s);

            if ((fd = open("server_fifo", O_WRONLY)) < 0){
                fprintf(stderr, "%s: failed to open SERVER FIFO %s\n","" , server_fifo_name);
                return 1;
            };

            if (write(fd, s, strlen(s)) <= 0){
                fprintf(stderr, "%s: failed to write CLIENT FIFO %s\n", "", server_fifo_name);
                return 2;
            }
            sleep(3);
            printf("Reading from %s \n",client_fifo_name);
            fd2 = open(client_fifo_name,O_RDONLY);
            if(fd2 < 0){
                fprintf(stderr, "%s: failed %d to open FIFO %s\n", argv[argc-argc],fd2, client_fifo_name);
                return 3;
            }

            int r = read(fd2, response, sizeof(char) * 16);
            if ( r < 0){
                fprintf(stderr, "%s: failed %d to read FIFO %s\n", argv[argc-argc],r, client_fifo_name);
                return 4;
            }
            printf("Result: %s \n",response);
            close(fd2);
            close(fd);
            memset(s, 0, strlen(s));
            memset(input, 0, strlen(input));
            printf("--------------------------------------------------------- \n");

        }else{
            memset(input, 0, strlen(input));
            break;
        }

    }while(strcmp(input,"exit") != 0);
    return 0;
}