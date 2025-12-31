#ifndef DEVICE_H
#define DEVICE_H

#include <thread>
#include <string>
#include <ctime>
#include <fstream>
#include "parser.h"
#include "DeviceDataBase.h"
#include "DeviceCommunication.h"
#include "EventLogger.h"
#include "probeMessagesEncoder.h"
#include "ThreatClassifier.h"
#include "ActiveThreatsManager.h"
#include "FrontCommunication.h"


#define STATUS_TIME_01                  1000
/*! @brief maximum number of radio cards */
#define RADIO_CARD_NUM_MAX              5
/*! @brief status ID number of mainboard */
#define MAIN_BOARD_NUM                  0
/*! @brief status ID number of radio card 1*/
#define RADIO_CARD1_NUM                 1
/*! @brief status ID number of radio card 2*/
#define RADIO_CARD2_NUM                 2
/*! @brief status ID number of radio card 3*/
#define RADIO_CARD3_NUM                 3
/*! @brief status ID number of radio card 4*/
#define RADIO_CARD4_NUM                 4
/*! @brief status ID number of radio card 5*/
#define RADIO_CARD5_NUM                 5
/*! @brief maximum size of message status */
#define STAUS_SIZE                      92
/*! @brief maximum size of localization data */
#define LOCALIZATION_DATA_SIZE          32

class Device
{
    private:
    typedef enum COMMAND_TYPE
    {
        COMMAND_NULL                    = 1100,
        COMMAND_GET_SHORT_STATUS        = 1102,
        COMMAND_RESTART                 = 1103,
        COMMAND_GET_EVENTS_LIST         = 1104,
        COMMAND_GET_CONFIG_FILES        = 1105,
        COMMAND_POST_CONFIG_FILES       = 1106,
        COMMAND_SFTWARE_UPGRADE         = 1107,
        COMMAND_STREAM_RAW_DATA         = 1108,
        COMMAND_STOP_STREAM_RAW_DATA    = 1109,
        COMMAND_GET_LOCALIZE_DATA       = 1110
    }CommandType;
    
    typedef struct
    {
        char head[5];
        unsigned char softwareHealthStatus;
        unsigned char systemHealthStatus;
        unsigned char boardHealthStatus;
        unsigned char radioCardsHealthStatus[RADIO_CARD_NUM_MAX + 1];
        unsigned char RAMusage;
        unsigned char CPUusage;
        unsigned int tasksNumber;
        unsigned char diskSpaceUsage;
        unsigned char transferUsage;
        unsigned int txTransmit;
        unsigned int rxTransmit;
        unsigned char noiseDiscovered;
        unsigned int alertsNumber;
        float mainVoltagePoints[RADIO_CARD_NUM_MAX + 1];
        float mainCurrentPoints[RADIO_CARD_NUM_MAX + 1];
        char tail;
    }StatusMessage;

    typedef struct
    {
        char head[5];
        unsigned char setFlag;
        double lon;
        double lat;
        float hight;
        char tail;
    } LocalizationData;

    int devIndex;
    DeviceCommunication deviceCommunication;
    FrontCommunication::Sender * frontendSender;

    void MainDeviceFunction(void);
    int ServeInternalEvent(void);
    int ServeStreamDeviceEvent(void);
    int ServeCommnadDeviceEvent(void);
    int ServeUserEvent(CommandType command);
    int FindIPaddr(string MACaddr, string* IPaddr);

    public:
    Parser :: config_set deviceParams;
    DeviceDataBase *deviceDB;
    CommandType actCommandInfo;
    StatusMessage statusMessage;
    LocalizationData localizationData;


    // ActiveThreatsManager is used to cache threats considered as "active" in RAM memory
    // and allow to minimize number of requests into the database
    ActiveThreatsManager * atm;
    // device version and unique id are not used when decoding - place anything (constructor parameters)
    ProbeMessagesEncoder decoder;


    Device(int idx, DeviceDataBase* db, ActiveThreatsManager * atm);
    ~Device();
    int GetIndex(void);
    /*!
     * @brief Checks if device is empty i.e. does not hold any data
     * @return True if device is not initialized and there are no data. Otherwise false
     */
    bool isEmpty() { return (GetIndex() < 0);}
    int DeleteData(int idx);
    int UpdateData(int idx, Parser::config_set* deviceData);
    int GetStatus(void);
    int Restart(void);
    int GetEventsList(void);
    int GetConfigFiles(void);
    int PostConfigFiles(const std::string & file_path);
    int SoftwareUpgrade(const std::string & file_path);
    int StartStreamRawData(void);
    int StopStreamRawData(void);
    int GetLocalizeData(void);
    void setSender(FrontCommunication::Sender * sender) {this->frontendSender = sender;}
};

#endif
