/*******************************************************************************
 * Poject pristineSim is the simulator of FPGA to receive Frame via UDP.
 * This is the main entry for the pristineSim.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#include "udpreceiver.h"

#define UDP_PORT    9000
#define MAX_PACKET_SIZE     2000
#define ACK

static int received_complete_frame(struct FRAME_INFO frame_info);

void send_ack(struct ACK_PACKET ack_packet)
{

}

int main(int argc, char **argv)
{
    int pSocket;
    struct sockaddr_in pSocketAddr;
    char buffer[MAX_PACKET_SIZE];
    int recsize;
    socklen_t addr_len;

    if ((pSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        fprintf(stderr, "Socket failed.\n");
        return -1;
    }

    memset(&pSocketAddr, 0, sizeof(pSocketAddr));
    pSocketAddr.sin_family = AF_INET;
    pSocketAddr.sin_addr.s_addr = INADDR_ANY;
    pSocketAddr.sin_port = htons(UDP_PORT);

    if (bind(pSocket,(struct sockaddr *)&pSocketAddr, sizeof(pSocketAddr)) != 0){
        fprintf(stderr, "Socket bind failed.\n");
        close(pSocket);
        return -1;
    }


    struct COMMON_HEADER header;
    for(;;){
        recsize = recvfrom (pSocket, (void *)buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&pSocketAddr, &addr_len);
        printf("recsize: %d\n", recsize);

        memcpy((void *)&header, (void *)buffer, sizeof(header));


        printf("%s\n", (char *)buffer);

        memset(&buffer, 0, MAX_PACKET_SIZE);
        sleep(1);
    }

}

