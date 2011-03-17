typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

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

int main ()
{
  char str_uint16[5];
  /*char str_tmp[5]; */
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
  return 0;
}
