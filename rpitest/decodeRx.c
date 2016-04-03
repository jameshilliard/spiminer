#include "../utils.h"

void main() {
  BYTE rx[] = { 0b00001010,0b01111101,0b10000001};
  BYTE output[3];

  decodeRx(output,rx,3);

  hexdump(rx,3);
  hexdump(output,3);
  
}
