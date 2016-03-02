#ifndef _UTILS_
#define _UTILS_

typedef unsigned char           BYTE;                           /* 8-bit unsigned  */
typedef unsigned short int      WORD;                           /* 16-bit unsigned */
typedef unsigned long           DWORD;                          /* 32-bit unsigned */

BYTE checksum(void *buffer, BYTE len)
{
      BYTE *buf = (BYTE *)buffer;
      BYTE sum;
      BYTE i;
      for (i = 0; i < len; ++i)
	sum += (BYTE)(*buf++);
      return sum;
}

#endif
