#ifndef _SYSTEMCOMMUNICATION
#define _SYSTEMCOMMUNICATION

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <netdb.h> 
#include <netinet/in.h>  
#include <sys/socket.h> 
#include <sys/types.h> 
#include <thread>

#include "EventLogger.h"

#define MAX_SIZE_MESSAGE_OUT_BUFF       1024
#define MAX_SIZE_STREAM_OUT_BUFF        1024
#define NOTHING_TO_STREAM               0
#define ALLERT_STREAM_MESSAGE           10
#define SIGNATURE_STREAM_MESSAGE        20
#define FFT_DATA_STREAM_MESSAGE         30

#define READY                           1
#define NOT_READY                       0


class SystemCommunication
{ 
    private:
    EventLogger eventLogger;
    
    public:
    typedef enum COMMAND_TYPE
    {
        COMMAND_NULL                    = 1100,
        COMMAND_GET_FULL_STATUS         = 1101,
        COMMAND_GET_SHORT_STATUS        = 1102,
        COMMAND_RESTART                 = 1103,
        COMMAND_GET_EVENTS_LIST         = 1104,
        COMMAND_GET_CONFIG_FILES        = 1105,
        COMMAND_POST_CONFIG_FILES       = 1106,
        COMMAND_SFTWARE_UPGRADE         = 1107,
        COMMAND_STREAM_RAW_DATA         = 1108,
        COMMAND_STOP_STREAM_RAW_DATA    = 1109
    }CommandType;

    int command_sockfd;
    int command_connfd; 
    int stream_sockfd;
    int stream_connfd;
    unsigned int command_len;
    unsigned int stream_len;
    struct sockaddr_in command_servaddr;
    struct sockaddr_in command_cli; 
    struct sockaddr_in stream_servaddr;
    struct sockaddr_in stream_cli;
  
    unsigned short int commandPort = 10000;
    unsigned short int streamPort = 10001;
    char *ipaddr;

    unsigned char streamMessageType;
    unsigned char streamMessageReady;
    int streamMessageSize;
    char *streamOutBuffer;

    CommandType command;
    unsigned char messageOutReady;
    int outMessageSize;
    char *messageOutBuffer;;

    SystemCommunication();
    int InitCommandConnection(void);
    int InitStreamConnection(void);
    int AcceptClientCommandConnection(void);
    int AcceptClientStreamConnection(void);
    int CommandInterface(void);
    int StreamInterface(void);
    int MessageSender(void);
    int Streamer(void);
    int ReceiveCommandMessage(char* buff);
    int SendCommandMessage(char* buff, int bytes_to_write);
    int ReceiveStreamMessage(char* buff);
    int SendStreamMessage(char* buff, int bytes_to_write);
    int StopConnection(void);
    unsigned char Start(unsigned short int stream_port, unsigned short int command_port);
};


#endif
  
