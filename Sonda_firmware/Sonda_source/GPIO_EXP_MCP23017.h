#ifndef _GPIO_EXP_MCP23017_H
#define _GPIO_EXP_MCP23017_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>        //Needed for I2C port
#include <fcntl.h>          //Needed for I2C port
#include <sys/ioctl.h>      //Needed for I2C port
#include <linux/i2c-dev.h>  //Needed for I2C port
#include <linux/i2c.h>      //Needed for I2C port


#define MCP23017_ADDRESS    0x20

#define MCP23017_IODIRA     0x00
#define MCP23017_IPOLA      0x02
#define MCP23017_GPINTENA   0x04
#define MCP23017_DEFVALA    0x06
#define MCP23017_INTCONA    0x08
#define MCP23017_IOCONA     0x0A
#define MCP23017_GPPUA      0x0C
#define MCP23017_INTFA      0x0E
#define MCP23017_INTCAPA    0x10
#define MCP23017_GPIOA      0x12
#define MCP23017_OLATA      0x14

#define MCP23017_IODIRB     0x01
#define MCP23017_IPOLB      0x03
#define MCP23017_GPINTENB   0x05
#define MCP23017_DEFVALB    0x07
#define MCP23017_INTCONB    0x09
#define MCP23017_IOCONB     0x0B
#define MCP23017_GPPUB      0x0D
#define MCP23017_INTFB      0x0F
#define MCP23017_INTCAPB    0x11
#define MCP23017_GPIOB      0x13
#define MCP23017_OLATB      0x15

#define MCP23017_INT_ERR    255

#define INPUT               1
#define OUTPUT              0
#define HIGH                1
#define LOW                 0



class GPIO_EXP_MCP23017
{
    private:
    typedef struct
    {
        std :: string i2c_num;
        unsigned char i2c_addr;
        unsigned short int pinsModeWrote;
        unsigned short int pinsModeRed;
        unsigned short int pinsPullUpModeWrote;
        unsigned short int pinsPullUpModeRed;
        unsigned short int pinsVarWrote;
        unsigned short int pinsVarRed;
    }GpioEXPconfigRegs;

    int i2c_fd;

    unsigned char bitForPin(unsigned char pin);
    unsigned char bitWrite(unsigned char *reg, unsigned char bit, unsigned char val);
    unsigned char bitRead(unsigned char *reg, unsigned char bit, unsigned char *val);
    unsigned char regForPin(unsigned char pin, unsigned char portAaddr, unsigned char portBaddr);
    int writeRegisters(unsigned char reg, unsigned char *val, unsigned char size);
    int readRegisters(unsigned char reg, unsigned char *val, unsigned char size);
    int updateRegisterBit(unsigned char pin, unsigned char pValue, unsigned char portAaddr, unsigned char portBaddr);

    public:
    GpioEXPconfigRegs gpioEXPconfigRegs;

    GPIO_EXP_MCP23017();
    ~GPIO_EXP_MCP23017();
    unsigned char Init(const char *i2c_dev_name, unsigned char i2c_addr);
    int pinMode(unsigned char p, unsigned char d);
    int pinsMode(unsigned short int reg);
    int readPinsMode(unsigned short int* reg);
    unsigned char digitalWrite(unsigned char pin, unsigned char d);
    int pullUp(unsigned char p, unsigned char d);
    int pinsPullUpState(unsigned short int reg);
    int readPinsPullUpState(unsigned short int* reg);
    unsigned char digitalRead(unsigned char pin, unsigned char *val);
    int writeGPIOAB(unsigned short int ba);
    int readGPIOAB(unsigned short int *val);
    unsigned char readGPIO(unsigned char b, unsigned char *val);
    unsigned char setupInterrupts(unsigned char mirroring, unsigned char openDrain, unsigned char polarity);
    unsigned char setupInterruptPin(unsigned char pin, unsigned char mode);
    unsigned char getLastInterruptPin(unsigned char *val);
    unsigned char getLastInterruptPinValue(unsigned char *val);
};

#endif

