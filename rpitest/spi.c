// Must disable rpi spi module, and enable device tree
#include <bcm2835.h>
#include <stdio.h>
#include <string.h>

int main() {
        char txbuf[16],rxbuf[16];
        char t,r;

        if(!bcm2835_init()) {
                printf("bcm2835_init failed. Are you running as root?");
                return 1;
        }
        if(!bcm2835_spi_begin()) {
                printf("bcm2835_spi_begin failed. Are you running as root?");
                return 1;
        }

        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
        bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
        bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0,LOW);

        strcpy(txbuf,"helloworld");
        bcm2835_spi_transfernb(txbuf,rxbuf,16);

        printf("Sent: %s\n",txbuf);
        printf("Recv: %s\n",rxbuf);

        t='h';
        r=bcm2835_spi_transfer(t);
        printf("Sent: %d\n",t);
        printf("Recv: %d\n",r);


        bcm2835_spi_end();
        bcm2835_close();

        return 0;
}
