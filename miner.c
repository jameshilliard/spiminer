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

void createChannel() {
        BYTE conf1[3] = {OUT0,OUT1,BF16};
        printf("Chain length:%d\n",_CHAIN_LEN);
        printf("BitArray size:%d\n",_ARRAY_SIZE);
        printf("Test1:\n");
        ClearChannelSeq();
        SetChannelSeq(conf1,3);
        DumpChannelSeq();
}

int main(int argc, char *argv[]) {
        int debug = 0;
        int option;
        char *method=NULL;

        while ((option=getopt(argc,argv,"dm:"))!=-1) {
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
        log_info("miner","calling method %s",method);
        if (strcmp(method,"createChannel")==0) {
                createChannel();
        } else {
                log_warn("miner","no such method");
        }
        log_warn("miner", "exiting...");
        return EXIT_SUCCESS;
}
