#ifndef _UTILS_
#define _UTILS_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>


typedef unsigned char           BYTE;                           /* 8-bit unsigned  */
typedef unsigned short int      WORD;                           /* 16-bit unsigned */
typedef unsigned int            DWORD;                          /* 32-bit unsigned */


BYTE chksum(void *buffer, BYTE len)
{
      BYTE *buf = (BYTE *)buffer;
      BYTE sum;
      BYTE i;
      for (i = 0; i < len; ++i)
            sum += (BYTE)(*buf++);
      return sum;
}

int hex2bin(BYTE *s)
{
    int ret=0;
    int i;
    for( i=0; i<2; i++ )
    {
        char c = *s++;
        int n=0;
        if( '0'<=c && c<='9' )
            n = c-'0';
        else if( 'a'<=c && c<='f' )
            n = 10 + c-'a';
        else if( 'A'<=c && c<='F' )
            n = 10 + c-'A';
        ret = n + ret*16;
    }
    return ret;
}

void bin2c(BYTE *buf,BYTE *s,int len) {
  int i;
  int offset=0;
  offset = sprintf(buf,"{\n");
  for(i=0;i<len;i++){
    offset+=sprintf(buf+offset,"0x%02x, ",s[i]^0xAA);
    if (i%8==7) offset+=sprintf(buf+offset,"\n");
  }
  sprintf(buf+offset,"0x00, 0x00, 0x00, 0x00 \n};\n");
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

void decodeRx(BYTE*output,BYTE *rx,int n) {
  int offset = 33-__builtin_clz(*rx);
  int i;
  for(i=0;i<n-1;i++){
    output[i]=(rx[i]<<offset)|(rx[i+1]>>(8-offset));
  }
  output[n-1]=rx[n-1]<<offset;
}

void decodeRx2(BYTE*output,BYTE *rx,int n) {
  int offset = 33-__builtin_clz(*rx)+1;
  int i;
  for(i=0;i<n-1;i++){
    output[i]=(rx[i]<<offset)|(rx[i+1]>>(8-offset));
  }
  output[n-1]=rx[n-1]<<offset;
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
