#include "CLKdist_LMK04610.h"

using namespace std;

CLKdist_LMK04610 :: CLKdist_LMK04610()
{

}

CLKdist_LMK04610 :: ~CLKdist_LMK04610()
{

}

unsigned short int CLKdist_LMK04610 :: Init(SPIcommunicator *spi)
{
    unsigned short int result = 0;
    spi_communicator = spi;
    spi_ss_pin = 0;

    return result;
}

unsigned char CLKdist_LMK04610 :: SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max)
{
    unsigned char result = 0;
    unsigned short int pinsVarRead;
    unsigned char csExpIdx = 0;
    fstream configFile;
    string line;
    string tokens[2];
    unsigned int tempInt;
    unsigned short int configRegsIdx = 0;

    printf("\nLoad LMK04610 configuration file %s", configFilePath);
    configFile.open(configFilePath, ios::in);
    if(configFile.is_open())
    {
        printf("\nConfiguration file %s opened successfully", configFilePath);

        while(getline(configFile, line))
        {
            if(line.at(0) != '#')
            {
                int start = 0;
                int end = line.find("0x");

                if(end > 0)
                {
                    tokens[0] = line.substr(start, end - start - 1);
                    tokens[1] = line.substr(end, 8);

                    CLKdist_LMK04610ConfigRegs[configRegsIdx].name = tokens[0];

                    // tempInt = stoul(tokens[2], nullptr, 16);
                    // CLKdist_LMK04610ConfigRegs[configRegsIdx].val_w = tempInt;
                    // printf("\n%s   %s   %d", tokens[0].c_str(), tokens[2].c_str(), end);

                    if(sscanf(tokens[1].c_str(), "%x", &tempInt) == 1)
                    {
                        CLKdist_LMK04610ConfigRegs[configRegsIdx].addr = (unsigned short int)(tempInt >> 8);
                        CLKdist_LMK04610ConfigRegs[configRegsIdx].val_w = (unsigned char)(tempInt & 0x0000FF);     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    //printf("\n%s  %d  0x%x", CLKdist_LMK04610ConfigRegs[configRegsIdx].name.c_str(), CLKdist_LMK04610ConfigRegs[configRegsIdx].addr, CLKdist_LMK04610ConfigRegs[configRegsIdx].val_w);

                    configRegsIdx++;
                    if(configRegsIdx >= LMK04610_CONFIG_REGS_NUM) configRegsIdx = LMK04610_CONFIG_REGS_NUM;
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

    //zapis danych z pliku konfiguracyjnego przetwornika
    for(int i = 0; i < configRegsIdx; i++)
    {
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 0); 
        WriteRegister(CLKdist_LMK04610ConfigRegs[i].addr, CLKdist_LMK04610ConfigRegs[i].val_w);
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 1);   
    }

    // //odczyt danych z pamięci przetwornika
    for(int i = 0; i < configRegsIdx; i++)
    {
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 0); 
        ReadRegister(CLKdist_LMK04610ConfigRegs[i].addr, &CLKdist_LMK04610ConfigRegs[i].val_r);
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 1); 
    }

    // //sprawdzenie statusu konfiguracji
    for(int i = 0; i < configRegsIdx; i++)
    {
        if(CLKdist_LMK04610ConfigRegs[i].val_w == CLKdist_LMK04610ConfigRegs[i].val_r)
        {
            printf("\nREG  %d:  %s \r\t\t\t\t0x%x   0x%x   0x%x", i, CLKdist_LMK04610ConfigRegs[i].name.c_str(), CLKdist_LMK04610ConfigRegs[i].addr, CLKdist_LMK04610ConfigRegs[i].val_w, CLKdist_LMK04610ConfigRegs[i].val_r);
        }
        else
        {
            printf("\n\033[0;33mREG  %d:  %s \r\t\t\t\t0x%x   0x%x   0x%x \033[0m", i, CLKdist_LMK04610ConfigRegs[i].name.c_str(), CLKdist_LMK04610ConfigRegs[i].addr, CLKdist_LMK04610ConfigRegs[i].val_w, CLKdist_LMK04610ConfigRegs[i].val_r);
        }
        usleep(10000);
    }

    return result;
}

unsigned short int CLKdist_LMK04610 :: WriteRegister(unsigned short int reg, unsigned char val)
{
    unsigned short int result = 0;
    unsigned char txBuf[3];
    unsigned char rxBuf[3];
    unsigned short int temp_reg;

    temp_reg = 0x0000 | reg;

    txBuf[0] = (temp_reg >> 8);
    txBuf[1] = (temp_reg & 0xFF);
    txBuf[2] = val;

    spi_communicator->TransferData(txBuf, rxBuf, 3, 0);
    usleep(2000);

    return result;
}

unsigned short int CLKdist_LMK04610 :: ReadRegister(unsigned short int reg, unsigned char *val)
{
    unsigned short int result = 0;
    unsigned char txBuf[3];
    unsigned char rxBuf[3];
    unsigned short int temp_reg;

    temp_reg = 0x8000 | reg;

    txBuf[0] = (temp_reg >> 8);
    txBuf[1] = (temp_reg & 0xFF);
    txBuf[2] = 0x00;

    spi_communicator->TransferData(txBuf, rxBuf, 3, 3);
    usleep(2000);

    *val = rxBuf[2];
    
    return result;
}

unsigned short int CLKdist_LMK04610 :: ReadData(void)
{
    unsigned short int result = 0;
    return result;
}


