#include "../bf250.h"
#include <stdio.h>

int main() {
  if(bcm2835_init() == -1)
    {
      printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
      return -1;
    }
  
  while(1) {
    ResetSeq(5);
  }
  return 0;
}
