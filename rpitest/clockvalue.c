#include "../utils.h"
#include "../bf250.h"

int main(){
  DWORD val = GetClockValue(0,0x1F);
  BYTE* ptr = (BYTE*)&val;
  hexdump(ptr,4);
  return 1;
}
