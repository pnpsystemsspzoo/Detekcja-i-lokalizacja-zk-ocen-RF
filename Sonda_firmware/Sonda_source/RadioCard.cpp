#include "RadioCard.h"

using namespace std;

RadioCard :: RadioCard()
{
    unsigned int gpioEXPnumber = 0;
}

RadioCard :: ~RadioCard()
{

}

unsigned char RadioCard :: Init(void)
{
    unsigned char result = 0;
    unsigned char internal_result = 0;

    INA226addr = 0x40;
    PCA9546addr = 0x74;


    if(internal_result = ina226.Init("/dev/i2c-1", INA226addr) > 0)
    {
        result = 0x02;
        printf("\nINA226 could not be intialized! Error code: %d", internal_result);
    }
    else
    {
        printf("\nINA226 intialized");
    }


    if(internal_result = i2cSWD.Init("/dev/i2c-1", PCA9546addr) > 0)
    {
        result = 0x03;
        printf("\nPCA9546 could not be intialized! Error code: %d", internal_result);
    }
    else
    {
        printf("\nPCA9546 intialized");

        i2cSWD.EnableChannel(CHANNEL0);
        i2cSWD.EnableChannel(CHANNEL2);
    }

    return result;
}

int RadioCard :: SetConfiguration(const char* configFilePath)
{
    int result = 0;
    fstream configFile;
    string line;
    string delimiter = " ";
    string tokens[10];
    int idxtoken = 0;
    string device;
    string adc1_cfg_path;
    string adc2_cfg_path;
    string adc3_cfg_path;
    string clkdist_cfg_path;
    unsigned int CSexpAddr;
    unsigned int CSexpPin;


    printf("\n\nLoad radio card configuration file %s", configFilePath);
    configFile.open(configFilePath, ios::in);
    if(configFile.is_open())
    {
        printf("\nConfiguration file %s opened successfully", configFilePath);

        while(getline(configFile, line))
        {
            if(line.at(0) != '#')
            {
                auto start = 0U;
                auto end = line.find(delimiter);

                while(end != std::string::npos)
                {
                    tokens[idxtoken] = line.substr(start, end - start);
                    idxtoken++;
                    if(idxtoken > 9) idxtoken = 9;
                    start = end + delimiter.length();
                    end = line.find(delimiter, start);
                }
                tokens[idxtoken] = line.substr(start, end);

                //Read the kind of device
                if(tokens[0] == "device")
                {
                    device = tokens[2];

                    if(device == "MCP23017")
                    {
                        gpioEXPnumber++;
                        if(gpioEXPnumber >= GPIO_EXP_NUMBER) gpioEXPnumber = GPIO_EXP_NUMBER;
                    }
                }

                if(device == "MCP23017")
                {
                    if(tokens[0] == "i2c_addr")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_addr) == 1)
                        {
                            printf("\n\nExpander 0x%x configuration", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_addr);
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "i2c_num")
                    {
                        gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_num = tokens[2];
                        if(result = gpioEXP_MCP23017[gpioEXPnumber].Init(gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_num.c_str(), gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_addr) > 0)
                        {
                            printf("\n\033[0;31minitialization expander on device %s: FILED - error code: %d\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_num.c_str(), result);
                            exit(1);
                        }
                        else
                        {
                            printf("\n\033[0;32minitialization expander on device %s: OK\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.i2c_num.c_str());
                        }
                    }

                    if(tokens[0] == "pins_mode")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeWrote) == 1)
                        {
                            printf("\nWrite pins mode: 0x%x", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeWrote);
                            result = gpioEXP_MCP23017[gpioEXPnumber].pinsMode(gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeWrote);
                            usleep(100000);

                            result = gpioEXP_MCP23017[gpioEXPnumber].readPinsMode(&gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeRed);
                            if((gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeWrote == gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeRed) && (result == 0))
                            {
                                printf("\n\033[0;32mPins mode was set successfully: 0x%x\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeRed);
                            }
                            else
                            {
                                printf("\n\033[0;31mPins mode is invalid: 0x%x\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsModeRed);
                            }       
                            usleep(100000);
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "pins_state")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarWrote) == 1)
                        {
                            printf("\nWrite pins state: 0x%x", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarWrote);
                            result = gpioEXP_MCP23017[gpioEXPnumber].writeGPIOAB(gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarWrote);
                            usleep(100000);

                            result = gpioEXP_MCP23017[gpioEXPnumber].readGPIOAB(&gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarRed);
                            if((gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarRed == gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarWrote) && (result == 0))
                            {
                                printf("\n\033[0;32mPins state was set successfully: 0x%x\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarRed);
                            }
                            else
                            {
                                printf("\n\033[0;31mPins state is invalid: 0x%x\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsVarRed);
                            } 
                            usleep(100000);
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "pins_pull_Up")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeWrote) == 1)
                        {
                            printf("\nWrite pins pull up state: 0x%x", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeWrote);
                            result = gpioEXP_MCP23017[gpioEXPnumber].pinsPullUpState(gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeWrote);
                            usleep(100000);

                            result = gpioEXP_MCP23017[gpioEXPnumber].readPinsPullUpState(&gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeRed);
                            if((gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeRed == gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeWrote) && (result == 0))
                            {
                                printf("\n\033[0;32mPins pull up state was set successfully: 0x%x\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeRed);
                            }
                            else
                            {
                                printf("\n\033[0;31mPins pull up state is invalid: 0x%x\033[0m", gpioEXP_MCP23017[gpioEXPnumber].gpioEXPconfigRegs.pinsPullUpModeRed);
                            } 
                            usleep(100000);
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m");
                            exit(1);
                        }
                    }
                }
                

                if(device == "AD9690")
                {
                    if(tokens[0] == "spi_cs_addr")
                    {
                        printf("\n\n\n%s configuration", device.c_str());
                        
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpAddr) == 1)
                        {
                            
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_cs_pin")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpPin) == 1)
                        {

                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_num")
                    {
                        if(spi_communicator.Init(tokens[2].c_str()) >= 0)
                        {
                            printf("\n\033[0;32mSPI device: %s initialization: OK\033[0m", tokens[2].c_str());
                        }
                        else
                        {
                            printf("\n\033[0;31mSPI device: %s initialization: FILED\033[0m", tokens[2].c_str());
                        }

                        if(result = adc1_AD9690.Init(&spi_communicator) > 0)
                        {
                            printf("\n\033[0;31minitialization ADC1 %s on device %s: FILED - error code: %d\033[0m", device.c_str(), tokens[2].c_str(), result);
                        }
                        else
                        {
                            printf("\n\033[0;32minitialization ADC1 %s on device %s: OK\033[0m", device.c_str(), tokens[2].c_str());
                        } 
                    }

                    if(tokens[0] == "adc1_path_cfg")
                    {
                        adc1_cfg_path = tokens[2];
                        adc1_AD9690.SetConfiguration(adc1_cfg_path.c_str(), gpioEXP_MCP23017, CSexpAddr, CSexpPin, GPIO_EXP_NUMBER);
                    }
                }


                if(device == "LTC2122")
                {
                    if(tokens[0] == "spi_cs_addr")
                    {
                        printf("\n\n%s configuration", device.c_str());
                    
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpAddr) == 1)
                        {
                            
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_cs_pin")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpPin) == 1)
                        {

                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_num")
                    {
                        if(spi_communicator.Init(tokens[2].c_str()) >= 0)
                        {
                            printf("\n\033[0;32mSPI device: %s initialization: OK\033[0m", tokens[2].c_str());
                        }
                         else
                        {
                            printf("\n\033[0;31mSPI device: %s initialization: FILED\033[0m", tokens[2].c_str());
                        }

                        if(result = adc2_LTC2122.Init(&spi_communicator) > 0)
                        {
                            printf("\n\033[0;31minitialization ADC2 %s on device %s: FILED - error code: %d\033[0m", device.c_str(), tokens[2].c_str(), result);
                        }
                        else
                        {
                            printf("\n\033[0;32minitialization ADC2 %s on device %s: OK\033[0m", device.c_str(), tokens[2].c_str());
                        }
                    }

                    if(tokens[0] == "adc2_path_cfg")
                    {
                        adc2_cfg_path = tokens[2];
                        adc2_LTC2122.SetConfiguration(adc2_cfg_path.c_str(), gpioEXP_MCP23017, CSexpAddr, CSexpPin, GPIO_EXP_NUMBER);
                    }
                }


                if(device == "MAX2771")
                {
                    if(tokens[0] == "spi_cs_addr")
                    {
                        printf("\n\n%s configuration", device.c_str());
                    
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpAddr) == 1)
                        {
                            
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_cs_pin")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpPin) == 1)
                        {

                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_num")
                    {
                        if(spi_communicator.Init(tokens[2].c_str()) >= 0)
                        {
                            printf("\n\033[0;32mSPI device: %s initialization: OK\033[0m", tokens[2].c_str());
                        }
                         else
                        {
                            printf("\n\033[0;31mSPI device: %s initialization: FILED\033[0m", tokens[2].c_str());
                        }

                        if(result = adc3_MAX2771.Init(&spi_communicator) > 0)
                        {
                            printf("\n\033[0;31minitialization ADC3 %s on device %s: FILED - error code: %d\033[0m", device.c_str(), tokens[2].c_str(), result);
                        }
                        else
                        {
                            printf("\n\033[0;32minitialization ADC3 %s on device %s: OK\033[0m", device.c_str(), tokens[2].c_str());
                        }
                    }

                    if(tokens[0] == "adc3_path_cfg")
                    {
                        adc3_cfg_path = tokens[2];
                        adc3_MAX2771.SetConfiguration(adc3_cfg_path.c_str(), gpioEXP_MCP23017, CSexpAddr, CSexpPin, GPIO_EXP_NUMBER);
                    }
                }


                if(device == "LMK04610")
                {
                    if(tokens[0] == "spi_cs_addr")
                    {
                        printf("\n\n%s configuration", device.c_str());
                    
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpAddr) == 1)
                        {
                            
                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_cs_pin")
                    {
                        if(sscanf(tokens[2].c_str(), "%x", &CSexpPin) == 1)
                        {

                        }
                        else
                        {
                            printf("\n\033[0;31mConfig file parsing error: bad input\033[0m\n");
                            exit(1);
                        }
                    }

                    if(tokens[0] == "spi_num")
                    {
                        if(spi_communicator.Init(tokens[2].c_str()) >= 0)
                        {
                            printf("\n\033[0;32mSPI device: %s initialization: OK\033[0m", tokens[2].c_str());
                        }
                         else
                        {
                            printf("\n\033[0;31mSPI device: %s initialization: FILED\033[0m", tokens[2].c_str());
                        }

                        if(result = clkdist_LMK04610.Init(&spi_communicator) > 0)
                        {
                            printf("\n\033[0;31minitialization Clock Dist %s on device %s: FILED - error code: %d\033[0m", device.c_str(), tokens[2].c_str(), result);
                        }
                        else
                        {
                            printf("\n\033[0;32minitialization Clock Dist %s on device %s: OK\033[0m", device.c_str(), tokens[2].c_str());
                        }
                    }

                    if(tokens[0] == "clkdist_path_cfg")
                    {
                        clkdist_cfg_path = tokens[2];
                        clkdist_LMK04610.SetConfiguration(clkdist_cfg_path.c_str(), gpioEXP_MCP23017, CSexpAddr, CSexpPin, GPIO_EXP_NUMBER);
                    }
                }

                idxtoken = 0;
            }
        }
    }
    else
    {
        printf("\n\033[0;31mCannot open Configuration file %s\033[0m", configFilePath);
    }
    configFile.close();

    printf("\n\n");
    return result;
}

unsigned char RadioCard :: ReadData(void)
{
    unsigned char result = 0;
    return result;
}

