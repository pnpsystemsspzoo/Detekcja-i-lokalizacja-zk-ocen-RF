#include "Status.h"

using namespace std;

Status :: Status()
{
    alertFlag = 0;
}

unsigned char Status :: GetSystemParams(void)
{
    unsigned char result = 0;
    return result;
}


unsigned char Status :: GetBoardParams(void)
{
    unsigned char result = 0;
    return result;
}


unsigned char Status :: GetFPGAParams(void)
{
    unsigned char result = 0;
    return result;
}


unsigned char Status :: GetRadioCardParams(void)
{
    unsigned char result = 0;
    return result;
}


unsigned char Status :: PrepareShortMessage(void)
{
    unsigned char result = 0;

    ShortMessage[0] = 1;
    ShortMessage[1] = 2;
    ShortMessage[2] = 3;

    return result;
}


unsigned char Status :: PrepareLongMessage(void)
{
    unsigned char result = 0;
    return result;
}

 
unsigned char Status :: CheckStatusParams(void)
{
    unsigned char result = 0;

    GetSystemParams();
    GetBoardParams();
    GetFPGAParams();
    GetRadioCardParams();

    for(int i = 0; i < VOLTAGE_MEAS_POINT_MAX; i++)
    {
        if(boardParams.voltage[i] < boardParams.voltageMIN[i])
        {
            if(boardParams.voltage_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }

        if(boardParams.voltage[i] > boardParams.voltageMAX[i])
        {
            if(boardParams.voltage_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }
    }


    for(int i = 0; i < CURRENT_MEAS_POINT_MAX; i++)
    {
        if(boardParams.current[i] < boardParams.currentMIN[i])
        {
            if(boardParams.current_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }

        if(boardParams.current[i] > boardParams.currentMAX[i])
        {
            if(boardParams.current_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }
    }


    for(int i = 0; i < TEMP_MEAS_POINT_MAX; i++)
    {
        if(boardParams.temp[i] < boardParams.tempMIN[i])
        {
            if(boardParams.temp_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }
        
        if(boardParams.temp[i] > boardParams.tempMAX[i])
        {
            if(boardParams.temp_event_prior[i] = HIGH_PRIOR_EVENT)
            {

            }
        }
    }


    for(int i = 0; i < DIST_MEAS_POINT_MAX; i++)
    {
        if(boardParams.disturbances[i] < boardParams.disturbancesMIN[i])
        {
            if(boardParams.disturbances_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }

        if(boardParams.disturbances[i] > boardParams.disturbancesMAX[i])
        {
            if(boardParams.disturbances_event_prior[i] == HIGH_PRIOR_EVENT)
            {
                
            }
        }
    }


    for(int i = 0; i < INTERNAL_COMM_LINES_MAX; i++)
    {
        if(boardParams.comm[i] < boardParams.commMIN[i])
        {
            if(boardParams.comm_event_prior[i] == HIGH_PRIOR_EVENT)
            {

            }
        }

        if(boardParams.comm[i] > boardParams.commMAX[i])
        {
            if(boardParams.comm_event_prior[i] == HIGH_PRIOR_EVENT)
            {
                
            }
        }
    }







    for(int j = 0; j < RADIO_CARD_NUM_MAX; j++)
    {
        for(int i = 0; i < VOLTAGE_MEAS_POINT_MAX; i++)
        {
            if(radiocardParams[j].voltage[i] < radiocardParams[j].voltageMIN[i])
            {
                if(radiocardParams[j].voltage_event_prior[i] == HIGH_PRIOR_EVENT)
                {

                }
            }

            if(radiocardParams[j].voltage[i] > radiocardParams[j].voltageMAX[i])
            {
                if(radiocardParams[j].voltage_event_prior[i] == HIGH_PRIOR_EVENT)
                {

                }
            }
        }


        for(int i = 0; i < CURRENT_MEAS_POINT_MAX; i++)
        {
            if(radiocardParams[j].current[i] < radiocardParams[j].currentMIN[i])
            {
                if(radiocardParams[j].current_event_prior[i] == HIGH_PRIOR_EVENT)
                {

                }
            }

            if(radiocardParams[j].current[i] > radiocardParams[j].currentMAX[i])
            {
                if(radiocardParams[j].current_event_prior[i] == HIGH_PRIOR_EVENT)
                {

                }
            }
        }


        for(int i = 0; i < TEMP_MEAS_POINT_MAX; i++)
        {
            if(radiocardParams[j].temp[i] < radiocardParams[j].tempMIN[i])
            {
                if(radiocardParams[j].temp_event_prior[i] == HIGH_PRIOR_EVENT)
                {

                }
            }
        
            if(radiocardParams[j].temp[i] > radiocardParams[j].tempMAX[i])
            {
                if(radiocardParams[j].temp_event_prior[i] = HIGH_PRIOR_EVENT)
                {

                }
            }
        }


        for(int i = 0; i < INTERNAL_COMM_LINES_MAX; i++)
        {
            if(radiocardParams[j].comm[i] < radiocardParams[j].commMIN[i])
            {
                if(radiocardParams[j].comm_event_prior[i] == HIGH_PRIOR_EVENT)
                {

                }
            }

            if(radiocardParams[j].comm[i] > radiocardParams[j].commMAX[i])
            {
                if(radiocardParams[j].comm_event_prior[i] == HIGH_PRIOR_EVENT)
                {
                
                }
            }
        }
    }


    return result;
}


unsigned char Status :: StatusEventListener(SystemCommunication *communication)
{
    unsigned char result = 0;

    do
    {
        /*!
         * @brief Switch instruction responsible for handling commands 
         * comming from the Central System, which are a request to 
         * send a status message.
         */
        switch(communication->command)
        {
            case SystemCommunication :: COMMAND_GET_FULL_STATUS:
                PrepareLongMessage();
                communication->messageOutBuffer = LongMessage;
                communication->outMessageSize = LONG_STAUS_SIZE;
                communication->messageOutReady = READY;
                bzero(LongMessage, LONG_STAUS_SIZE);
            break;
            case SystemCommunication :: COMMAND_GET_SHORT_STATUS:
                PrepareShortMessage();
                communication->messageOutBuffer = ShortMessage;
                communication->outMessageSize = SHORT_STAUS_SIZE;
                communication->messageOutReady = READY;
                bzero(ShortMessage, SHORT_STAUS_SIZE);
            break;
            default:
            break;
        }

        CheckStatusParams();

        /*!
         * @brief If instuction launches sending an alert informing about 
         * incorrect external software operation
         */
        if(alertFlag == 1)
        {
            communication->streamOutBuffer = AlertMessage;
            communication->streamMessageSize = strlen(AlertMessage);
            communication->streamMessageReady = READY;
            bzero(AlertMessage, ALERT_MESSAGE_SIZE);
            alertFlag = 0;
        }

        usleep(25000);

    } while(1);
    
    return result;
}


unsigned char Status :: StartDiagnostics(SystemCommunication *communication)
{
    unsigned char result = 0;

    thread StatusEventListenerWorker(&Status :: StatusEventListener, this, communication);
    StatusEventListenerWorker.detach();
    
    return result;
}


unsigned char Status :: SaveAlert(char *message, unsigned char prior)
{
    EventLogger eventLogger("../../event_logs.txt");
    time_t current_time;
    char* c_time_string;
    const char *timeError = "Incorrect time"; 
    unsigned char result = 0;
    int msg_size;

    current_time = time(NULL);
    c_time_string = ctime(&current_time);

    eventLogger.SaveEvent(message, c_time_string);

    msg_size = strlen(message);
    if(msg_size <= ALERT_MESSAGE_SIZE)
    {
        if(c_time_string == NULL)
        {
            strcat(AlertMessage, timeError);
        }
        else
        {
            strcat(AlertMessage, c_time_string);
        }
        
        strcat(&AlertMessage[strlen(c_time_string) - 2], "\t");
        strcat(AlertMessage, message);
        strcat(AlertMessage, "\n");
    }

    if(prior == 1)
    {
        alertFlag = 1;
    }
    
    bzero(AlertMessage, ALERT_MESSAGE_SIZE);

    return result;
}



