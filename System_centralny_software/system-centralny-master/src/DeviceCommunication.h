#ifndef DEVICECOMMUNICATION_H
#define DEVICECOMMUNICATION_H

#include <netdb.h> 
#include <cstdio>
#include <cstdlib>
#include <string> 
#include <sys/socket.h>
#include <sys/time.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "EventLogger.h"

/*! @brief maximum size of message with signature */
#define THREAT_BUFF_SIZE             54
#define RAWDATA_BUFF_SIZE            1024
#define ALLERT_BUFF_SIZE             2048

class DeviceCommunication
{
    private:
    int command_sockfd;
    int command_connfd; 
    int stream_sockfd;
    int stream_connfd; 
    struct sockaddr_in command_servaddr;
    struct sockaddr_in command_cli;
    struct timeval command_timeout; 
    struct sockaddr_in stream_servaddr;
    struct sockaddr_in stream_cli;
    struct timeval stream_timeout;
    int command_connection_status;
    int stream_connection_status;

    public:   
    long int commandPort;
    long int streamPort;
    std::string ipaddr;

    char ThreatBuffer[THREAT_BUFF_SIZE];
    char RawDataBuffer[RAWDATA_BUFF_SIZE];
    char AllertBuffer[ALLERT_BUFF_SIZE];
    unsigned char receivedThreatFrameFlag;
    unsigned char receivedRawDataFrameFlag;
    unsigned char receivedAllertFrameFlag;
    void* InCommandBuff;
    int InCommandMessageSize;
    unsigned char receivedCommandFrameFlag;

    DeviceCommunication();
    ~DeviceCommunication();
    int InitCommandConnection(void);
    int InitStreamConnection(void);
    int CommandInterface(void);
    int StreamInterface(void);
    int ReceiveCommandMessage(void* buff, int bufSize);
    int SendCommandMessage(char* buff, int msgsize);
    int ReceiveStreamMessage(void);
    int SendStreamMessage(char* buff, int msgsize);
    /*!
     * @brief Checks if device is connected
     * @return True if device is connected. Otherwise false
     */
    bool isConnected() { return (this->stream_connection_status >= 0);}
    void operator()() const
    {
        InitStreamConnection();
        StreamInterface();
    }
};

#endif
  
