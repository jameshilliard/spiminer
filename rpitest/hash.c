// gcc hash.c ../sha2.c

#include "../utils.h"
#include "../sha2.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  BYTE headerHex[] = "000000020f60ba73a67140bb5e1272d809fe7b926739feb84137e1ee0000000300000000dd33336ac4fc668f930a09b2cd8317e9cec5b7c39648f787bbe02ee49544edde52908b9919070bfbde030000";
  // 7092902a3d787611c6e1cfb95971eb9d5aeebcb7e47a275836a5c5bad35a8369
  
  DWORD header[20]={0};
  DWORD hash[8]={0};
  DWORD hash1[8]={0};
  BYTE midstate[32]={0};
  int i;
  BYTE *header2 = (BYTE*)&header;

  for(i=0;i<80;i++) {
    header2[i] = hex2bin(headerHex+i*2);
  }
  hexdump((BYTE *)header,80);
  printf("============\n");

  calc_midstate(midstate,header2);
  hexdump((BYTE *)midstate,32);


  printf("ms3 test\n");

/* Wi 0=CD3F992C 1=037F8197 2=A58E091A */
/* MS0 A=0CAD7CD1 B=CBE38FD9 C=D14DC164 D=F90EB10B E=819621CF F=358D45CD G=8C14CAE3 H=538EF887 */
/* MS3 A=5FF18CDD B=8CDA24A4 C=180266F9 D=0CAD7CD1 E=B0CA39FA F=DD30B962 G=36D2CBC6 H=819621CF */


  BYTE sendStr[] = "a607d67b614925737be76bce53a41ba12b3c8b659f27ef6726be6049f924522d2b3c8b659c78616c779a13c81a60935067953386a9d52b3d0f24a3b0a607d67bb2a8cc5326708e0ef55b267700000000";
  BYTE sendBin[76];

  printf("send:\n");
  for(i=0;i<76;i++) sendBin[i]=hex2bin(sendStr+i*2);
  hexdump(sendBin,76);
  printf("decoded:\n");
  for(i=0;i<76;i++) sendBin[i]=sendBin[i]^0xAA;
  hexdump(sendBin,76);

  
  printf("ms3:\n");
  BYTE job[76]={0};
  BYTE newJob[76]={0};
  DWORD mid[8] = {0x0CAD7CD1,0xCBE38FD9,0xD14DC164,0xF90EB10B,0x819621CF,0x358D45CD,0x8C14CAE3,0x538EF887};
  DWORD w[3] = {0xCD3F992C,0x037F8197,0xA58E091A};
  BYTE buf[4096];

  memcpy(job,(BYTE*)&mid,32);
  memcpy(job+48,(BYTE*)&w,12);
  ms3_compute(job);

  flipN(newJob,job,76);
  hexdump(newJob,76);
  bin2c(buf,newJob,76);
  printf("%s\n",buf);
  
  return 0;
}

