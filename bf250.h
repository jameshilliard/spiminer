#ifndef _BF250_
#define _BF250_

#include <bcm2835.h>
#include "utils.h"

#define _CHAIN_LEN 3

#define SCLK RPI_V2_GPIO_P1_23
#define MOSI RPI_V2_GPIO_P1_19
#define MISO RPI_V2_GPIO_P1_21

// Command
#define CMD_Taskwrite 0x01
#define CMD_Switch    0x02
#define CMD_ReadNonce 0x04
#define CMD_SetClock  0x08
#define CMD_Toggle    0x10
#define CMD_SetMask   0x20

#define Toggle_Magic  0xA5000002

// Command Frame
struct bf250command
{
  BYTE  addr:4;
  BYTE  cmd;
  BYTE  len;
} __attribute__((__packed__));

struct bf250status
{
  BYTE nonce_counter:4;
  BYTE end_buffer:2;
  BYTE start_buffer:2;
} __attribute__((__packed__));;

struct bf250resp
{
  struct bf250status status;
  BYTE cmd_checksum;
  BYTE nonces[48];
  BYTE nonce_checksum;
};

// Channel

// a=start position from right, b=length
#define BITSLICE(x, a, b) ((x) >> (a)) & ((1 << ((b))) - 1)

#define _ARRAY_SIZE (_CHAIN_LEN*5) / 8 + 1

BYTE bitarray[_ARRAY_SIZE];

#define BF16 4
#define OUT0 6
#define OUT1 7
//#define DIS  7

static inline void setIndex(BYTE idx) {
  bitarray[idx / 8] |= (1 << (7-idx % 8));
}

static inline void setConfig(BYTE nChan,BYTE config) {
  int idx = (_ARRAY_SIZE)*8 - nChan*3;
  if (config&4) setIndex(idx);
  if (config&2) setIndex(idx+1);
  if (config&1) setIndex(idx+2);
}

void ClearChannelSeq(){
  memset(bitarray,0,(_ARRAY_SIZE));
}

void SetChannelSeq(BYTE *config,int len) {
  int i;
  for(i=0;i<len;i++) {
    setConfig(len-i,config[i]);
  }
}

void DumpChannelSeq() {
  int i;
  for(i=0;i<(_ARRAY_SIZE);i++){
    printf("%s",byte_to_binary(bitarray[i]));
  }
  printf("\n");
}

// Reset sequence
void ResetSeq(int n) {
  int i;
  // Put SCLK to high
  bcm2835_gpio_fsel(SCLK, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(SCLK, HIGH);

  // Generate n cycle on MOSI
  bcm2835_gpio_fsel(MOSI, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(MISO, BCM2835_GPIO_FSEL_OUTP);


  for (i=0;i<n;i++) {
    bcm2835_gpio_write(MOSI,HIGH);
    bcm2835_st_delay(bcm2835_st_read(),1);
    bcm2835_gpio_write(MOSI,LOW);
    bcm2835_st_delay(bcm2835_st_read(),1);
  }

  bcm2835_st_delay(bcm2835_st_read(),10);

  // Put SCLK to low
  bcm2835_gpio_write(SCLK,LOW);

  // Config back to SPI
  bcm2835_gpio_fsel(MOSI, BCM2835_GPIO_FSEL_ALT0);
  bcm2835_gpio_fsel(SCLK, BCM2835_GPIO_FSEL_ALT0);
  bcm2835_gpio_fsel(MISO, BCM2835_GPIO_FSEL_ALT0);
}


// generate clock value with prescaler and clock code
DWORD GetClockValue(BYTE prescaler,BYTE code) {
  DWORD ret;
  ret = 0;
  ret = 0x038<<20;
  if (prescaler==0) {
    ret |= 1<<19;
    ret |= 1<<12;
  }
  ret |= ((DWORD)(code&0x3F))<<6;
  //  ret |= ((DWORD)(code&0x3F))<<13;
  return __bswap_32(ret);
}


#endif
