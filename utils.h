#ifndef _UTILS_
#define _UTILS_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

void bin_to_strhex(unsigned char *bin, unsigned int binsz, char **result)
{
  char          hex_str[]= "0123456789abcdef";
  unsigned int  i;

  *result = (char *)malloc(binsz * 2 + 1);
  (*result)[binsz * 2] = 0;

  if (!binsz)
    return;

  for (i = 0; i < binsz; i++)
    {
      (*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
      (*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
    }
}

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif

void hexdump(void *mem, unsigned int len)
{
  unsigned int i, j;

  for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
    {
      /* print offset */
      if(i % HEXDUMP_COLS == 0)
	{
	  printf("0x%06x: ", i);
	}

      /* print hex data */
      if(i < len)
	{
	  printf("%02x ", 0xFF & ((char*)mem)[i]);
	}
      else /* end of block, just aligning for ASCII dump */
	{
	  printf("   ");
	}

      /* print ASCII dump */
      if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
	{
	  for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
	    {
	      if(j >= len) /* end of block, not really printing */
		{
		  putchar(' ');
		}
	      else if(isprint(((char*)mem)[j])) /* printable char */
		{
		  putchar(0xFF & ((char*)mem)[j]);
		}
	      else /* other char */
		{
		  putchar('.');
		}
	    }
	  putchar('\n');
	}
    }
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
