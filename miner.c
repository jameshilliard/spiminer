#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "miner.h"
#include "bf250.h"
#include "sha2.h"
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
BYTE decRx[BUFSIZE];

#define TASKBUFSIZE 96
char TaskTxBuf[TASKBUFSIZE];
char TaskRxBuf[TASKBUFSIZE];
BYTE TaskDecRx[BUFSIZE];

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
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
        bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);
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
/*
BYTE TestTask[80] = {
	0xa6,0x07,0xd6,0x7b,0x61,0x49,0x25,0x73,0x7b,0xe7,0x6b,0xce,0x53,0xa4,0x1b,0xa1,
	0x2b,0x3c,0x8b,0x65,0x9f,0x27,0xef,0x67,0x26,0xbe,0x60,0x49,0xf9,0x24,0x52,0x2d,
	0x2b,0x3c,0x8b,0x65,0x9c,0x78,0x61,0x6c,0x77,0x9a,0x13,0xc8,0x1a,0x60,0x93,0x50,
	0x67,0x95,0x33,0x86,0xa9,0xd5,0x2b,0x3d,0x0f,0x24,0xa3,0xb0,0xa6,0x07,0xd6,0x7b,
	0xb2,0xa8,0xcc,0x53,0x26,0x70,0x8e,0x0e,0xf5,0x5b,0x26,0x77,0x00,0x00,0x00,0x00,
};*/

BYTE TestTask[80] ={
0xa6, 0x07, 0xd6, 0x7b, 0x61, 0x49, 0x25, 0x73, 
0x7b, 0xe7, 0x6b, 0xce, 0x53, 0xa4, 0x1b, 0xa1, 
0x2b, 0x3c, 0x8b, 0x65, 0x9f, 0x27, 0xef, 0x67, 
0x26, 0xbe, 0x60, 0x49, 0xf9, 0x24, 0x52, 0x2d, 
0x2b, 0x3c, 0x8b, 0x65, 0x9c, 0x78, 0x61, 0x6c, 
0x77, 0x9a, 0x60, 0x01, 0x79, 0x6a, 0x5d, 0x98, 
0x67, 0x95, 0x33, 0x86, 0xa9, 0xd5, 0x2b, 0x3d, 
0x0f, 0x24, 0xa3, 0xb0, 0xa6, 0x07, 0xd6, 0x7b, 
0xb2, 0xa8, 0xcc, 0x53, 0x26, 0x70, 0x9f, 0x47, 
0x12, 0x5f, 0x93, 0x41, 0x00, 0x00, 0x00, 0x00 
};

void Task_Send_and_Receive(BYTE ChipAddr)
{
	BYTE checksum;
	int len;
	memset(TaskTxBuf,0,TASKBUFSIZE);
	memset(TaskRxBuf,0,TASKBUFSIZE);
  	printf("-------- CMD_Taskwrite --------\n");
	
	TaskTxBuf[0]=0x01;// start bit
	TaskTxBuf[1]=ChipAddr<<4;// address
	TaskTxBuf[2] = CMD_Taskwrite;
	TaskTxBuf[3] = 0x4f;
	
	memcpy(TaskTxBuf+4,TestTask,80);
	len = 84;
	checksum = chksum(TaskTxBuf+2,len-2);
	
	bcm2835_spi_transfernb(TaskTxBuf,TaskRxBuf,TASKBUFSIZE);
	
	log_info("miner","sent");
	hexdump(TaskTxBuf,TASKBUFSIZE);
	log_info("miner","recv");
	hexdump(TaskRxBuf,TASKBUFSIZE);
	
	printf("Checksum: %02x\n",checksum);
	decodeRx(TaskDecRx,(BYTE*)TaskRxBuf+len,TASKBUFSIZE-len);
	//hexdump(TaskDecRx,TASKBUFSIZE);
	memset(TaskDecRx,0,TASKBUFSIZE);
	printf("decode2\n");
	decodeRx2(TaskDecRx,(BYTE*)TaskRxBuf+len,TASKBUFSIZE-len);
	hexdump(TaskDecRx,TASKBUFSIZE);
	bcm2835_delay(10);
}

void Nonce_Send_and_Receive(BYTE ChipAddr)
{
	BYTE checksum;
	int len;
	memset(TaskTxBuf,0,TASKBUFSIZE);
	memset(TaskRxBuf,0,TASKBUFSIZE);
  	printf("-------- CMD_ReadNonce --------\n");
	
	TaskTxBuf[0]=0x01;// start bit
	TaskTxBuf[1]=ChipAddr<<4;// address
	TaskTxBuf[2] = CMD_ReadNonce;
	TaskTxBuf[3] = 0x00;
	TaskTxBuf[4] = 0x00;
	len = 5;
	checksum = chksum(TaskTxBuf+2,len-2);
	
	bcm2835_spi_transfernb(TaskTxBuf,TaskRxBuf,TASKBUFSIZE);
	
	log_info("miner","sent");
	hexdump(TaskTxBuf,TASKBUFSIZE);
	log_info("miner","recv");
	hexdump(TaskRxBuf,TASKBUFSIZE);
	
	printf("Checksum: %02x\n",checksum);
	decodeRx(TaskDecRx,(BYTE*)TaskRxBuf+len,TASKBUFSIZE-len);
	//hexdump(TaskDecRx,TASKBUFSIZE);
	memset(TaskDecRx,0,TASKBUFSIZE);
	printf("decode2\n");
	decodeRx2(TaskDecRx,(BYTE*)TaskRxBuf+len,TASKBUFSIZE-len);
	hexdump(TaskDecRx,TASKBUFSIZE);
	bcm2835_delay(10);
}


void Cmd_Send_and_Receive(BYTE ChipAddr,BYTE cmd)
{
	BYTE checksum;
	int len;
	memset(txbuf,0,BUFSIZE);
	memset(rxbuf,0,BUFSIZE);
  
	txbuf[0]=0x01;// start bit
	txbuf[1]=ChipAddr<<4;// address
	txbuf[2] = cmd;
	switch(cmd)
	{
		case CMD_Taskwrite:
			printf("-------- Error -------- \n");
			break;
		case CMD_Switch:
			printf("-------- CMD_Switch -------- \n");
			txbuf[3] = 0x00;
			txbuf[4] = 0x00;
			len = 5;
			break;
		case CMD_ReadNonce:
			printf("-------- Error -------- \n");
			break;
		case CMD_SetClock:
			{
			DWORD clock;
			printf("-------- CMD_SetClock -------- \n");
			txbuf[3] = 0x03;
			clock = GetClockValue(0,0x32);
			memcpy(txbuf+4,(BYTE*)&clock,4);
			len = 8;
			}
			break;
		case CMD_Toggle:
			printf("-------- Error --------\n");
			break;
		case CMD_SetMask:
			printf("-------- CMD_SetMask -------- \n");
			txbuf[3] = 0x03;
			txbuf[4] = 0x00;
			txbuf[5] = 0x00;
			txbuf[6] = 0x00;
			txbuf[7] = 0x00;
			len = 8;
			break;
	}
	
	checksum = chksum(txbuf+2,len-2);
	bcm2835_spi_transfernb(txbuf,rxbuf,BUFSIZE);
	dumpTxRx();
	printf("Checksum: %02x\n",checksum);
	decodeRx(decRx,(BYTE*)rxbuf+len,BUFSIZE-len);
	//hexdump(decRx,BUFSIZE);
	memset(decRx,0,BUFSIZE);
	printf("decode2\n");
	decodeRx2(decRx,(BYTE*)rxbuf+len,BUFSIZE-len);
	hexdump(decRx,BUFSIZE);
	bcm2835_delay(10);
}

void test() {
  BYTE conf[3] = {OUT1,BF16};
   BYTE addr =1;
	
	//while(1)
	{
		//	  log_info("miner","reset bf250");
		ResetSeq(6);

		log_info("miner","###################### start #####################");
		ClearChannelSeq();
		SetChannelSeq(conf,2);
		// DumpChannelSeq();

		memset(txbuf,0,BUFSIZE);
		memset(rxbuf,0,BUFSIZE);
		memcpy(txbuf,bitarray,_ARRAY_SIZE);
		// more zeros after chan conf sequence
		bcm2835_spi_transfernb(txbuf,rxbuf,_ARRAY_SIZE+1);
		// dumpTxRx();

		Cmd_Send_and_Receive(addr,CMD_SetClock);
		Cmd_Send_and_Receive(addr,CMD_SetMask);
		Cmd_Send_and_Receive(addr,CMD_Switch);
		Task_Send_and_Receive(addr);
		Cmd_Send_and_Receive(addr,CMD_Switch);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
		bcm2835_delay(10);
		Nonce_Send_and_Receive(addr);
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
