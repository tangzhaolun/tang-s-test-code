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
    fd_set master;    /* master file descriptor list */
    fd_set read_fds;    /* temp file descriptor list for select() */
    struct sockaddr_in serveraddr;    /* server address */
    struct sockaddr_in clientaddr;    /* client address */
    int fdmax;    /* maximum file descriptor number */
    int listener;   /* listening socket descriptor */
    int newfd;    /* newly accept()ed socket descriptor */
    char buf[1024];   /* buffer for client data */
    int nbytes;
    struct timeval tv = {2, 500000};

    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
   
    /* get the listener socket fd*/
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("Server-socket() error!");
        return -1;
    }
    printf("Server-socket() is OK...\n");

    /* bind */
    memset(&(serveraddr), '\0', sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    
    if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
        printf("Server-bind() error!");
        return -1;
    }
    printf("Server-bind() is OK...\n");

    /* listen */
    if(listen(listener, 10) == -1){
        printf("Server-listen() error!");
        return -1;
    }
    printf("Server-listen() is OK...\n");

    /* frist step: add the listener socket fd to the master set */
    FD_SET(listener, &master);

    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/

    for(;;){
        read_fds = master; /* understand why backup master fd_set */
        int ret = select(fdmax+1, &read_fds, NULL, NULL, &tv);
        if (ret == -1){
            printf("Server-select() error!");
            return -1;
        }
        if (ret == 0) { /* timeval = 0 */
            printf("Time out.\n");
            tv.tv_sec = 2;
            continue;
        }

        /* if ret != 0 or 1, ret means the current fd from select, so
         * run through all file descritors,include listening and connection fds*/
        int i;
        /*for(i = 0; i <= fdmax; i++){ */
        for(i = 0; i <= ret; i++){
            if(FD_ISSET(i, &read_fds)){ 
                
                /* if it's listener socket, accept new connections */
                if(i == listener){ 
                    int addrlen = sizeof(clientaddr);
                    newfd = accept(listener, 
                            (struct sockaddr *)&clientaddr, &addrlen);
                    if (newfd == -1){
                        printf("Server-accept() error!");
                    }
                    else{
                        FD_SET(newfd, &master); /* add to master set */
                        if(newfd > fdmax){ /* keep track of the maximum */
                            fdmax = newfd;
                        }
                        printf("\tAccept new connection from %s on socket %d\n", 
                                inet_ntoa(clientaddr.sin_addr), newfd);
                    }
                } /* finish if handling to accept new fd*/
                
                /* if not a listener socket, it is newfd to read or write data*/
                else{ /* handle data from a client */
                    nbytes = recv(i, buf, sizeof(buf), 0);
                    if (nbytes <= 0){
                        fprintf(stderr, "recv error from fd %d, "
                                "due to connection shutdown\n", i);
                        close (i);
                        FD_CLR(i, &master);
                        continue;
                    }
                    printf("\tReciving from connection %d: %s\n", i, buf);
                } /* finish if-else handling of accepted connection */
            } /* end of if FD_ISSET judgement*/ 
        } /* end of for(), the loop of all fds */
    
    } /* end of top-level for (), to start select() again */

    return 0;
}
