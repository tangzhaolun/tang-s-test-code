typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

typedef unsigned long long uint64;

uint16 swap_endian_short(uint16 n)
{
  union {
    uint8 b[2];
    uint16 s;
  } u = {.s = n};

  return (u.b[0]<<8) | u.b[1];
}

uint16 swap_endian_short2(uint16 n)
{
  union {
    uint8 b[2];
    uint16 s;
  } u = {.b[0] = (n << 8), .b[1] = n};

  return u.s;
}

uint64 swap_64bit(uint64 data){
    return ((uint64)
            ((data<<56)&0xFF00000000000000L) | 
            ((data<<40)&0x00FF000000000000L) | 
            ((data<<24)&0x0000FF0000000000L) |
            ((data<<8) &0x000000FF00000000L) |
            ((data>>8 )&0x00000000FF000000L) | 
            ((data>>24)&0x0000000000FF0000L) | 
            ((data>>40)&0x000000000000FF00L) |
            ((data>>56)&0x00000000000000FFL) );
}

int main ()
{
    long long my64 = 55622322248322411LL;
    printf("long long:\t 0x%016llx\n", my64);
    printf("after swap:\t 0x%016llx\n", swap_64bit(my64));

  /*char str_uint16[5];
  uint16 n;

  char c = 'a';
  printf("Char %c:%d;\tsizeof(c):%d;\tsizeof(c<<8):%d\n", 
      c, c, sizeof(c), sizeof(c<<8));

  while(1){
    printf("Input an integar (0, %d] (0 to quit): ", USHRT_MAX);
    scanf("%s", str_uint16);
    n = (uint16) strtol (str_uint16, (char **) NULL, 10);
    if (n == 0)
      break;
    
    printf("Output integar: %d\n", n);
    printf("After swap big/little: %d\n", swap_endian_short(n));
  }
  return 0;*/
}
