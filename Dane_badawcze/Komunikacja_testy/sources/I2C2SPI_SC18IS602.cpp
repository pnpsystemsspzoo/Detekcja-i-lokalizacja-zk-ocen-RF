#include "I2C2SPI_SC18IS602.h"


/**
 * 
 */
I2C2SPI_SC18IS602 :: I2C2SPI_SC18IS602()
{

}


/**
 * 
 */
I2C2SPI_SC18IS602 :: ~I2C2SPI_SC18IS602()
{
    
}


/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: Init(const char *i2c_dev_name, unsigned char i2c_addr)
{
    unsigned char result = 0;

    i2c_fd = open(i2c_dev_name, O_RDWR);
    if(i2c_fd <= 0)
    {
        printf("\nCannot open %s device!", i2c_dev_name);
    }

    if(ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) 
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: ResetDevice(void)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: GPIOEnable(unsigned char pin)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: SSEnable(unsigned char pin)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */		
unsigned char I2C2SPI_SC18IS602 :: ReadBytes(unsigned char *buffer, unsigned char length)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: ReadByte(void)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: WriteBytes(unsigned char *buffer, unsigned char length)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: WriteRegister(unsigned char reg_addr, unsigned char val)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: begin()
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: begin(unsigned char ss_pin)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: end()
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: setBitOrder(unsigned char order) //LSBFIRST MSBFIRST
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: setClockDivider(unsigned char divider)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: setDataMode(unsigned char mode)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: transfer(unsigned char value)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: transfer(unsigned char *buffer, unsigned char length)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: pinMode(unsigned char pin, unsigned char io)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: digitalWrite(unsigned char pin, unsigned char value)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: digitalRead(unsigned char pin)
{
    unsigned char result = 0;
    return result;
}

/**
 * 
 */
unsigned char I2C2SPI_SC18IS602 :: InterruptClear(void)
{
    unsigned char result = 0;
    return result;
}


