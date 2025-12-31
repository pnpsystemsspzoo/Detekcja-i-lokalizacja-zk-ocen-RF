/** @file */

#ifndef _CONFIGURATOR
#define _CONFIGURATOR

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <fstream>
#include <ctime>

#include "EventLogger.h"
#include "parser.h"
#include "Encryptor.h"

class Configurator
{
    private:
    EventLogger eventLogger;

    public:
    Encryptor encryptor;
    Parser::config_set params;

    unsigned char LoadLicenseFile(void);
};

#endif
