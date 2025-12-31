#include "RadioCard.h"


RadioCard :: RadioCard()
{
    
}

RadioCard :: ~RadioCard()
{

}

unsigned char RadioCard :: Init(const char *i2c_dev_name)
{
    unsigned char result = 0;
    unsigned char internal_result = 0;

    INA226addr = 0x01;
    PCA9546addr = 0x76;
    SC18IS602addr = 0x28;
    MCP23017addr[0] = 0x27;
    MCP23017addr[1] = 0x20;
    MCP23017addr[2] = 0x21;
    MCP23017addr[3] = 0x22;
    MCP23017addr[4] = 0x23;
    MCP23017addr[5] = 0x24;
    MCP23017addr[6] = 0x25;


    // printf("\nDevice %s opened successfully", i2c_dev_name);

    // if(internal_result = ina226.Init(i2c_fd, INA226addr) > 0)
    // {
    //     result = 0x02;
    //     printf("\nINA226 could not be intialized! Error code: %d", internal_result);
    // }
    // else
    // {
    //     printf("\nINA226 intialized");
    // }


    if(internal_result = gpioEXP[0].Init(i2c_dev_name, MCP23017addr[0]) > 0)
    {
        result = 0x05;
        printf("\nMCP23017 nr 0 could not be intialized! Error code: %d", internal_result);
    }
    else
    {
        printf("\nMCP23017 nr 0 intialized");
    }
    

    if(internal_result = i2cSWD.Init(i2c_dev_name, PCA9546addr) > 0)
    {
        result = 0x03;
        printf("\nPCA9546 could not be intialized! Error code: %d", internal_result);
    }
    else
    {
        printf("\nPCA9546 intialized");

        i2cSWD.EnableChannel(CHANNEL0);
        i2cSWD.EnableChannel(CHANNEL2);

        if(internal_result = i2c2spi.Init(i2c_dev_name, SC18IS602addr) > 0)
        {
            result = 0x04;
            printf("\nSC18IS602 could not be intialized! Error code: %d", internal_result);
        }
        else
        {
            printf("\nSC18IS602 intialized");
        }

        // if(internal_result = gpioEXP[1].Init(i2c_fd, MCP23017addr[1]) > 0)
        // {
        //     result = 0x06;
        //     printf("\nMCP23017 nr 1 could not be intialized! Error code: %d", internal_result);
        // }
        // else
        // {
        //     printf("\nMCP23017 nr 1 intialized");
        // }

        // if(internal_result = gpioEXP[2].Init(i2c_fd, MCP23017addr[2]) > 0)
        // {
        //     result = 0x07;
        //     printf("\nMCP23017 nr 2 could not be intialized! Error code: %d", internal_result);
        // }
        // else
        // {
        //     printf("\nMCP23017 nr 2 intialized");
        // }

        // if(internal_result = gpioEXP[3].Init(i2c_fd, MCP23017addr[3]) > 0)
        // {
        //     result = 0x08;
        //     printf("\nMCP23017 nr 3 could not be intialized! Error code: %d", internal_result);
        // }
        // else
        // {
        //     printf("\nMCP23017 nr 3 intialized");
        // }

        // if(internal_result = gpioEXP[4].Init(i2c_fd, MCP23017addr[4]) > 0)
        // {
        //     result = 0x09;
        //     printf("\nMCP23017 nr 4 could not be intialized! Error code: %d", internal_result);
        // }
        // else
        // {
        //     printf("\nMCP23017 nr 4 intialized");
        // }

        if(internal_result = gpioEXP[5].Init(i2c_dev_name, MCP23017addr[5]) > 0)
        {
            result = 0x0A;
            printf("\nMCP23017 nr 5 could not be intialized! Error code: %d", internal_result);
        }
        else
        {
            printf("\nMCP23017 nr 5 intialized");
        }

        // if(internal_result = gpioEXP[6].Init(i2c_fd, MCP23017addr[6]) > 0)
        // {
        //     result = 0x0B;
        //     printf("\nMCP23017 nr 6 could not be intialized! Error code: %d", internal_result);
        // }
        // else
        // {
        //     printf("\nMCP23017 nr 6 intialized");
        // }
    }


    
    return result;
}

unsigned char RadioCard :: TestInternalDevices(void)
{
    unsigned char result = 0;
    unsigned short int regValin = 0;
    unsigned short int regValout = 0;

    //gpioEXP[0].SelfTest();
    //i2cSWD.SelfTest();

    gpioEXP[0].pinMode(1, OUTPUT);
    regValin = 0x0002;
	gpioEXP[0].writeGPIOAB(regValin);
    gpioEXP[0].readGPIOAB(&regValout);
	printf("\nReg Val out 27:  %d\n", regValout);


    gpioEXP[5].pinMode(0, OUTPUT);
    gpioEXP[5].pinMode(1, OUTPUT);
    gpioEXP[5].pinMode(4, OUTPUT);
    gpioEXP[5].pinMode(8, OUTPUT);
    gpioEXP[5].pinMode(10, OUTPUT);
    regValin = 0x0111;
	gpioEXP[5].writeGPIOAB(regValin);
    gpioEXP[5].readGPIOAB(&regValout);
	printf("\nReg Val out 24:  %d\n", regValout);
    
    return result;
}

unsigned char RadioCard :: ReadData(void)
{
    unsigned char result = 0;
    return result;
}

