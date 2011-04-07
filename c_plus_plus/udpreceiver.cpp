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
#include "cframe.hpp"
#include "cframebuffer.hpp"

#ifndef __x86_64__
#include "ctestplayer.hpp"
#else
#include "cpristine.hpp"
#endif

const int MAX_PACKET_SIZE = 2000;

struct stream
{
    char *strm;	//alway point to the head of the stream
    char *p;	//may point to any position in the stream
};

/* This function moves pointer 1 byte in the buffer.*/
inline uint8 stream_get_uint8(struct stream *strm)
{
    uint8 val = *(uint8 *) (strm->p);
    strm->p += sizeof(uint8);
    return val;
}

/* This function moves pointer 2 bytes in the buffer.
 * And transfer the network byte order to host order.*/
inline uint16 stream_get_uint16(struct stream *strm)
{
    uint16 val = be16_to_cpu(*(uint16 *)(strm->p));
    strm->p += sizeof(uint16);
    return val;
}

/* This function moves pointer 4 bytes in the buffer.
 * And transfer the network byte order to host order.*/
inline uint32 stream_get_uint32(struct stream *strm)
{
    uint32 val = be32_to_cpu(*(uint32 *)(strm->p));
    strm->p += sizeof(uint32);
    return val;
}

/* Read stream, from position pointed by its member pointer p
 * following lengh bytes, to memory pointed by dest*/
inline void stream_get_bytes(struct stream *strm, void *dest, size_t length)
{
    memcpy(dest, strm->p, length);
    strm->p += length;
}

/* This function reads the frame common header
 * from the buffer to struct common_header pointer.*/
void read_common_header(struct common_header *header, struct stream *strm)
{
    assert(header != NULL);
    assert(strm != NULL);

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
}

void read_info_header(struct frame_info_header *header, struct stream *strm)
{
    assert(header != NULL);
    assert(strm != NULL);

    header->content_type = stream_get_uint8(strm);
    header->content_info = stream_get_uint8(strm);
    header->flags = stream_get_uint16(strm);
    header->len_encrypt = stream_get_uint32(strm);
    header->len_unencrypt = stream_get_uint32(strm);
    header->plaintext_ofs = stream_get_uint32(strm);

    /* 24*4 are the bytes for the rest of info_header */
    stream_get_bytes(strm, header->key_uuid, 24 * 4);
}

void read_setup_header(struct SSETUP_HEADER *header, struct stream *strm)
{
    assert(header != NULL);
    assert(strm != NULL);

    header->imageHeight = stream_get_uint32(strm);
    header->imageWidth = stream_get_uint32(strm);
    header->frameRate = stream_get_uint32(strm);
}

/* This function sets the values of Ack packet.*/
void set_ack(struct ack_packet *ack, int frame_pkt_seq_no, CFrameBuffer *pFrameBuffer)
{
    static int ack_packet_count = 0;

    ack->pkt_type = 0x32;
    ack->pkt_seq_no = cpu_to_be32(ack_packet_count++);
    ack->payload_len = cpu_to_be16(8);
    ack->ack_seq_no = cpu_to_be32(frame_pkt_seq_no);
    ack->audio_level = 0;
    ack->video_level = pFrameBuffer->getVideoLevel();
}

#define SWAP(a,b) \
    { char c = a; a = b; b = c; }
/* Reverse byte order every 64bits in memory content,
 * which is pointed by ptr and of length bytes*/
void swap_every_64bit(char *ptr, size_t length)
{
	if (((length & 7) != 0) || (length == 0))
		return;
	char *tmp = ptr;
	int i = 1;
	while (i <= (length >> 3)){ //length / 8
        SWAP(tmp[0], tmp[7]);
        SWAP(tmp[1], tmp[6]);
        SWAP(tmp[2], tmp[5]);
        SWAP(tmp[3], tmp[4]);
		//*(uint64*)tmp = EndianSwap_64(*((uint64*)tmp));
		
        tmp += 8;
		i++;
	}
}

int main(int argc, char **argv)
{
    char buffer[MAX_PACKET_SIZE];
    int recsize;

    CUDPClientSocket *pSocketTransmit = 0;
    int32 nSocketNum = -1;
    CUDPServerSocket socketReceive;

    socketReceive.create(9000, "127.0.0.1");

    struct common_header header;
    struct frame_info_header info_header;
    struct frame_info frame_info;
    struct ack_packet ack;

    int packet_count = 0;
    int received_pkt_seq_no = 0;
    int subframe_total = 0;
    int subframe_count = 0;

    enum
    {
        PROCESS_FRAME_INFO, PROCESS_FRAME_DATA,
    };
    int states = PROCESS_FRAME_INFO;

    CFrameBuffer *frameBuf = new CFrameBuffer();
#ifndef __x86_64__
    CTestPlayer *testPlayer = new CTestPlayer(frameBuf);
#else
    CPristine *testPlayer = new CPristine(frameBuf);
#endif
    /* Setup frame data.*/
    CFrame *frame = NULL;
    uint8 *content_data = NULL;

    int errorCount = 0;

    testPlayer->Start();

    /* Main loop to receive packets and send back Act.*/
    for (;;)
    {
        recsize = socketReceive.recvFrom((uint8 *) buffer, sizeof(buffer));

        // Detect the IP address of the calling client
        if (socketReceive.getRecvIPAddr() != nSocketNum)
        {
            if (pSocketTransmit != 0)
            {
                delete pSocketTransmit;
            }

            pSocketTransmit = new CUDPClientSocket();

            char *ipAddr = socketReceive.getRecvIPAddrString();
            if (pSocketTransmit->create(9001, ipAddr) != 0)
            {
                printf("Failed to create transmit socket %s\n", ipAddr);
            }
            else
            {
                printf("New transmit socket created for %s\n", ipAddr);
                nSocketNum = socketReceive.getRecvIPAddr();
            }

        }

        struct stream strm;
        strm.strm = buffer;
        strm.p = buffer;

        /* Read and analyze packet common header.*/
        read_common_header(&header, &strm);

        if (header.pkt_type == 0x40) // setup packet
        {

            printf("Setup packet received\n");

            struct SSETUP_HEADER setupHeader;
            read_setup_header(&setupHeader, &strm);

            packet_count = 0;
            received_pkt_seq_no = 0;

            if (frame != 0)
            {
                frameBuf->releaseQueuedFrame(frame);
            }

           // subframe_count = 0;
            content_data = 0;
            states = PROCESS_FRAME_INFO;
            frame = 0;

#ifdef __x86_64__
            testPlayer->configure(setupHeader.imageHeight, setupHeader.imageWidth, setupHeader.frameRate);
#endif

            continue;
        }

        /*Send an Act Packet if frame packet is missing. */
        if (header.pkt_seq_no != received_pkt_seq_no)
        {
            errorCount++;
            if (++packet_count == 16)
            {
                printf("Frame packet missing %d expected %d.\n", header.pkt_seq_no, received_pkt_seq_no);
                set_ack(&ack, (received_pkt_seq_no - 1), frameBuf);
                if (pSocketTransmit != 0)
                {
                    int byte_sent = pSocketTransmit->sendTo((uint8*) &ack, sizeof(ack));
                    if (byte_sent < 0)
                        fprintf(stderr, "Error sending Act Packet. \n");
                }
                else
                {
                    fprintf(stderr, "Error sending Act Packet. \n");
                }

                packet_count = 0;
                //       	printf("Send out Ack.\n");
            }

            if (errorCount > 10)
            {
                //return 0;
            }
            continue; /* Ignore current packet and start receiving new one.*/
        }

        received_pkt_seq_no++;


        if (states == PROCESS_FRAME_INFO)
        {
            /* process frame info */
            frame = frameBuf->getNewFrame();
            if (frame != 0)
            {
                subframe_total = header.subframe_total;

                /* Swap common header every 64 bit before put into frameBuf.*/
                swap_every_64bit ((char *)&header, sizeof(header));
                frame_info.hdr = header;

                read_info_header(&info_header, &strm);
                /* Swap info header every 64 bit.before put into frameBuf*/
                swap_every_64bit ((char *)&info_header, sizeof(info_header));
                frame_info.infos = info_header;

                frame->m_Info = frame_info;

                //printf("Retrieve new frame %d\n", frame_info.hdr.subframe_total);

                content_data = frame->m_pData;

                subframe_count = 0;
            }
            /* Next package should be frame data */
            states = PROCESS_FRAME_DATA;
        }
        else if (states == PROCESS_FRAME_DATA)
        {
            /* Swap common header every 64 bit before put into frameBuf.*/
            swap_every_64bit ((char *)&header, sizeof(header));
        	memcpy(content_data, &header, sizeof (header)); //copy frame data's header
        	content_data += sizeof (header);

            memcpy(content_data, strm.p, header.payload_len); //copy frame data's content
            content_data += header.payload_len;
            subframe_count++;
        }
        else
        {
            /* Should not reach */
            assert (0);
        }

        if (subframe_count == subframe_total)
        {
           // printf("Frame Complete %d\n", frame->m_Info.infos.content_type);
            if (frame != 0)
            {
                if (frame->m_Info.infos.content_type == 0x01)
                {
                    frameBuf->queueFrame(frame);
                }
                else
                {
                    //Do not queue up audio frames in the simulator
                    frameBuf->releaseQueuedFrame(frame);
                }
            }
            /* Reset flags here */
            //subframe_count = 0;

            /* Next package should be frame info */
            states = PROCESS_FRAME_INFO;
            content_data = NULL;
            frame = NULL;
        }

        /*Send an Act Packet every 16 successive received packets. */
        if (++packet_count == 16)
        {
            set_ack(&ack, header.pkt_seq_no, frameBuf);
            if (pSocketTransmit != 0)
            {
                int byte_sent = pSocketTransmit->sendTo((uint8*) &ack, sizeof(ack));
                if (byte_sent < 0)
                    fprintf(stderr, "Error sending Act Packet. \n");
            }
            else
            {
                fprintf(stderr, "Error sending Act Packet. \n");
            }

            //printf("Send out Ack %d.\n", header.pkt_seq_no);

            packet_count = 0;
        }

    } /* end for()*/

    testPlayer->Stop();
    delete testPlayer;
}

