#ifndef _BF250_
#define _BF250_

#include "spidriver.h"

// Command
#define CMD_Taskwrite 0x01
#define CMD_Switch    0x02
#define CMD_ReadNonce 0x04
#define CMD_SetClock  0x08
#define CMD_Toggle    0x10
#define CMD_SetMask   0x20

#define Toggle_Magic  0xA5000002


// Reset sequence
void ResetSeq(int n) {
  int i;
  struct timespec sleepValue;
  sleepValue.tv_sec = 0;
  sleepValue.tv_nsec = 1L;
  
  // Put SCLK to high
  INP_GPIO(SCLK);OUT_GPIO(SCLK);
  GPIO_SET = 1 << SCLK;

  // Generate n cycle on MOSI
  INP_GPIO(MOSI);OUT_GPIO(MOSI);
  INP_GPIO(MISO);OUT_GPIO(MISO);
  for (i=0;i<n;i++) {
    GPIO_SET = 1 << MOSI;
    nanosleep(sleepValue,NULL);
    GPIO_CLR = 1 << MOSI;
    nanosleep(sleepValue,NULL);
  }

  // Put SCLK to low
  GPIO_CLR = 1 << SCLK;

  // Config back to SPI
  INP_GPIO(SCLK);SET_GPIO_ALT(SCLK,0);
  INP_GPIO(MOSI);SET_GPIO_ALT(MOSI,0);
  INP_GPIO(MISO);SET_GPIO_ALT(MISO,0);
}


#endif
