/** @file */

#ifndef _STATUS
#define _STATUS

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <thread>
#include <fstream>

#include "SystemCommunication.h"
#include "EventLogger.h"

/*! @brief maximum number of radio cards */
#define RADIO_CARD_NUM_MAX              5
/*! @brief maximum number of voltage measuring points */
#define VOLTAGE_MEAS_POINT_MAX          32
/*! @brief maximum number of current measuring points */
#define CURRENT_MEAS_POINT_MAX          32
/*! @brief maximum number of temperature measuring points */
#define TEMP_MEAS_POINT_MAX             32
/*! @brief maximum number of disturbances an attitude measuring points */
#define DIST_MEAS_POINT_MAX             4
/*! @brief maximum number of communication lines */
#define INTERNAL_COMM_LINES_MAX         10
/*! @brief maximum size of short message status */
#define SHORT_STAUS_SIZE                127
/*! @brief maximum size of long message status */
#define LONG_STAUS_SIZE                 256
/*! @brief maximum size of alert message */
#define ALERT_MESSAGE_SIZE              256
/*! @brief high level priority event */
#define HIGH_PRIOR_EVENT                1
/*! @brief low level priority event */
#define LOW_PRIOR_EVENT                 0


/*!
 * @brief Class Status manages probe status information and
 * monitors its values. When any of the values exceeds the 
 * specified safety threshold, the class generates a warning message. 
 * All messages are saved in the database.
 */
class Status
{
    private:

    typedef struct
    {
        float p1;   
    }SystemParams;

    typedef struct
    {
        unsigned char voltage_event_prior[VOLTAGE_MEAS_POINT_MAX]; ///< Table storing priority of voltage diagnostic variables
        float voltageMAX[VOLTAGE_MEAS_POINT_MAX];
        float voltage[VOLTAGE_MEAS_POINT_MAX];
        float voltageMIN[VOLTAGE_MEAS_POINT_MAX];   

        unsigned char current_event_prior[CURRENT_MEAS_POINT_MAX];
        float currentMAX[CURRENT_MEAS_POINT_MAX];
        float current[CURRENT_MEAS_POINT_MAX];
        float currentMIN[CURRENT_MEAS_POINT_MAX];   

        unsigned char temp_event_prior[TEMP_MEAS_POINT_MAX];
        float tempMAX[TEMP_MEAS_POINT_MAX];
        float temp[TEMP_MEAS_POINT_MAX];
        float tempMIN[TEMP_MEAS_POINT_MAX];   

        unsigned char disturbances_event_prior[DIST_MEAS_POINT_MAX];
        float disturbancesMAX[DIST_MEAS_POINT_MAX];
        float disturbances[DIST_MEAS_POINT_MAX];
        float disturbancesMIN[DIST_MEAS_POINT_MAX];

        unsigned char comm_event_prior[INTERNAL_COMM_LINES_MAX];
        float commMAX[INTERNAL_COMM_LINES_MAX];
        float comm[INTERNAL_COMM_LINES_MAX];
        float commMIN[INTERNAL_COMM_LINES_MAX];  

    }BoardParams;

    typedef struct
    {
        float p1;   
    }FPGAParams;

    typedef struct
    {
        unsigned char voltage_event_prior[VOLTAGE_MEAS_POINT_MAX];
        float voltageMAX[VOLTAGE_MEAS_POINT_MAX];
        float voltage[VOLTAGE_MEAS_POINT_MAX];
        float voltageMIN[VOLTAGE_MEAS_POINT_MAX];   

        unsigned char current_event_prior[CURRENT_MEAS_POINT_MAX];
        float currentMAX[CURRENT_MEAS_POINT_MAX];
        float current[CURRENT_MEAS_POINT_MAX];
        float currentMIN[CURRENT_MEAS_POINT_MAX];   

        unsigned char temp_event_prior[TEMP_MEAS_POINT_MAX];
        float tempMAX[TEMP_MEAS_POINT_MAX];
        float temp[TEMP_MEAS_POINT_MAX];
        float tempMIN[TEMP_MEAS_POINT_MAX];  

        unsigned char comm_event_prior[INTERNAL_COMM_LINES_MAX];
        float commMAX[INTERNAL_COMM_LINES_MAX];
        float comm[INTERNAL_COMM_LINES_MAX];
        float commMIN[INTERNAL_COMM_LINES_MAX];  
    }RadioCardParams;


    SystemParams systemParams;
    BoardParams boardParams;
    FPGAParams fpgaParams;
    RadioCardParams radiocardParams[RADIO_CARD_NUM_MAX];


    public:
    Status();
    char ShortMessage[SHORT_STAUS_SIZE];
    char LongMessage[LONG_STAUS_SIZE];
    char AlertMessage[ALERT_MESSAGE_SIZE];
    unsigned char alertFlag;
    
    unsigned char GetSystemParams(void);
    unsigned char GetBoardParams(void);
    unsigned char GetFPGAParams(void);
    unsigned char GetRadioCardParams(void);
    unsigned char PrepareShortMessage(void);
    unsigned char PrepareLongMessage(void);
    unsigned char CheckStatusParams(void);
    unsigned char StatusEventListener(SystemCommunication *communication);
    unsigned char StartDiagnostics(SystemCommunication *communication);
    unsigned char SaveAlert(char *message, unsigned char prior);
};

#endif
