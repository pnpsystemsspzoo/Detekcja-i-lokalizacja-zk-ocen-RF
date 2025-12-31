#ifndef _RADIO_CARD_H
#define _RADIO_CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fstream>
#include <sstream>
#include <string>

#include "SPIcommunicator.h"
#include "I2C_SWD_PCA9546.h"
#include "ADC_LTC2122.h"
#include "ADC_AD9690.h"
#include "CLKdist_LMK04610.h"
#include "PowerMeg_INA226.h"
#include "GNSS_MAX2771.h"
#include "GPIO_EXP_MCP23017.h"

#define GPIO_EXP_NUMBER    10

class RadioCard
{
    private:

    public:
    SPIcommunicator spi_communicator;
    I2C_SWD_PCA9546 i2cSWD;
    ADC_AD9690 adc1_AD9690;
    ADC_LTC2122 adc2_LTC2122;
    GNSS_MAX2771 adc3_MAX2771;
    CLKdist_LMK04610 clkdist_LMK04610;
    PowerMeg_INA226 ina226;
    GPIO_EXP_MCP23017 gpioEXP_MCP23017[GPIO_EXP_NUMBER];
    unsigned int gpioEXPnumber;
    
    unsigned char INA226addr;
    unsigned char PCA9546addr;

    RadioCard();
    ~RadioCard();
    unsigned char Init(void);
    int SetConfiguration(const char* configFilePath);
    unsigned char ReadData(void);
};

#endif

