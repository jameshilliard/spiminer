CC = gcc
CFLAGS = -Wall -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
LFLAGS = -lbcm2835

all: miner

miner: miner.c miner.h utils.h bf250.h log.h log.c hash.h
	$(CC) $(CFLAGS) -o miner miner.c log.c $(LFLAGS)

clean:
	rm *.o *~ miner
