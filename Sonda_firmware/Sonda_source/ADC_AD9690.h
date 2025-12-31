#ifndef _ADC_AD9690_H
#define _ADC_AD9690_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include "SPIcommunicator.h"
#include "GPIO_EXP_MCP23017.h"

#define AD9690_CONFIG_REGS_NUM      140

class ADC_AD9690
{
    private:
    typedef struct
    {
        std :: string name;
        unsigned short int addr;
        unsigned char val_w;
        unsigned char val_r;
        unsigned char rw;
    }ADCad9690configRegs;
    
    unsigned char spi_ss_pin;
    SPIcommunicator *spi_communicator;

    ADCad9690configRegs ADC_AD9690ConfigRegs[AD9690_CONFIG_REGS_NUM];
    
    public:
    ADC_AD9690();
    ~ADC_AD9690();
    unsigned short int Init(SPIcommunicator *spi);
    unsigned char SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max);
    unsigned short int WriteRegister(unsigned short int reg, unsigned char val);
    unsigned short int ReadRegister(unsigned short int reg, unsigned char *val);
    unsigned short int ReadData(void);
};

#endif

