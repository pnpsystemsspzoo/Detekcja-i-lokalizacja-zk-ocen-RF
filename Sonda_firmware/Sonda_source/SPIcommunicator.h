#ifndef _SPICOMMUNICATOR_H
#define _SPICOMMUNICATOR_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define SS_PIN1 0
#define SS_PIN2 1
#define SS_PIN3 2
#define SS_PIN0 4

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

class SPIcommunicator
{
    private:
    int spi_fd;
    unsigned char spi_ss_pin;

    unsigned char mode = 0;
    unsigned char bits = 8;
    unsigned int speed = 50000;
    unsigned short int delay;

    public:
    SPIcommunicator();
    ~SPIcommunicator();
    int Init(const char *spi_dev_name);
    int TransferData(unsigned char* tx_buffer, unsigned char* rx_buffer, unsigned char bytes_to_transfer, unsigned char bytes_to_receive);
};

#endif
