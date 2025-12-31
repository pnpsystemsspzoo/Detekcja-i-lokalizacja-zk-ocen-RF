#include "EventLogger.h"
#include "Settings.h"

using namespace std;

EventLogger :: EventLogger(const char *log_path)
{
    logFile.open(log_path, std::ios::out | std::ios::app);
    if(!logFile.good())
    {
        cout << "Logfile open error. Cannot open log file " << log_path << " for write. " << strerror(errno) << endl;
    }
}

EventLogger :: EventLogger(const EventLogger &)
{
}

EventLogger :: ~EventLogger() 
{
    logFile.close();
}

char *EventLogger::getTimeString() const
{
    time_t current_time = time(NULL);
    char * c_time_string = ctime(&current_time);
    c_time_string[strlen(c_time_string) - 2] = '\n';
    c_time_string[strlen(c_time_string) - 1] = '\t';

    if(c_time_string == NULL)
    {
        c_time_string = "Incorrect time";
    }
    return c_time_string;
}

EventLogger &EventLogger::SaveEvent()
{
    static EventLogger singleton(Settings::EventLogger.log_path.c_str());
    return singleton;
}

EventLogger EventLogger::SaveUserEvent(uint8_t user_id)
{
    return EventLogger((Settings::UserInterface.share_path + "/users/user" + to_string(user_id) + "_logs.txt").c_str());
}


