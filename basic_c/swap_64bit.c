/*This file provides a method to swap every 64bit memory,
 * and demonstrates its usage with struct data*/
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#define SWAP(a,b) \
    { char c = a; a = b; b = c; }

#define SWAP16(a,b) \
    { uint16 c = a; a = b; b = c; }

#define SWAP32(a,b) \
    { uint32 c = a; a = b; b = c; }

/* Reverse byte order every 64bits in memory content,
 * which is pointed by ptr and of length bytes*/
void swap_every_64bit(char *ptr, size_t length)
{
	if (((length & 7) != 0) || (length == 0)) //lenght&7 = length%8
		return;
	char *tmp = ptr;
	int i = 1;
	while (i <= (length >> 3)){ //length>>3 = length/8
        	SWAP(tmp[0], tmp[7]);
        	SWAP(tmp[1], tmp[6]);
        	SWAP(tmp[2], tmp[5]);
        	SWAP(tmp[3], tmp[4]);
 	       tmp += 8;
		i++;
	}
}


struct simple_header {
    uint8 c1;
    uint8 c2;
    uint8 c3;
    uint8 c4;
    uint8 c5;
    uint8 c6;
    uint8 c7;
    uint8 c8;
}__attribute__((packed));

struct simple_header1 {
    uint8 c1;
    uint8 c2;
    uint8 c3;
    uint8 c4;
    uint8 c5;
    uint8 c6;
    uint16 c7;
}__attribute__((packed));

struct simple_header2 {
    uint16 c1;
    uint8 c2;
    uint8 c3;
    uint8 c4;
    uint8 c5;
    uint8 c6;
    uint8 c7;
}__attribute__((packed));

/* Reverse content (member order) of simple_header1 &2 */
/*struct simple_header2 *swap_simple_headers(struct simple_header1 sh1)
{
	struct simple_header2 sh2;
        return &sh2;
}*/

struct common_header {
	uint16 magic;
	uint8 version;
	uint8 type;
	uint32 seqno;

	uint32 id;
	uint32 timecode;
}__attribute__((packed));

struct reverse_header {
	uint32 timecode;
	uint32 id;

	uint32 seqno;
	uint8 type;
	uint8 version;
	uint16 magic;
}__attribute__((packed));

int main()
{
/*Test swap 64bit of struct simple_header1 & 2
 *Note last member in simple_header1:sh1.c7 is not char,
 *so after swap and re-assign to simple_header2,
 *the first member's value s2.c1 != s1.c7
 *Exception: when sh1.c7=256, which happens to be 1010,
 *so even after swapping, the value is still 1010*/    
      struct simple_header1 sh1 = {1,2,3,4,5,6,(256+2)};

        printf ("c1=%d,c2=%d,c3=%d,c4=%d,c5=%d,c6=%d,c7=%d\n",
                sh1.c1, sh1.c2, sh1.c3, sh1.c4, 
                sh1.c5, sh1.c6, sh1.c7);
	
        printf ("[Test] Before swap :\t%016llx\n", *(uint64 *)(&sh1));
        swap_every_64bit ((char *)&sh1, sizeof(sh1));
	printf ("[Test] Before swap :\t%016llx\n", *(uint64 *)(&sh1));
        
        struct simple_header2 *sh2 = (struct simple_header2 *)(&sh1);
        printf ("c1=%d,c2=%d,c3=%d,c4=%d,c5=%d,c6=%d,c7=%d\n",
                sh2->c1, sh2->c2, sh2->c3, sh2->c4, 
                sh2->c5, sh2->c6, sh2->c7);

/*Test swap 64bit of struct simple_header 
 *Because each member in simple_header is char, 
 *so swap result is what is expected.*/    
/*      struct simple_header sh = {1,2,3,4,5,6,7,8};

        printf ("c1=%d,c2=%d,c3=%d,c4=%d,c5=%d,c6=%d,c7=%d,c8=%d\n",
                sh.c1, sh.c2, sh.c3, sh.c4, 
                sh.c5, sh.c6, sh.c7, sh.c8);
	
        printf ("[Test] Before swap :\t%016llx\n", *(uint64 *)(&sh));
        swap_every_64bit ((char *)&sheader, sizeof(sheader));
	printf ("[Test] Before swap :\t%016llx\n", *(uint64 *)(&sh));
        
        printf ("c1=%d,c2=%d,c3=%d,c4=%d,c5=%d,c6=%d,c7=%d,c8=%d\n",
                sh.c1, sh.c2, sh.c3, sh.c4, 
                sh.c5, sh.c6, sh.c7, sh.c8);
*/


/*Test swap 64bit of struct common_header & reverse_header */	
/*	struct common_header header;
	header.magic = 111;
	header.version = 222;
	header.type = 223;
	header.seqno = 444;
	header.id = 555;
	header.timecode = 666;
	
        printf("Test header: \n\tmagic:%d\n", header.magic);
        printf("\tversion:%d\n", header.version);
        printf("\ttype:%d\n", header.type);
        printf("\tseqno:%d\n", header.seqno);
        printf("\tid:%d\n", header.id);
        printf("\ttimecode:%d\n", header.timecode);

	printf ("[Test] Before swap :\t%016llx\n", *(uint64 *)(&header));
        swap_every_64bit ((char *)&header, sizeof(header));
	printf ("[Test] Before swap :\t%16llx\n", *(uint64 *)(&header));
        
        printf("Reverse header: \n\ttimecode:%d\n", 
                ((struct reverse_header *)(&header))->timecode);
        printf("\tid:%d\n", ((struct reverse_header *)(&header))->id);
        printf("\tseqno:%d\n", ((struct reverse_header *)(&header))->seqno);
        printf("\ttype:%d\n", ((struct reverse_header *)(&header))->type);
        printf("\tversion:%d\n", ((struct reverse_header *)(&header))->version);
        printf("\tmagic:%d\n", ((struct reverse_header *)(&header))->magic);
*/
        return 0;
}

