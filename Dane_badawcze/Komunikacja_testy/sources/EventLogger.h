/** @file */

#ifndef _EVENT_LOGGER
#define _EVENT_LOGGER

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fstream>

#define MAX_MESSAGE_SIZE 2048

class EventLogger
{
    private:
    char Message[MAX_MESSAGE_SIZE];
    std::string logFilePath;

    public:
    EventLogger(const char * path);
    ~EventLogger();
    unsigned char SaveEvent(const char *msg, char *timemsg, ...);
    const char * getLogPath() {return logFilePath.c_str();}
};

#endif
