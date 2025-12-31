#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "ADC_LTC2122.h"
#include "ADC_AD9690.h"
#include "CLKdist_LMK04610.h"
#include "PowerMeg_INA226.h"
#include "GNSS_MAX2771.h"
#include "I2C_SWD_PCA9546.h"
#include "I2C2SPI_SC18IS602.h"
#include "GPIO_EXP_MCP23017.h"


class RadioCard
{
    private:
    unsigned char INA226addr;
    unsigned char PCA9546addr;
    unsigned char SC18IS602addr;
    unsigned char MCP23017addr[7];

    ADC_LTC2122 adc1;
    ADC_AD9690 adc2;
    CLKdist_LMK04610 clkdist;
    PowerMeg_INA226 ina226;
    GNSS_MAX2771 max2771;
    I2C_SWD_PCA9546 i2cSWD;
    I2C2SPI_SC18IS602 i2c2spi;
    GPIO_EXP_MCP23017 gpioEXP[7];

    public:
    RadioCard();
    ~RadioCard();
    unsigned char Init(const char *i2c_dev_name);
    unsigned char TestInternalDevices(void);
    unsigned char ReadData(void);
};

