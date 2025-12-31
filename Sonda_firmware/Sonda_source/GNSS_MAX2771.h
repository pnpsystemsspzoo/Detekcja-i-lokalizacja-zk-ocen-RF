#ifndef _GNSS_MAX2771_H
#define _GNSS_MAX2771_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include "SPIcommunicator.h"
#include "GPIO_EXP_MCP23017.h"

#define MAX2771_CONFIG_REGS_NUM      12

class GNSS_MAX2771
{
    private:
    typedef struct
    {
        std :: string name;
        unsigned char addr;
        unsigned int val_w;
        unsigned int val_r;
        unsigned char rw;
    }GNSSmax2771configRegs;

    unsigned char spi_ss_pin;
    SPIcommunicator *spi_communicator;

    GNSSmax2771configRegs GNSS_MAX2771ConfigRegs[MAX2771_CONFIG_REGS_NUM];
    
    public:
    GNSS_MAX2771();
    ~GNSS_MAX2771();
    unsigned char Init(SPIcommunicator *spi);
    unsigned char SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max);
    unsigned char WriteRegister(unsigned char reg, unsigned int val);
    unsigned char ReadRegister(unsigned char reg, unsigned int *val);
    unsigned char ReadData(void);
};

#endif

