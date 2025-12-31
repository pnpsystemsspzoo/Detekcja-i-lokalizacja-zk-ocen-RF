#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        //Needed for I2C port
#include <fcntl.h>          //Needed for I2C port
#include <sys/ioctl.h>      //Needed for I2C port
#include <linux/i2c-dev.h>  //Needed for I2C port
#include <linux/i2c.h>      //Needed for I2C port


//Clock Divider
#define SC18IS602_CLOCK_1843K     (0X00)
#define SC18IS602_CLOCK_461K      (0X01)
#define SC18IS602_CLOCK_115K      (0X02)
#define SC18IS602_CLOCK_58K       (0X03)
#define SC18IS602_SPI_MODE0       (0x00)
#define SC18IS602_SPI_MODE1       (0x01)
#define SC18IS602_SPI_MODE2       (0x02)
#define SC18IS602_SPI_MODE3       (0x03)
#define SC18IS602_SS_0   		  (0X00)
#define SC18IS602_SS_1  		  (0X01)
#define SC18IS602_SS_2   		  (0X02)
#define SC18IS602_SS_3   		  (0X03)


class I2C2SPI_SC18IS602
{
    private:
    int i2c_fd;
    unsigned char i2caddr;

    unsigned char device_address;
	unsigned char ss_pin;
	unsigned char reg_f0_config; //configuration register 0xF0
	unsigned char reg_f4_gpio_w; //gpio write register
	unsigned char reg_f5_gpio_r; //gpio read register
	unsigned char reg_f6_gpio_e; //gpio enable register
	unsigned char reg_f7_gpio_d; //gpio direction
	unsigned char ResetDevice(void);
	unsigned char GPIOEnable(unsigned char pin);
	unsigned char SSEnable(unsigned char pin);
			
	unsigned char ReadBytes(unsigned char *buffer, unsigned char length);
	unsigned char ReadByte(void);
	unsigned char WriteBytes(unsigned char *buffer, unsigned char length);
    unsigned char WriteRegister(unsigned char reg_addr, unsigned char val);


    public:
    I2C2SPI_SC18IS602();
    ~I2C2SPI_SC18IS602();
    unsigned char Init(const char *i2c_dev_name, unsigned char i2c_addr);

    unsigned char begin();
	unsigned char begin(unsigned char ss_pin);
	unsigned char end();
	unsigned char setBitOrder(unsigned char order); //LSBFIRST MSBFIRST
	unsigned char setClockDivider(unsigned char divider);
	unsigned char setDataMode(unsigned char mode);
	unsigned char transfer(unsigned char value);
	unsigned char transfer(unsigned char *buffer, unsigned char length);
    unsigned char pinMode(unsigned char pin, unsigned char io);
    unsigned char digitalWrite(unsigned char pin, unsigned char value);
    unsigned char digitalRead(unsigned char pin);
	unsigned char InterruptClear(void);
};

