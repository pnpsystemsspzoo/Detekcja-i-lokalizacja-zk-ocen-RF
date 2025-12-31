#include "Device.h"

using namespace std;

Device :: Device(int idx, DeviceDataBase* db, ActiveThreatsManager * atm) :
frontendSender(nullptr),
deviceDB(db),
devIndex(idx),
deviceCommunication(),
atm(atm),
decoder(0,0)
{
    actCommandInfo = COMMAND_NULL;

    deviceDB->GetDataSelectedDevice(idx, &deviceParams);

    deviceCommunication.commandPort = deviceParams.control_port;
    deviceCommunication.streamPort = deviceParams.data_port;

    thread MainDeviceWorker(&Device :: MainDeviceFunction, this);
    MainDeviceWorker.detach();
}

Device :: ~Device()
{
}

void Device :: MainDeviceFunction(void)
{
    string tmp_str;

    //searching MAC address and IP adress of device in ARP table
    while(FindIPaddr(deviceParams.mac_addr, &tmp_str) != 1)
    {
        EventLogger :: SaveEvent() << "Device " + deviceParams.mac_addr + " not found";
        usleep(5000000);
    }
    deviceParams.ip_address = tmp_str;
    deviceCommunication.ipaddr = tmp_str;
    EventLogger :: SaveEvent() << "Device " + deviceParams.mac_addr + " found - IP: " + deviceCommunication.ipaddr.c_str();

    //run workers responsible for communication witch device
    thread CommandCommunicationWorker(&DeviceCommunication :: CommandInterface, &deviceCommunication);
    thread StreamCommunicationWorker(&DeviceCommunication :: StreamInterface, &deviceCommunication);
    CommandCommunicationWorker.detach();
    StreamCommunicationWorker.detach();
    usleep(2000000);
    thread InternalEventWorker(&Device :: ServeInternalEvent, this);
    InternalEventWorker.detach();

    do
    {
        ServeStreamDeviceEvent();
        ServeCommnadDeviceEvent();
    } 
    while(1);
}

int Device :: ServeInternalEvent(void)
{
    int result = 0;
    int req1Timer = 0;

    do
    {
        GetLocalizeData();
        usleep(1000000);
    } while(localizationData.setFlag == 0);
    

    do
    {
        req1Timer++;
        if(req1Timer >= STATUS_TIME_01)
        {
            GetStatus();
            req1Timer = 0;            
        }
        usleep(1000);
    }
    while(1);

    return result;
}

int Device :: ServeStreamDeviceEvent(void)
{
    int result = 0;

    if(deviceCommunication.receivedThreatFrameFlag == 1)
    {
        // for(int i = 0; i < THREAT_BUFF_SIZE; i++)
        // {
        //     printf("\ndeviceCommunication.ThreatBuffer[%d]:    %c", i, deviceCommunication.ThreatBuffer[i]);
        // }
        // printf("\n\n\n");

        // decode message
        auto decoded = decoder.unpackMessage(deviceCommunication.ThreatBuffer, THREAT_BUFF_SIZE);
        // Check type of the message. Try to cast message to messageThreat
        auto msgThreat = dynamic_cast<ProbeMessagesEncoder::messageThreat*>(decoded.get());

        if(msgThreat != nullptr)
        {
            // received message is messageThreat
            // get number of threats holded by this message (can be up to 5 in the single message)
            uint8_t number_of_threats_in_message = msgThreat->countThreats();
            // get pointer to the array containing all threats
            // return value is ProbeMessagesEncoder::messageThreat::threat_descriptor_t *
            auto buffer_with_threats = msgThreat->getThreats();
            // process all incoming threats
            for(uint8_t i=0; i < number_of_threats_in_message; ++i)
            {
                // write signature to the ActiveThreatsManager (signature will be automatically pushed to the database after it expires)
                atm->pushSignature(deviceParams.id, buffer_with_threats[i], deviceParams.longitude, deviceParams.latitude);
            }
        }

        bzero(deviceCommunication.ThreatBuffer, THREAT_BUFF_SIZE);
        deviceCommunication.receivedThreatFrameFlag = 0;
    }


    if(deviceCommunication.receivedRawDataFrameFlag == 1)
    {
        // for(int i = 0; i < RAWDATA_BUFF_SIZE; i++)
        // {
        //     printf("\ndeviceCommunication.RawDataBuffer[%d]:    %c", i, deviceCommunication.RawDataBuffer[i]);
        // }
        // printf("\n\n\n");

        bzero(deviceCommunication.RawDataBuffer, RAWDATA_BUFF_SIZE);
        deviceCommunication.receivedRawDataFrameFlag = 0;
    }


    if(deviceCommunication.receivedAllertFrameFlag == 1)
    {
        // for(int i = 0; i < ALLERT_BUFF_SIZE; i++)
        // {
        //     printf("\ndeviceCommunication.AllertBuffer[%d]:    %c", i, deviceCommunication.AllertBuffer[i]);
        // }
        // printf("\n\n\n");

        bzero(deviceCommunication.AllertBuffer, ALLERT_BUFF_SIZE);
        deviceCommunication.receivedAllertFrameFlag = 0;
    }

    return result;
}

int Device :: ServeCommnadDeviceEvent(void)
{
    int result = 0;

    if(deviceCommunication.receivedCommandFrameFlag == 1)
    {
        switch(actCommandInfo)
        {
            case COMMAND_NULL:
            break;
            case COMMAND_GET_SHORT_STATUS:
                //printf("\n\nStatus message: %c%c%c  %d   %d", statusMessage.head[0], statusMessage.head[1], statusMessage.head[2], statusMessage.rxTransmit, statusMessage.tail);
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_RESTART:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_GET_EVENTS_LIST:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_GET_CONFIG_FILES:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_POST_CONFIG_FILES:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_SFTWARE_UPGRADE:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_STREAM_RAW_DATA:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_STOP_STREAM_RAW_DATA:
                actCommandInfo = COMMAND_NULL;
            break;
            case COMMAND_GET_LOCALIZE_DATA:
                //printf("\n\n\nLat %f   Lon %f    Heigth  %f\n\n", localizationData.lon, localizationData.lat, localizationData.hight);
                deviceParams.longitude = localizationData.lon;
                deviceParams.latitude = localizationData.lat;
                deviceParams.height = (unsigned int)localizationData.hight;
                deviceParams.firstmess = 1;
                deviceDB->UpdateSelectedDevice(devIndex, &deviceParams);
                actCommandInfo = COMMAND_NULL;
            break;
            default:
            break;
        }

        //bzero((char*)deviceCommunication.InCommandBuff, sizeof((char*)deviceCommunication.InCommandBuff));
        deviceCommunication.receivedCommandFrameFlag = 0;
    }

    return result;
}

int Device :: ServeUserEvent(CommandType command)
{
    int result = 0;
    char buff[8];
    short int commandTemp;

    commandTemp = (short int)command;
    buff[0] = '$';
    buff[1] = 0;
    buff[2] = (char)(commandTemp >> 8);
    buff[3] = (char)(commandTemp);
    buff[4] = 0;
    buff[5] = 0;
    buff[6] = 0;
    buff[7] = 'K';
    result = deviceCommunication.SendCommandMessage(buff, 8);

    return result;
}

int Device :: FindIPaddr(string MACaddr, string* IPaddr)
{
    int result = 0;
    string line;
    string mac;
    string ip;
    fstream file;
    string delimiter = " ";
    int pos;
    string token;
    unsigned char firstPiece;

    //open file with ARP table data
    file.open("/proc/net/arp", std::ios::in);
    if(file.good() == true)
    {
        while(getline(file, line))
        {
            pos = 0;
            line = line + " ";
            firstPiece = 0;

            while((pos = line.find(delimiter)) != string :: npos)
            {
                token = line.substr(0, pos);
                line.erase(0, pos + delimiter.length());

                //remember IP addres
                if(firstPiece == 0)
                {
                    ip = token;
                    firstPiece = 1;
                }

                if(pos == 17)
                {
                    //check MAC address assigned to remembered IP address
                    if(token == MACaddr)
                    {
                        *IPaddr = ip;
                        result = 1;
                        break;
                    }
                }
            }
        }
        file.close();
    }
    else
    {
        result = -1;
    }
    
    return result;
}




int Device :: GetIndex(void)
{
    return devIndex;
}

int Device :: DeleteData(int idx)
{
    int result = 0;

    result = deviceDB->DeleteSelectedDevice(idx);

    if(!(result < 0))
    {
        // success
        // set idx to negative in order to indicate this object is empty
        devIndex = -1;
    }

    return result;
}

int Device :: UpdateData(int idx, Parser::config_set* deviceData)
{
    int result = 0;

    deviceDB->UpdateSelectedDevice(idx, deviceData);

    return result;
}

int Device :: GetStatus(void)
{
    int result = 0;
    deviceCommunication.InCommandBuff = (void*)&statusMessage;
    deviceCommunication.InCommandMessageSize = STAUS_SIZE;
    result = ServeUserEvent(COMMAND_GET_SHORT_STATUS);
    actCommandInfo = COMMAND_GET_SHORT_STATUS;
    return result;
}

int Device :: Restart(void)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_RESTART);
    actCommandInfo = COMMAND_RESTART;
    return result;
}

int Device :: GetEventsList(void)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_GET_EVENTS_LIST);
    actCommandInfo = COMMAND_GET_EVENTS_LIST;
    string tempStr = "sudo curl -k \"sftp://" + deviceCommunication.ipaddr + ":22/event_logs.txt\" --user \"mock:1234\" -o \"/home/software/SC/NodeWWW/docker/shared-volume/public/share/probe" + to_string(devIndex) + "/event_logs.txt\"";
    system(tempStr.c_str());
    if(frontendSender->isClientConneted())
    {
        FrontCommunication::message::get_log_response response;
        response.device_ID = devIndex;
        response.log_file_name = "event_logs.txt";
        response.status = 0; // set status to no error

        frontendSender->SendMessage(response);

        //printf("\nIP: %s   ID: %d\n", deviceCommunication.ipaddr.c_str(), devIndex);
    }
    return result;
}

int Device :: GetConfigFiles(void)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_GET_CONFIG_FILES);
    actCommandInfo = COMMAND_GET_CONFIG_FILES;
    return result;
}

int Device :: PostConfigFiles(const std::string & file_path)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_POST_CONFIG_FILES);
    actCommandInfo = COMMAND_POST_CONFIG_FILES;
    return result;
}

int Device :: SoftwareUpgrade(const string &file_path)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_SFTWARE_UPGRADE);
    actCommandInfo = COMMAND_SFTWARE_UPGRADE;
    return result;
}

int Device :: StartStreamRawData(void)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_STREAM_RAW_DATA);
    actCommandInfo = COMMAND_STREAM_RAW_DATA;
    return result;
}

int Device :: StopStreamRawData(void)
{
    int result = 0;
    result = ServeUserEvent(COMMAND_STOP_STREAM_RAW_DATA);
    actCommandInfo = COMMAND_STOP_STREAM_RAW_DATA;
    return result;
}

int Device :: GetLocalizeData(void)
{
    int result = 0;
    deviceCommunication.InCommandBuff = (void*)&localizationData;
    deviceCommunication.InCommandMessageSize = LOCALIZATION_DATA_SIZE;
    result = ServeUserEvent(COMMAND_GET_LOCALIZE_DATA);
    actCommandInfo = COMMAND_GET_LOCALIZE_DATA;
    return result;
}

