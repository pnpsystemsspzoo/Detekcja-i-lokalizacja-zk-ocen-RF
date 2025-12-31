#include "ADC_LTC2122.h"

using namespace std;

ADC_LTC2122 :: ADC_LTC2122()
{

}

ADC_LTC2122 :: ~ADC_LTC2122()
{

}

unsigned char ADC_LTC2122 :: Init(SPIcommunicator *spi)
{
    unsigned char result = 0;
    spi_communicator = spi;
    spi_ss_pin = 0;

    return result;
}


unsigned char ADC_LTC2122 :: SetConfiguration(const char* configFilePath, GPIO_EXP_MCP23017* csGPIOexp, unsigned int cs_addr, unsigned int cs_pin, unsigned int cs_exp_max)
{
    unsigned char result = 0;
    unsigned short int pinsVarRead;
    unsigned char csExpIdx = 0;
    fstream configFile;
    string line;
    string tokens[4];
    unsigned int tempInt;
    unsigned short int configRegsIdx = 0;

    printf("\nLoad LTC2122 configuration file %s", configFilePath);
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
                ADC_LTC2122ConfigRegs[configRegsIdx].name = tokens[0];
                
                start = line.find("0x");
                if(start > 0)
                {
                    tokens[1] = line.substr(start, 4);
                    tokens[2] = line.substr(start + 7, 4);
                    tokens[3] = line.substr(start + 7 + 7, 3);

                    if(sscanf(tokens[1].c_str(), "%x", &tempInt) == 1)
                    {
                        ADC_LTC2122ConfigRegs[configRegsIdx].addr = (unsigned char)tempInt;     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    if(sscanf(tokens[2].c_str(), "%x", &tempInt) == 1)
                    {
                        ADC_LTC2122ConfigRegs[configRegsIdx].val_w = (unsigned char)tempInt;     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    if(sscanf(tokens[3].c_str(), "%x", &tempInt) == 1)
                    {
                        ADC_LTC2122ConfigRegs[configRegsIdx].rw = (unsigned char)tempInt;     
                    }
                    else
                    {
                        printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                        exit(1);
                    }

                    configRegsIdx++;
                    if(configRegsIdx >= LTC2122_CONFIG_REGS_NUM) configRegsIdx = LTC2122_CONFIG_REGS_NUM;
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
        if(ADC_LTC2122ConfigRegs[i].rw == 0x0)
        {
            usleep(1000);
            csGPIOexp[csExpIdx].digitalWrite(cs_pin, 0);
            WriteRegister(ADC_LTC2122ConfigRegs[i].addr, ADC_LTC2122ConfigRegs[i].val_w);
            usleep(1000);
            csGPIOexp[csExpIdx].digitalWrite(cs_pin, 1);
        }   
    }

    //odczyt danych z pamięci przetwornika
    for(int i = 0; i < configRegsIdx; i++)
    {
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 0); 
        ReadRegister(ADC_LTC2122ConfigRegs[i].addr, &ADC_LTC2122ConfigRegs[i].val_r);
        usleep(1000);
        csGPIOexp[csExpIdx].digitalWrite(cs_pin, 1); 
    }

    //sprawdzenie statusu konfiguracji
    for(int i = 0; i < configRegsIdx; i++)
    {
        if(ADC_LTC2122ConfigRegs[i].val_w == ADC_LTC2122ConfigRegs[i].val_r)
        {
            printf("\nREG  %d:  %s \r\t\t\t\t\t\t0x%x   0x%x   0x%x   0x%x", i, ADC_LTC2122ConfigRegs[i].name.c_str(), ADC_LTC2122ConfigRegs[i].addr, ADC_LTC2122ConfigRegs[i].val_w, ADC_LTC2122ConfigRegs[i].val_r, ADC_LTC2122ConfigRegs[i].rw);
        }
        else
        {
            printf("\n\033[0;33mREG  %d:  %s \r\t\t\t\t\t\t0x%x   0x%x   0x%x   0x%x \033[0m", i, ADC_LTC2122ConfigRegs[i].name.c_str(), ADC_LTC2122ConfigRegs[i].addr, ADC_LTC2122ConfigRegs[i].val_w, ADC_LTC2122ConfigRegs[i].val_r, ADC_LTC2122ConfigRegs[i].rw);
        }
        usleep(10000);
    }

    return result;
}


unsigned char ADC_LTC2122 :: WriteRegister(unsigned char reg, unsigned char val)
{
    unsigned char result = 0;
    unsigned char txBuf[2];
    unsigned char rxBuf[2];
    unsigned char temp_reg;

    temp_reg = reg;

    txBuf[0] = temp_reg;
    txBuf[1] = val;

    spi_communicator->TransferData(txBuf, rxBuf, 2, 0);
    usleep(2000);

    return result;
}

unsigned char ADC_LTC2122 :: ReadRegister(unsigned char reg, unsigned char *val)
{
    unsigned char result = 0;
    unsigned char txBuf[2];
    unsigned char rxBuf[2];
    unsigned char temp_reg;

    temp_reg = 0x80 | reg;

    txBuf[0] = temp_reg;
    txBuf[1] = 0x00;

    spi_communicator->TransferData(txBuf, rxBuf, 2, 2);
    usleep(2000);

    *val = rxBuf[1];

    return result;
}


unsigned char ADC_LTC2122 :: ReadData(void)
{
    unsigned char result = 0;
    return result;
}


