#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        //Needed for I2C port
#include <fcntl.h>          //Needed for I2C port
#include <sys/ioctl.h>      //Needed for I2C port
#include <linux/i2c-dev.h>  //Needed for I2C port
#include <linux/i2c.h>      //Needed for I2C port


#define CHANNEL0        0x01
#define CHANNEL1        0x02
#define CHANNEL2        0x04
#define CHANNEL3        0x08


class I2C_SWD_PCA9546
{
    private:
    int i2c_fd;

    public:
    I2C_SWD_PCA9546();
    ~I2C_SWD_PCA9546();
    unsigned char Init(const char *i2c_dev_name, unsigned char i2c_addr);
    unsigned char EnableChannel(unsigned char ch);
    unsigned char DisableChannel(unsigned char ch);
    unsigned char DisableAllChannels(void);
    unsigned char GetChannelsState(unsigned char *state);
    unsigned char SelfTest(void);
};

