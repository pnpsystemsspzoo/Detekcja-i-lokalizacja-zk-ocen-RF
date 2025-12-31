#ifndef _ADC_LTC2122_H
#define _ADC_LTC2122_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include "SPIcommunicator.h"
#include "GPIO_EXP_MCP23017.h"

#define LTC2122_CONFIG_REGS_NUM      12

class ADC_LTC2122
{
    private:
    typedef struct
    {
        std :: string name;
        unsigned char addr;
        unsigned char val_w;
        unsigned char val_r;
        unsigned char rw;
    }ADCltc2122configRegs;
    
    unsigned char spi_ss_pin;
    SPIcommunicator *spi_communicator;

    ADCltc2122configRegs ADC_LTC2122ConfigRegs[LTC2122_CONFIG_REGS_NUM];
    
    public:
    ADC_LTC2122();
    ~ADC_LTC2122();
    unsigned char Init(SPIcommunicator *spi);
    unsigned char SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max);
    unsigned char WriteRegister(unsigned char reg, unsigned char val);
    unsigned char ReadRegister(unsigned char reg, unsigned char *val);
    unsigned char ReadData(void);
};

#endif
