#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "miner.h"
#include "hash.h"
#include "log.h"

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

int main(int argc, char *argv[]) {
        int debug = 0;
        int option;
        char msg[80];

        while ((option=getopt(argc,argv,"d"))!=-1) {
                switch (option) {
                case 'd':
                        debug=1;
                        break;
                default:
                        fprintf(stderr, "unknown option `%c'\n", option);
                        usage();
                        exit(1);
                }
        }

        log_init(7,"miner");
        log_info("miner", "miner started...");
        sprintf(msg,"debug %s",debug?"on":"off");
        log_info("miner", msg);
        log_warn("miner", "exiting...");
        return EXIT_SUCCESS;
}
