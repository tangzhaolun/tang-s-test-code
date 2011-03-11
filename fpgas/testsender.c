#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#define UDP_PORT         9000
#define PACKET_SIZE     100

int main()
{
    int sock;
    struct sockaddr_in sa;
    char buffer[PACKET_SIZE];
    int byte_sent;
    int buffer_len;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        fprintf(stderr, "Socket failed.\n");
        return -1;
    }

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(0x7F000001); /* send to address 127.0.0.1 */
    sa.sin_port = htons(UDP_PORT);

    int i;
    for(i = 0; i < 5; i++){
        buffer_len = sprintf(buffer, "Hello %d ", i);
        byte_sent = sendto(sock, 
			   buffer, 
			   buffer_len + 1, 
			   0, 
			   (struct sockaddr *)&sa, 
			   sizeof sa);

	if (byte_sent < 0){
            fprintf(stderr, "Error sending packets \n");
            break;
        }
    }

    close(sock);
    return 0;

}

