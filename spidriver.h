#ifndef _SPIDRIVER_
#define _SPIDRIVER_

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>

#include <sched.h>		// To set the priority on linux

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

// Define which Raspberry Pi board we are using
//#define RPI
#define RPI2

#ifdef RPI
#define BCM2708_PERI_BASE       0x20000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)	
#endif

#ifdef RPI2
#define BCM2708_PERI_BASE       0x3F000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)	
#endif

// GPIO pins
#define MOSI 10
#define MISO 9
#define SCLK 11
#define CS   8
#define TEST 4

#define PAGE_SIZE 		(4*1024)
#define BLOCK_SIZE 		(4*1024)

#define NANO_TO_MILLISEC  1000000

// IO Acces
struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) 	*(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3)) // Set PIN g as Input
#define OUT_GPIO(g) 	*(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3)) // Set PIN g as Output
#define SET_GPIO_ALT(g,a) *(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3)) // Set PIN g to its alternative function(a=0~6)
// i.e. PIN19: Low PCM_FS SD11 <reserved> BSCSL SCL/SCLK SPI1_MISO PWM1


#define GPIO_SET 	*(gpio.addr + 7)  
#define GPIO_CLR 	*(gpio.addr + 10) 

#define GPIO_READ(g) 	*(gpio.addr + 13) &= (1<<(g))

// Function prototypes
int map_peripheral(struct bcm2835_peripheral *p);
void unmap_peripheral(struct bcm2835_peripheral *p);

// Priority
int SetProgramPriority(int priorityLevel);

// Implementation
struct bcm2835_peripheral gpio = {GPIO_BASE};

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2835_peripheral *p)
{
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }

   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,  // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p      // Address in physical map that we want this memory block to expose
   );

   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }

   p->addr = (volatile unsigned int *)p->map;

   return 0;
}

void unmap_peripheral(struct bcm2835_peripheral *p) {

    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}

int SetProgramPriority(int priorityLevel)
{
    struct sched_param sched;

    memset (&sched, 0, sizeof(sched));

    if (priorityLevel > sched_get_priority_max (SCHED_RR))
        priorityLevel = sched_get_priority_max (SCHED_RR);

    sched.sched_priority = priorityLevel;

    return sched_setscheduler (0, SCHED_RR, &sched);
}


#endif
