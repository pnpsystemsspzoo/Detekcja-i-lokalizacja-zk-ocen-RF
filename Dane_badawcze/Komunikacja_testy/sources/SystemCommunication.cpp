#include "SystemCommunication.h"

using namespace std;

SystemCommunication :: SystemCommunication():
    eventLogger("../../event_logs.txt")
{
    streamMessageType = NOTHING_TO_STREAM;
    command = COMMAND_NULL;
    streamMessageReady == NOT_READY;
    messageOutReady == NOT_READY;

    streamMessageSize = 0;
    outMessageSize = 0;
}

int SystemCommunication :: InitCommandConnection(void)
{
    int nFoo = 1;

    // socket create and verification 
    command_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (command_sockfd == -1) { 
        printf("\nCommand socket creation failed...\n"); 
        eventLogger.SaveEvent("Command socket creation failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Command socket successfully created..\n");
        eventLogger.SaveEvent("Command socket successfully created", NULL);
    }

    setsockopt(command_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo)); 
    bzero(&command_servaddr, sizeof(command_servaddr)); 
  
    // assign IP, PORT
    memset(&command_servaddr, 0, sizeof(struct sockaddr)); 
    command_servaddr.sin_family = AF_INET; 
    command_servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    command_servaddr.sin_port = htons(commandPort); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(command_sockfd, (struct sockaddr*)&command_servaddr, sizeof(struct sockaddr))) != 0) 
    { 
        printf("Command socket bind failed with port %d ...\n", commandPort); 
        eventLogger.SaveEvent("Command socket bind failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Command socket successfully binded with port %d..\n", commandPort);
        eventLogger.SaveEvent("Command socket successfully binded", NULL);
    }
        
    // Now server is ready to listen and verification 
    if ((listen(command_sockfd, 1)) != 0) 
    { 
        printf("Command listen on port %d failed...\n", commandPort);
        eventLogger.SaveEvent("Command listen failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Command server listening on port %d..\n", commandPort);
        eventLogger.SaveEvent("Command server listening", NULL);
    }


    return 1;
}
         
    

int SystemCommunication :: InitStreamConnection(void)
{
    int nFoo = 1;

    // socket create and verification 
    stream_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (stream_sockfd == -1) { 
        printf("\nStream socket creation failed...\n"); 
        eventLogger.SaveEvent("Stream socket creation failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Stream socket successfully created..\n");
        eventLogger.SaveEvent("Stream socket successfully created", NULL);
    }

    setsockopt(stream_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo)); 
    bzero(&stream_servaddr, sizeof(stream_servaddr)); 
  
    // assign IP, PORT
    memset(&stream_servaddr, 0, sizeof(struct sockaddr)); 
    stream_servaddr.sin_family = AF_INET; 
    stream_servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    stream_servaddr.sin_port = htons(streamPort); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(stream_sockfd, (struct sockaddr*)&stream_servaddr, sizeof(struct sockaddr))) != 0) 
    { 
        printf("Stream socket bind failed with port %d ...\n", streamPort);
        eventLogger.SaveEvent("Stream socket bind failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Stream socket successfully binded with port %d..\n", streamPort);
        eventLogger.SaveEvent("Stream socket successfully binded", NULL);
    }
        
    // Now server is ready to listen and verification 
    if ((listen(stream_sockfd, 1)) != 0) 
    { 
        printf("Stream server listen failed on port %d...\n", streamPort);
        eventLogger.SaveEvent("Stream server listen failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Stream server listening on port %d..\n", streamPort);
        eventLogger.SaveEvent("Stream server listening", NULL);
    }
         

    return 1;
}


int SystemCommunication :: AcceptClientCommandConnection(void)
{
    // Accept the data packet from client and verification  
    command_len = sizeof(command_cli);
    command_connfd = accept(command_sockfd, (struct sockaddr*)&command_cli, &command_len);
    if(command_connfd < 0) 
    { 
        printf("Command server acccept failed...\n"); 
        eventLogger.SaveEvent("Command server acccept failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Command server acccept the client...\n");
        eventLogger.SaveEvent("Command server acccept the client", NULL);
    }

    return command_connfd;
}


int SystemCommunication :: AcceptClientStreamConnection(void)
{
    //Accept the data packet from client and verification 
    stream_len = sizeof(stream_cli);
    stream_connfd = accept(stream_sockfd, (struct sockaddr*)&stream_cli, &stream_len);
    if(stream_connfd < 0) 
    { 
        printf("Stream server acccept failed...\n"); 
        eventLogger.SaveEvent("Stream server acccept failed", NULL);
        exit(0); 
    } 
    else
    {
        printf("Stream server acccept the client...\n");
        eventLogger.SaveEvent("Stream server acccept the client", NULL);
    }

    return stream_connfd;
}


unsigned char SystemCommunication :: Start(unsigned short int stream_port, unsigned short int command_port)
{
    unsigned char result = 0;

    commandPort = command_port;
    streamPort = stream_port;

    InitCommandConnection();
    thread CommandInterfaceWorker(&SystemCommunication :: CommandInterface, this);
    CommandInterfaceWorker.detach();

    InitStreamConnection();
    thread StreamInterfaceWorker(&SystemCommunication :: StreamInterface, this);
    StreamInterfaceWorker.detach();

    thread StreamerWorker(&SystemCommunication :: Streamer, this);
    StreamerWorker.detach();

    thread MessageSenderWorker(&SystemCommunication :: MessageSender, this);
    MessageSenderWorker.detach();

    return result;
}


int SystemCommunication :: CommandInterface(void)
{
    int RWresult;
    char InBuff[127];
    short int commandTemp;
    
    do
    {
        if(AcceptClientCommandConnection() >= 0)
        {
            do
            {
                bzero(InBuff, 127);
                RWresult = ReceiveCommandMessage(InBuff);

                if(RWresult < 0)
                {
                    printf("\nCommand reading fault!!!");
                    eventLogger.SaveEvent("Command reading fault", NULL);
                    break;
                }

                if(RWresult == 0)
                {
                    printf("\nCommand connection closed!!!");
                    eventLogger.SaveEvent("Command connection closed", NULL);
                    break;
                }

                commandTemp = ((short int)InBuff[2] << 8) | InBuff[3];
                printf("\nCommand number: %d", commandTemp); 
                command = COMMAND_GET_SHORT_STATUS;
            }
        while(RWresult > 0);
        }     
    }
    while(1);
    return 1;
}



int SystemCommunication :: StreamInterface(void)
{
    int RWresult;
    char InBuff[127];
    
    do
    {
        if(AcceptClientStreamConnection() >= 0)
        {
            do
            {
                bzero(InBuff, 80);
                RWresult = ReceiveStreamMessage(InBuff);

                if(RWresult < 0)
                {
                    printf("\nStream reading fault!!!");
                    eventLogger.SaveEvent("Stream reading fault", NULL);
                    break;
                }

                if(RWresult == 0)
                {
                    printf("\nStream connection closed!!!");
                    eventLogger.SaveEvent("Stream connection closed", NULL);
                    break;
                }

                printf("\nStream from user: %s", InBuff); 

            }
            while(RWresult > 0);
        }    
    }
    while(1);
    return 1;
}


int SystemCommunication :: Streamer(void)
{
    int result = 0;

    do
    {
        if(streamMessageReady == READY)
        {
            result = SendStreamMessage(streamOutBuffer, streamMessageSize);
            streamMessageType = NOTHING_TO_STREAM;
            streamMessageReady = NOT_READY;
        }
        usleep(25000);
    } 
    while(1);

    return result;
}

int SystemCommunication :: MessageSender(void)
{
    int result = 0;

    do
    {
        if(messageOutReady == READY)
        {
            result = SendCommandMessage(messageOutBuffer, outMessageSize);
            command = COMMAND_NULL;
            messageOutReady = NOT_READY;
            command = COMMAND_NULL;
        }

        usleep(25000);
    } 
    while(1);

    return result;
}



int SystemCommunication :: ReceiveCommandMessage(char* buff)
{
    int result = 0;
    result = read(command_connfd, buff, 8); 
    return result;
}


int SystemCommunication :: SendCommandMessage(char* buff, int bytes_to_write)
{  
    int result = 0;
    if(bytes_to_write < 0)
    {
        // string message
        bytes_to_write = strlen(buff);
    }
    result =  write(command_connfd, buff, bytes_to_write);

    return result;
}


int SystemCommunication :: ReceiveStreamMessage(char* buff)
{
    int result = 0;
    result = read(stream_connfd, buff, 8); 
    return result;
}


int SystemCommunication :: SendStreamMessage(char* buff, int bytes_to_write)
{  
    int result = 0;
    if(bytes_to_write < 0)
    {
        // string message
        bytes_to_write = strlen(buff);
    }
    result =  write(stream_connfd, buff, bytes_to_write);
    return result;
}


int SystemCommunication :: StopConnection(void)
{
    int result = 0;
    // close the socket 
    close(command_sockfd); 
    close(stream_sockfd);

    return result;
}
