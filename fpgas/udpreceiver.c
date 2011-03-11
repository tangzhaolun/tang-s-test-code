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
#include <assert.h>

#include "udpreceiver.h"

#define UDP_PORT	    9000
#define MAX_PACKET_SIZE     2000

struct buffer {
    char *buf;
    char *p;
};

inline uint8 buffer_get_uint8(struct buffer *buf)
{
    uint8 val = *(uint8 *)(buf->p);
    buf->p += sizeof(uint8);
    return val;
}

inline uint16 buffer_get_uint16(struct buffer *buf)
{
    uint16 val = ntohs(*(uint16 *)(buf->p));
    buf->p += sizeof(uint16);
    return val;
}

inline uint32 buffer_get_uint32(struct buffer *buf)
{
    uint32 val = ntohl(*(uint32 *)(buf->p));
    buf->p += sizeof(uint32);
    return val;
}

void *read_header(struct common_header *header,
                  struct buffer *buf)
{
    assert(header != NULL);
    assert(buf != NULL);

    header->magic = buffer_get_uint16(buf);
}

int main(int argc, char **argv)
{
    int sock;
    struct sockaddr_in addr;
    char data[MAX_PACKET_SIZE];
    int recsize;
    int retval;
    
    socklen_t addr_len;


    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1){
        fprintf(stderr, "Socket failed.\n");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(UDP_PORT);

    retval = bind(sock,(struct sockaddr *)&addr, sizeof(addr));
    if(retval != 0){
        fprintf(stderr, "Socket bind failed.\n");
        close(sock);
        return -1;
    }

    struct common_header header;
    int i = 0;
    for(;;){
         
        recsize = recvfrom (sock, data, sizeof(data), 0,
			(struct sockaddr *)&addr, &addr_len);
        printf("recsize: %d\n", recsize);
        
        struct buffer buf;
        buf.buf = data;
        buf.p = data;

        read_header(&header, &buf);

        sleep(1);

        if (++i == 16){
            /* sendto(); */
            i = 0;
        }
    }

}

