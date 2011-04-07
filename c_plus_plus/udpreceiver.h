#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <libsmscommon/cudpsocket.hpp>


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;


struct common_header
{
	uint16 magic;
	uint8 version;
	uint8 pkt_type;
	uint32 pkt_seq_no;
	uint8 chan_id;
	uint8 _reserved0;
	uint16 payload_len;
	uint32 timecode;
	uint32 frame_seq_no;
	uint16 subframe_idx;
	uint16 subframe_total;
	uint32 _reserved1[2];
}__attribute__((packed));

struct frame_info_header
{
	uint8 content_type;
	uint8 content_info;
	uint16 flags;
	uint32 len_encrypt;
	uint32 len_unencrypt;
	uint32 plaintext_ofs;
	uint8 key_uuid[16];
	uint8 iv[16];
	uint8 cv[16];
	uint8 so_lnfld_tf_id;
	uint8 so_lnfld_sqn;
	uint8 so_lnfld_mic;
	uint8 _reserved0;
	uint8 lnfld_tf_id[4];
	uint8 track_fileid[16];
	uint8 lnfld_sqn[4];
	uint8 track_seqno[8];
	uint8 lnfld_mic[4];
	uint32 _reserved1;
	uint32 _reserved2;
}__attribute__((packed));

/* The complete Frame Info */
struct frame_info
{
	struct common_header hdr; /* Header */
	struct frame_info_header infos; /* Setup infos */
}__attribute__((packed));

struct frame_data
{
	struct common_header hdr;
	void * content;
}__attribute__((packed));

struct ack_packet
{
	uint16 magic;
	uint8 version;
	uint8 pkt_type;
	uint32 pkt_seq_no;
	uint8 chan_id;
	uint8 _reserved0;
	uint16 payload_len;
	uint32 _reserved1;
	uint32 ack_seq_no;
	uint16 _reserved2;
	uint8 audio_level;
	uint8 video_level;
}__attribute__((packed));

struct SSETUP_HEADER
{
    uint32 imageHeight;
    uint32 imageWidth;
    uint32 frameRate;
}__attribute__((packed));

struct SSETUP_PACKET
{
    struct common_header hdr; /* Header */
    struct SSETUP_HEADER setup; /* Setup infos */
}__attribute__((packed));


#endif
