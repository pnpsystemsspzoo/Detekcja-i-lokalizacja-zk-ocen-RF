/** @file */

#ifndef _EVENT_LOGGER
#define _EVENT_LOGGER

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string.h>
#include <time.h>

using namespace std;

class EventLogger
{
    private:
    fstream logFile;

    EventLogger(const char * log_path="event_logs.txt");
    EventLogger(const EventLogger &);

    public:
    ~EventLogger();
    /*!
     * @brief Returns string representation of time.
     * @return Current time saved as C string
     */
    char * getTimeString() const;
    template <typename T> EventLogger & operator << (const T & write)
    {
        logFile << this->getTimeString() << write << endl;
        return * this;
    }

    static EventLogger & SaveEvent();
    static EventLogger SaveUserEvent(uint8_t user_id);
};

#endif
