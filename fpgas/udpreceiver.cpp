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
#include "cframe.hpp"
#include "cframebuffer.hpp"
#include "ctestplayer.hpp"

const int MAX_PACKET_SIZE = 2000;

struct stream
{
    char *strm;
    char *p;
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

int main(int argc, char **argv)
{
    char buffer[MAX_PACKET_SIZE];
    int recsize;

    CUDPClientSocket socketTransmit;
    CUDPServerSocket socketReceive;

    socketTransmit.create(9001, "127.0.0.1");
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
    CTestPlayer *testPlayer = new CTestPlayer(frameBuf);
    /* Setup frame data.*/
    CFrame *frame = NULL;
    uint8 *content_data = NULL;

    testPlayer->Start();

    int errorCount = 0;

    /* Main loop to receive packets and send back Act.*/
    for (;;)
    {
        recsize = socketReceive.recvFrom((uint8 *) buffer, sizeof(buffer));

        struct stream strm;
        strm.strm = buffer;
        strm.p = buffer;

        /* Read and analyze packet common header.*/
        read_common_header(&header, &strm);
        //        printf("Received %d bytes for packet %d.\n", recsize, header.pkt_seq_no);

        /*Send an Act Packet if frame packet is missing. */
        if (header.pkt_seq_no != received_pkt_seq_no)
        {
            errorCount++;
           // if (++packet_count == 16)
            {
                printf("Frame packet missing %d expected %d.\n", header.pkt_seq_no, received_pkt_seq_no);
                set_ack(&ack, (received_pkt_seq_no-1), frameBuf);
                int byte_sent = socketTransmit.sendTo((uint8*) &ack, sizeof(ack));
                if (byte_sent < 0)
                    fprintf(stderr, "Error sending Act Packet. \n");

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
            subframe_total = header.subframe_total;

            frame_info.hdr = header;
            read_info_header(&info_header, &strm);
            frame_info.infos = info_header;

            frame->m_Info = frame_info;

            //printf("Retrieve new frame %d\n", frame_info.hdr.subframe_total);

            /* TODO: alloc m_pData?
             * frame->m_pData = malloc (info_header.len_encrypt);
             */
            content_data = frame->m_pData;

            /* Next package should be frame data */
            states = PROCESS_FRAME_DATA;
        }
        else if (states == PROCESS_FRAME_DATA)
        {
            /* process frame data */
            /* TODO: frame->m_pData has allocated memory.
             * TODO: to setup all frame data, frame->m_pData should move automatically.
             * After call read_common_header, strm already points to content data.*/
            stream_get_bytes(&strm, content_data, header.payload_len);
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
            if (frame->m_Info.infos.content_type == 0x01)
            {
                frameBuf->queueFrame(frame);
            }
            else
            {
                //Do not queue up audio frames in the simulator
                frameBuf->releaseQueuedFrame(frame);
            }

            /* Reset flags here */
            subframe_count = 0;

            /* Next package should be frame info */
            states = PROCESS_FRAME_INFO;
            content_data = NULL;
            frame = NULL;
        }

        /*Send an Act Packet every 16 successive received packets. */
        if (++packet_count == 16)
        {
            set_ack(&ack, header.pkt_seq_no, frameBuf);
            int byte_sent = socketTransmit.sendTo((uint8*) &ack, sizeof(ack));
            if (byte_sent < 0)
                fprintf(stderr, "Error sending Act Packet. \n");
          //  printf("Send out Ack %d.\n", header.pkt_seq_no);

            packet_count = 0;
        }

    } /* end for()*/

    testPlayer->Stop();
    delete testPlayer;
}

