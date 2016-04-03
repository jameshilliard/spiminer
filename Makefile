CC = gcc
CFLAGS = -Wall
#CFLAGS += -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
LFLAGS = -lbcm2835

all: miner

miner: miner.c miner.h utils.h bf250.h log.h log.c sha2.h sha2.c
	$(CC) $(CFLAGS) -o miner miner.c log.c sha2.c $(LFLAGS)

clean:
	rm -f *.o *~ miner
