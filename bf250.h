#ifndef _BF250_
#define _BF250_

#include <bcm2835.h>
#include "utils.h"

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

BYTE *bitarray;

void initBitArray(int size) {
  bitarray = (BYTE *)calloc(size / 8 + 1, 1);
}

void resetBitArray() {
  free(bitarray);
}

static inline void setIndex(BYTE* bitarray, size_t idx) {
    bitarray[idx / 8] |= (1 << (idx % 8));
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
    bcm2835_st_delay(0,1);
    bcm2835_gpio_write(MOSI,LOW);
    bcm2835_st_delay(0,1);
  }

  // Put SCLK to low
  bcm2835_gpio_write(SCLK,LOW);

  // Config back to SPI
  bcm2835_gpio_fsel(SCLK, BCM2835_GPIO_FSEL_ALT0);
  bcm2835_gpio_fsel(MOSI, BCM2835_GPIO_FSEL_ALT0);
  bcm2835_gpio_fsel(MISO, BCM2835_GPIO_FSEL_ALT0);
}



#endif
