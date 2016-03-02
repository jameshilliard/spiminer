#include "../spidriver.h"

int main() {
  //  SetProgramPriority();
  
  struct timespec sleepValue;
  sleepValue.tv_sec = 0;
  sleepValue.tv_nsec = 1L;

  // Never swap this process
  /* struct sched_param sp; */
  /* memset(&sp, 0, sizeof(sp)); */
  /* sp.sched_priority = sched_get_priority_max(SCHED_FIFO); */
  /* sched_setscheduler(0, SCHED_FIFO, &sp); */
  /* mlockall(MCL_CURRENT | MCL_FUTURE); */
  
  if(map_peripheral(&gpio) == -1)
    {
      printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
      return -1;
    }

  // GPIO 4 = RPI Pin 7
  INP_GPIO(4);
  OUT_GPIO(4);

  while(1)
    {
      GPIO_SET = 1 << 4;
      nanosleep(&sleepValue,NULL);
      

      GPIO_CLR = 1 << 4;
      nanosleep(&sleepValue,NULL);

    }

  return 0;
}
