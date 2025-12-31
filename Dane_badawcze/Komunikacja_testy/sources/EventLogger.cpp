#include "EventLogger.h"

using namespace std;

EventLogger :: EventLogger(const char * path) :
    logFilePath(path)
{

}

EventLogger :: ~EventLogger()
{
}

unsigned char EventLogger :: SaveEvent(const char *msg, char *timemsg, ...)
{
    time_t current_time;
    char* c_time_string;
    const char *timeError = "Incorrect time";
    fstream logFilefd;
    unsigned char result = 0;
    int msg_size;
    va_list args;
    char message[1024];

    sprintf(message, msg, &args);

    if(timemsg == NULL)
    {
        current_time = time(NULL);
        c_time_string = ctime(&current_time);
    }
    else
    {
        c_time_string = timemsg;
    }

    bzero(Message, MAX_MESSAGE_SIZE);
    msg_size = strlen(message);
    if(msg_size <= MAX_MESSAGE_SIZE)
    {
        if(c_time_string == NULL)
        {
            strcat(Message, timeError);
        }
        else
        {
            strcat(Message, c_time_string);
        }

        strcat(&Message[strlen(c_time_string) - 2], "\t");
        strcat(Message, message);
        strcat(Message, "\n");
    }

    logFilefd.open(logFilePath.c_str(), ios::out | ios::app);
    if(logFilefd.good() == true)
    {
        //cout << "\nLogfile opened successfully" << endl;
    }
    else
    {
        cout << "EventLogger: cannot open log file for write: " << logFilePath.c_str() << endl;
    }

    logFilefd.write(Message, strlen(Message));
    logFilefd.close();

    return result;
}

