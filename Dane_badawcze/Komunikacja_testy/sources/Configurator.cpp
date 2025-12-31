#include "Configurator.h"

using namespace std;

unsigned char Configurator :: LoadLicenseFile(void)
{
    unsigned char result = 0;

    if(encryptor.setKeyFromFile("../../key.txt"))
    {
        if(encryptor.decryptFile("../../license.bin", &params))
        {
            eventLogger.SaveEvent("License file loaded successully", NULL);
        }
        else
        {
            eventLogger.SaveEvent("License file open error", NULL);
        }
    }
    else
    {
        eventLogger.SaveEvent("License file open error", NULL);
    }
    

    return result;
}

