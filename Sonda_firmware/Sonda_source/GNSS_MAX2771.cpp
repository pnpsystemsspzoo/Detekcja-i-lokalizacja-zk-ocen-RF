#include "GNSS_MAX2771.h"

using namespace std;

GNSS_MAX2771 :: GNSS_MAX2771()
{

}

GNSS_MAX2771 :: ~GNSS_MAX2771()
{

}

unsigned char GNSS_MAX2771 :: Init(SPIcommunicator *spi)
{
    unsigned char result = 0;
    spi_communicator = spi;
    spi_ss_pin = 0;
    
    return result;
}

unsigned char GNSS_MAX2771 :: SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max)
{
    unsigned char result = 0;
    unsigned short int pinsVarRead;
    unsigned char csExpIdx = 0;
    fstream configFile;
    string line;
    string tokens[4];
    unsigned int tempInt;
    unsigned short int configRegsIdx = 0;

    printf("\nLoad MAX2771 configuration file %s", configFilePath);
    configFile.open(configFilePath, ios::in);
    if(configFile.is_open())
    {
        printf("\nConfiguration file %s opened successfully", configFilePath);

        while(getline(configFile, line))
        {
            if(line.at(0) != '#')
            {
                int start = 0;
                int end = line.find(" ");
                tokens[0] = line.substr(start, end - start);
                GNSS_MAX2771ConfigRegs[configRegsIdx].name = tokens[0];
                
                start = line.find("0x");
                if(start > 0)
                {
                    tokens[1] = line.substr(start, 4);
                    tokens[2] = line.substr(start + 7, 10);
                    tokens[3] = line.substr(start + 7 + 13, 3);

                    if(sscanf(tokens[1].c_str(), "%x", &tempInt) == 1)
                    {
                        GNSS_MAX2771ConfigRegs[configRegsIdx].addr = (unsigned char)tempInt;     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    if(sscanf(tokens[2].c_str(), "%x", &tempInt) == 1)
                    {
                        GNSS_MAX2771ConfigRegs[configRegsIdx].val_w = tempInt;     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    if(sscanf(tokens[3].c_str(), "%x", &tempInt) == 1)
                    {
                        GNSS_MAX2771ConfigRegs[configRegsIdx].rw = (unsigned char)tempInt;     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    configRegsIdx++;
                    if(configRegsIdx >= MAX2771_CONFIG_REGS_NUM) configRegsIdx = MAX2771_CONFIG_REGS_NUM;
                }
            }
        }
    }
    else
    {
        printf("\n\033[0;31mCannot open Configuration file %s\033[0m", configFilePath);
    }
    configFile.close();
    printf("\n\n");

    //znajdowanie ekspandera o odpwiednim adresie
    for(int i = 0; i < cs_exp_max; i++)
    {
        if(csGPIOexp[i].gpioEXPconfigRegs.i2c_addr == cs_addr)
        {
            csExpIdx = i;
        }
    }

    unsigned short int val;
    unsigned char reg_val;
    //zapis danych z pliku konfiguracyjnego przetwornika
    for(int i = 0; i < configRegsIdx; i++)
    {
        if(GNSS_MAX2771ConfigRegs[i].rw == 0x0)
        {
            usleep(1000);
            csGPIOexp[csExpIdx].digitalWrite(cs_pin, 0);
            WriteRegister(GNSS_MAX2771ConfigRegs[i].addr, GNSS_MAX2771ConfigRegs[i].val_w);
            usleep(1000);
            csGPIOexp[csExpIdx].digitalWrite(cs_pin, 1);
        }   
    }

    //odczyt danych z pamięci przetwornika
    for(int i = 0; i < configRegsIdx; i++)
    {
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 0); 
        ReadRegister(GNSS_MAX2771ConfigRegs[i].addr, &GNSS_MAX2771ConfigRegs[i].val_r);
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 1); 
    }

    //sprawdzenie statusu konfiguracji
    for(int i = 0; i < configRegsIdx; i++)
    {
        if(GNSS_MAX2771ConfigRegs[i].val_w == GNSS_MAX2771ConfigRegs[i].val_r)
        {
            printf("\nREG  %d:  %s \r\t\t\t\t\t\t0x%x   0x%x   0x%x   0x%x", i, GNSS_MAX2771ConfigRegs[i].name.c_str(), GNSS_MAX2771ConfigRegs[i].addr, GNSS_MAX2771ConfigRegs[i].val_w, GNSS_MAX2771ConfigRegs[i].val_r, GNSS_MAX2771ConfigRegs[i].rw);
        }
        else
        {
            printf("\n\033[0;33mREG  %d:  %s \r\t\t\t\t\t\t0x%x   0x%x   0x%x   0x%x \033[0m", i, GNSS_MAX2771ConfigRegs[i].name.c_str(), GNSS_MAX2771ConfigRegs[i].addr, GNSS_MAX2771ConfigRegs[i].val_w, GNSS_MAX2771ConfigRegs[i].val_r, GNSS_MAX2771ConfigRegs[i].rw);
        }
        usleep(10000);
    }

    return result;
}

unsigned char GNSS_MAX2771 :: WriteRegister(unsigned char reg, unsigned int val)
{
    typedef union
    {
        unsigned char b[4];
        unsigned int v;
    }int2byte;
    int2byte i2b;
    
    unsigned char result = 0;
    unsigned char txBuf[6];
    unsigned char rxBuf[6];
    unsigned char temp_reg;

    temp_reg = (reg << 4);

    txBuf[0] = 0;
    txBuf[1] = temp_reg;
    i2b.v = val;
    txBuf[2] = i2b.b[3];
    txBuf[3] = i2b.b[2];
    txBuf[4] = i2b.b[1];
    txBuf[5] = i2b.b[0];

    spi_communicator->TransferData(txBuf, rxBuf, 6, 0);
    usleep(2000);

    return result;
}

unsigned char GNSS_MAX2771 :: ReadRegister(unsigned char reg, unsigned int *val)
{
    typedef union
    {
        unsigned char b[4];
        unsigned int v;
    }int2byte;
    int2byte i2b;

    unsigned char result = 0;
    unsigned char txBuf[6];
    unsigned char rxBuf[6];
    unsigned char temp_reg;

    temp_reg = (reg << 4) | 0x08;

    txBuf[0] = 0;
    txBuf[1] = temp_reg;
    txBuf[2] = 0;
    txBuf[3] = 0;
    txBuf[4] = 0;
    txBuf[5] = 0;

    spi_communicator->TransferData(txBuf, rxBuf, 6, 6);
    usleep(2000);

    i2b.b[0] = rxBuf[5];
    i2b.b[1] = rxBuf[4];
    i2b.b[2] = rxBuf[3];
    i2b.b[3] = rxBuf[2];

    *val = i2b.v;

    return result;
}

unsigned char GNSS_MAX2771 :: ReadData(void)
{
    unsigned char result = 0;
    return result;
}

