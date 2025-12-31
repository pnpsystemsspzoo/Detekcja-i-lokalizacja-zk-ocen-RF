#include "DeviceCommunication.h"
#include "Settings.h"

using namespace std;

DeviceCommunication :: DeviceCommunication()
{
    receivedCommandFrameFlag = 0;
    receivedThreatFrameFlag = 0;
    receivedRawDataFrameFlag = 0;
    receivedAllertFrameFlag = 0;
}

DeviceCommunication :: ~DeviceCommunication()
{
    close(command_sockfd);
    close(stream_sockfd);
}

int DeviceCommunication :: InitCommandConnection(void)
{
    int nFoo = 1;

    // socket create and varification 
    command_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (command_sockfd == -1) 
    { 
        EventLogger :: SaveEvent() << "Device command socket creation failed";
    } 
    else
    {
        EventLogger :: SaveEvent() << "Device command socket successfully created";
    }

    //setsockopt(command_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo)); 
    //fcntl(command_sockfd, F_SETFL, O_NONBLOCK);
    bzero(&command_servaddr, sizeof(command_servaddr)); 
  
    // assign IP, PORT 
    command_servaddr.sin_family = AF_INET; 
    command_servaddr.sin_addr.s_addr = inet_addr(ipaddr.c_str());
    command_servaddr.sin_port = htons(commandPort); 

    command_timeout.tv_sec  = 10;  // after 7 seconds connect() will timeout
    command_timeout.tv_usec = 0;
    setsockopt(command_sockfd, SOL_SOCKET, SO_SNDTIMEO, &command_timeout, sizeof(command_timeout));

    // connect the client socket to server socket 
    command_connection_status = connect(command_sockfd, (struct sockaddr*)&command_servaddr, sizeof(command_servaddr));
    if(command_connection_status != 0) 
    { 
        EventLogger :: SaveEvent() << "Command connection with the device server failed - IP addr: " + string(ipaddr) + " - port num: " + to_string(commandPort);
    } 
    else
    {
        EventLogger :: SaveEvent() << "Command connected to the device server - IP addr: " + string(ipaddr) + " - port num: " + to_string(commandPort);
    }

    return 1;
}


int DeviceCommunication :: InitStreamConnection(void)
{
    int nFoo = 1;
    char *tempStr;

    // socket create and varification 
    stream_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (stream_sockfd == -1) 
    { 
        EventLogger :: SaveEvent() << "Device stream socket creation failed";
    } 
    else
    {
        EventLogger :: SaveEvent() << "Device stream socket successfully created";
    }

    //setsockopt(stream_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo)); 
    //fcntl(stream_sockfd, F_SETFL, O_NONBLOCK);
    bzero(&stream_servaddr, sizeof(stream_servaddr)); 
  
    // assign IP, PORT 
    stream_servaddr.sin_family = AF_INET; 
    stream_servaddr.sin_addr.s_addr = inet_addr(ipaddr.c_str());
    stream_servaddr.sin_port = htons(streamPort); 

    stream_timeout.tv_sec  = 60;  // after 7 seconds connect() will timeout
    stream_timeout.tv_usec = 0;
    setsockopt(stream_sockfd, SOL_SOCKET, SO_SNDTIMEO, &stream_timeout, sizeof(stream_timeout));

    // connect the client socket to server socket
    stream_connection_status = connect(stream_sockfd, (struct sockaddr*)&stream_servaddr, sizeof(stream_servaddr));
    if(stream_connection_status != 0) 
    { 
        EventLogger :: SaveEvent() << "Stream connection with the device server failed - IP addr: " + string(ipaddr) + " - port num: " + to_string(streamPort);
    } 
    else
    {
        EventLogger :: SaveEvent() << "Stream connected to the device server - IP addr: " + string(ipaddr) + " - port num: " + to_string(streamPort);
    }

    return 1;
}


int DeviceCommunication :: CommandInterface(void)
{
    int RWresult = 0;
    int AvalilableBytes;

    do
    {
        usleep(5000000);

        if(RWresult == 0)
        {
            InitCommandConnection();
        }

        usleep(1000000);

        if(command_connection_status == 0)
        {
            do
            {
                RWresult = ReceiveCommandMessage(InCommandBuff, InCommandMessageSize);
                receivedCommandFrameFlag = 1;
            }
            while(RWresult != 0);
        }
        close(command_sockfd);
    }
    while(1);

    return 1;
}


int DeviceCommunication :: StreamInterface(void)
{
    int RWresult = 0;

    do
    {
        usleep(5000000);

        if(RWresult == 0)
        {
            InitStreamConnection();
        }

        usleep(1000000);
        
        if(stream_connection_status == 0)
        {
            do
            {
                RWresult = ReceiveStreamMessage();
            } 
            while(RWresult != 0);
        }
        close(stream_sockfd);
    }
    while(1);

    return 1;
}


int DeviceCommunication :: ReceiveCommandMessage(void* buff, int bufSize) 
{
    int result; 
    int AvalilableBytes = bufSize;  

    result = read(command_sockfd, buff, AvalilableBytes); 

    if(result < 0)
    {
        EventLogger :: SaveEvent() << "Command message from device reading fault!!!";
    }

    if(result == 0)
    {
        EventLogger :: SaveEvent() << "Command connection with device closed!!!";
    }

    return result;
} 


int DeviceCommunication :: SendCommandMessage(char* buff, int msgsize)
{  
    int result;
    result = send(command_sockfd, buff, msgsize, MSG_NOSIGNAL); 
    return result;
}


int DeviceCommunication :: ReceiveStreamMessage() 
{
    int result; 
    int AvalilableBytes;
    char headBuff[5];

    result = recv(stream_sockfd, headBuff, 5, MSG_PEEK); 
    //printf("\n\n\n  %s\n\n", headBuff);

    if((headBuff[0] == '#') && (headBuff[1] == 'D') && (headBuff[2] == 'I') && (headBuff[3] == 'S') && (headBuff[4] == 'T'))
    {
        AvalilableBytes = THREAT_BUFF_SIZE;
        result = recv(stream_sockfd, ThreatBuffer, AvalilableBytes, 0);
        receivedThreatFrameFlag = 1;
    } 

    if((headBuff[0] == '#') && (headBuff[1] == 'R') && (headBuff[2] == 'A') && (headBuff[3] == 'W') && (headBuff[4] == 'D'))
    {
        AvalilableBytes = RAWDATA_BUFF_SIZE;
        result = recv(stream_sockfd, RawDataBuffer, AvalilableBytes, 0);
        receivedRawDataFrameFlag = 1;
    }

    if((headBuff[0] == '#') && (headBuff[1] == 'A') && (headBuff[2] == 'L') && (headBuff[3] == 'R') && (headBuff[4] == 'T'))
    {
        AvalilableBytes = ALLERT_BUFF_SIZE;
        result = recv(stream_sockfd, AllertBuffer, AvalilableBytes, 0);
        receivedAllertFrameFlag = 1;
    }

    if(result < 0)
    {
        EventLogger :: SaveEvent() << "Stream message from device reading fault!!!";
    }

    if(result == 0)
    {
        EventLogger :: SaveEvent() << "Stream connection with device closed!!!";
    }

    
    return result;
} 


int DeviceCommunication :: SendStreamMessage(char* buff, int msgsize)
{  
    int result;
    result = send(stream_sockfd, buff, msgsize, MSG_NOSIGNAL); 
    return result;
}



