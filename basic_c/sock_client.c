/* This file is for unit test of sock_select.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


const int PORT = 9000;

int main(int argc, char *argv[])
{
    struct sockaddr_in clientaddr;    /* client address */
    int sock;
    char buf[1024];   /* buffer for client data */
    int nbytes;
    char client_id[100];
    
    if (argc > 1)
        strcpy(client_id, argv[1]);
    else
        strcpy(client_id, argv[0]);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        fprintf(stderr, "socket() failed.\n");
        return -1;
    }

    memset(&(clientaddr), '\0', sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientaddr.sin_port = htons(PORT);
    
    
    int ret = connect(sock, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
    if (ret < 0){
        fprintf(stderr, "connect() failed.\n");
        return -1;
    }
    
    printf("Ready to send data ...\n");
    int i;
    for (i = 0; i < 3; i++){
        char str[100] = {"Hello from "};
        strcat (str, client_id);
        strcpy (buf, str);
        nbytes = send(sock, buf, sizeof (buf), 0);
        if (nbytes < 0)
            fprintf(stderr, "Sending data failed.\n");
        
        sleep(4);
    }
    printf("Sending data done.\n");

    return 0;    
}
