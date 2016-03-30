#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "miner.h"
#include "hash.h"
#include "bf250.h"
#define VERSION "1.0"

static void usage() {
        fprintf(stderr, "Usage: %s [OPTIONS]\n",
                "miner");
        fprintf(stderr, "Version: %s\n", VERSION);
        fprintf(stderr, "\n");
        fprintf(stderr, " -d, --debug        be more verbose.\n");
        fprintf(stderr, " -h, --help         display help and exit\n");
        fprintf(stderr, " -v, --version      print version and exit\n");
        fprintf(stderr, "\n");
}

#define BUFSIZE 16
char txbuf[BUFSIZE];
char rxbuf[BUFSIZE];

void dumpTxRx() {
        log_info("miner","sent");
        hexdump(txbuf,BUFSIZE);
        log_info("miner","recv");
        hexdump(rxbuf,BUFSIZE);
}

int init() {
        if(!bcm2835_init()) {
                log_crit("miner","bcm2835_init failed. Are you running as root?");
                return 0;
        }
        if(!bcm2835_spi_begin()) {
                log_crit("miner","bcm2835_spi_begin failed. Are you running as root?");
                return 0;
        }
        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
        bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
        bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0,LOW);
        return 1;
}


void createChannel() {
        BYTE conf1[3] = {OUT0,OUT1,BF16};
        printf("Chain length:%d\n",_CHAIN_LEN);
        printf("BitArray size:%d\n",_ARRAY_SIZE);
        printf("Test1:\n");
        ClearChannelSeq();
        SetChannelSeq(conf1,3);
        DumpChannelSeq();
}


void test() {
        BYTE conf[1] = {BF16};
        BYTE addr =0;

        while(1){
        log_info("miner","reset bf250");
        ResetSeq(6);

        log_info("miner","creating channel");
        ClearChannelSeq();
        SetChannelSeq(conf,1);
        DumpChannelSeq();
        memset(txbuf,0,BUFSIZE);
        memset(rxbuf,0,BUFSIZE);
        memcpy(txbuf,bitarray,_ARRAY_SIZE);
        bcm2835_spi_transfernb(txbuf,rxbuf,_ARRAY_SIZE);
        dumpTxRx();

        log_info("miner","force task switch");
        memset(txbuf,0,BUFSIZE);
        memset(rxbuf,0,BUFSIZE);
        txbuf[1]=1<<4|addr;
        txbuf[2]=0x04;
        bcm2835_spi_transfernb(txbuf,rxbuf,BUFSIZE);
        dumpTxRx();

        bcm2835_st_delay(0,1000);
        ResetSeq(6);

        bcm2835_st_delay(0,100000);
        addr++;
        if(addr>10) addr=0;
        }
}

int main(int argc, char *argv[]) {
        int debug = 0;
        int option;
        char *method=NULL;

        while ((option=getopt(argc,argv,"dm:t"))!=-1) {
                switch (option) {
                case 'd':
                        debug=1;
                        break;
                case 'm':
                        method=optarg;
                        break;
                default:
                        fprintf(stderr, "unknown option `%c'\n", option);
                        usage();
                        exit(1);
                }
        }

        log_init(7,"miner");
        log_info("miner", "miner started...");
        log_info("miner", "debug %s",debug?"on":"off");
        log_info("miner", "initing...");
        if(!init()) {
                return -1;
        }
        log_info("miner","calling method %s",method);
        if (strcmp(method,"createChannel")==0) {
                createChannel();
        } else if (strcmp(method,"test")==0) {
                test();
        } else {
                log_warn("miner","no such method");
        }
        log_warn("miner", "exiting...");
        bcm2835_close();
        return EXIT_SUCCESS;
}
