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

#include <libsmscommon/system.hpp>

#include "udpreceiver.h"

#define UDP_PORT 9000
#define MAX_PACKET_SIZE 2000

struct stream {
	char *strm;
	char *p;
};

/* This function moves pointer 1 byte in the buffer.*/
inline uint8 stream_get_uint8(struct stream *strm)
{
	uint8 val = *(uint8 *)(strm->p);
	strm->p += sizeof(uint8);
	return val;
}

/* This function moves pointer 2 bytes in the buffer.
 * And transfer the network byte order to host order.*/
inline uint16 stream_get_uint16(struct stream *strm)
{
	uint16 val = ntohs(*(uint16 *)(strm->p));
	strm->p += sizeof(uint16);
	return val;
}

/* This function moves pointer 4 bytes in the buffer.
 * And transfer the network byte order to host order.*/
inline uint32 stream_get_uint32(struct stream *strm)
{
	uint32 val = ntohl(*(uint32 *)(strm->p));
	strm->p += sizeof(uint32);
	return val;
}


/* This function reads the frame common header out from the buffer.
 * Analysis of common header should be done in this function.*/
void *read_header(struct common_header *header,
                  struct stream *strm)
{
	assert(header != NULL);
	assert(strm != NULL);

	/* Read the common header.*/
	header->magic = stream_get_uint16(strm);
	header->version = stream_get_uint8(strm);
	header->pkt_type = stream_get_uint8(strm);
	header->pkt_seq_no = stream_get_uint32(strm);
	header->chan_id = stream_get_uint8(strm);
	header->_reserved0 = stream_get_uint8(strm);
	header->payload_len = stream_get_uint16(strm);
	header->timecode = stream_get_uint32(strm);
	header->frame_seq_no = stream_get_uint32(strm);
	header->subframe_idx = stream_get_uint16(strm);
	header->subframe_total = stream_get_uint16(strm);
	header->_reserved1[0] = stream_get_uint32(strm);
	header->_reserved1[1] = stream_get_uint32(strm);

	/* ToDo: analysis the common header. */
	return NULL;
}

/* This function sets the values of Ack packet.*/
void set_ack(struct ack_packet *ack)
{
	static int ack_packet_count = 0;
	ack->pkt_type = 0x32;
	ack->pkt_seq_no++;
	ack->payload_len = 8;
	/* ack->ack_seq_no = 0; */
	ack->audio_level = 0;
	ack->video_level = 0;

}


int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in addr;
	char buffer[MAX_PACKET_SIZE];
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
	struct ack_packet ack;
	int packet_count = 0;

	/* Main loop to receive packets and send back Act.*/
	for(;;){
		recsize = recvfrom (sock, buffer, sizeof(buffer), 0,
						(struct sockaddr *)&addr, &addr_len);
		printf("recsize: %d\n", recsize);

		struct stream strm;
		strm.strm = buffer;
		strm.p = buffer;

		/* Read and analyze packet common header.*/
		read_header(&header, &strm);

        sleep(1);

        /*Send an Act Packet every 16 successive received packets. */
        if (++packet_count == 16){
        	set_ack(&ack); /* Set the ack contetns.*/

        	addr.sin_addr.s_addr = htonl(0x7F000001); /* send to address 127.0.0.1 */
        	addr.sin_port = htons(UDP_PORT);

        	int byte_sent = sendto(sock, &ack, sizeof(ack), 0,
            				(struct sockaddr *)&addr, addr_len);
        	if (byte_sent < 0)
        		fprintf(stderr, "Error sending Act Packet. \n");
        	addr.sin_addr.s_addr = INADDR_ANY; /* Set address back to recvfrom */
        	addr.sin_port = htons(UDP_PORT);

        	packet_count = 0;
        }
	}

}


