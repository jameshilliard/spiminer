#include "../bf250.h"

int main() {
  if(map_peripheral(&gpio) == -1)
    {
      printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
      return -1;
    }
  
  MaxProgramPriority();
  ResetSeq(5);
  return 0;
}
