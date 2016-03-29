#ifndef _UTILS_
#define _UTILS_

#include <stdlib.h>
#include <string.h>

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

const char *byte_to_binary(BYTE x) {
      static char b[9];
      b[0] = '\0';
      int z;
      for(z=128;z>0;z>>=1){
            strcat(b,((x&z)==z)?"1":"0");
      }
      return b;
}

double n2float(DWORD nBits) {
      double diff;
      DWORD shift = (nBits >> 24) & 0x00ff;

      nBits &= 0x00FFFFFF;
      diff = (double)0x00ffff/(double)nBits;
      if (shift < 29)  {
            while (shift < 29) {
                  diff *= 256.0;
                  shift++;
            }
      } else {
            while (shift > 29) {
                  diff /= 256.0;
                  shift--;
            }
      }
      return diff;
}

double hash2diff(BYTE *targ) {
      int i, shift;
      DWORD nBits;

      i = 31; shift=32;
      while ( (targ[i] == 0) && (targ[i - 1] < 0x80) ) {
            i--;
            shift--;
            if (i - 1 < 0) {
                  return 0.0;
            }
      }
      nBits = (shift << 24) + (targ[i] << 16) + (targ[i - 1] << 8) + targ[i - 2];
      return n2float(nBits);
}

#endif
