#ifndef _CLK_LMK04610_H
#define _CLK_LMK04610_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include "SPIcommunicator.h"
#include "GPIO_EXP_MCP23017.h"

#define LMK04610_CONFIG_REGS_NUM      240

class CLKdist_LMK04610
{
    private:
    typedef struct
    {
        std :: string name;
        unsigned short int addr;
        unsigned char val_w;
        unsigned char val_r;
    }LMK04610configRegs;

    unsigned char spi_ss_pin;
    SPIcommunicator *spi_communicator;

    LMK04610configRegs CLKdist_LMK04610ConfigRegs[LMK04610_CONFIG_REGS_NUM];
    
    public:
    CLKdist_LMK04610();
    ~CLKdist_LMK04610();
    unsigned short int Init(SPIcommunicator *spi);
    unsigned char SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max);
    unsigned short int WriteRegister(unsigned short int reg, unsigned char val);
    unsigned short int ReadRegister(unsigned short int reg, unsigned char *val);
    unsigned short int ReadData(void);
};

#endif

