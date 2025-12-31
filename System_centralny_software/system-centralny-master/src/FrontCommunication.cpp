#include "FrontCommunication.h"
#include "Settings.h"

void FrontCommunication::setClientConnectedStatus(bool is_connected)
{
    // assert last known state has changed
    if(this->client_connected == is_connected) return;

    this->client_connected = is_connected;
    if(this->client_connected)
    {
        EventLogger :: SaveEvent() << "FrontCommunication: Frontend client connected";
    }else
    {
        close(this->connfd);
        this->connfd = -1;
        EventLogger :: SaveEvent() << "FrontCommunication: Frontend client disconnected";
    }
}

void FrontCommunication::addListener(FrontCommunication::listener *l)
{
    if(!(std::find(listeners.begin(), listeners.end(), l) != listeners.end()))
    {
        listeners.push_back(l);
    }
}

void FrontCommunication::removeListener(FrontCommunication::listener *l)
{
    listeners.remove(l);
}

FrontCommunication :: FrontCommunication() :
    connfd(-1),
    sockfd(-1),
    sender(this)
{
    tx_buffer = new char[FRONT_COMMUNICATION_TX_BUFFER_SIZE_KB*1024];
}

FrontCommunication::~FrontCommunication()
{
    delete [] tx_buffer;
}


bool FrontCommunication :: InitConnection(void)
{   
    EventLogger :: SaveEvent() << "FrontCommunication: initializing TCP server";

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        EventLogger :: SaveEvent() << "FrontCommunication: TCP socket creation failed " + string(strerror(errno));
        return false;
    }
    //EventLogger :: SaveEvent() << "FrontCommunication: TCP socket successfully created",NULL);

    // allow reuse address
    int nFoo = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));


    // assign IP, PORT
    memset(&servaddr, 0, sizeof(struct sockaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(this->ipaddr.c_str());
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        EventLogger :: SaveEvent() << "FrontCommunication: binding TCP socket with the server address failed. " + string(strerror(errno));
        close(sockfd);
        return false;
    }
    //EventLogger :: SaveEvent() << "FrontCommunication: TCP socket successfully binded",NULL);

    // Now server is ready to listen and verification
    if ((listen(sockfd, 1)) != 0) {
        EventLogger :: SaveEvent() << "FrontCommunication: TCP server listening failed. " + string(strerror(errno));
        close(sockfd);
        return false;
    }
    EventLogger :: SaveEvent() << "FrontCommunication: TCP server is listening on port ..." + to_string(port);

    return true;
}

bool FrontCommunication::isListening()
{
    int val;
    socklen_t len = sizeof(val);
    if (getsockopt(this->sockfd, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1)
    {
        // sockfd is not a socket
        return false;
    }
    else if (val)
    {
        // socket is listening
        return true;
    }
    // non-listening socket
    return false;
}


int FrontCommunication :: AcceptClientMessage(void)
{
    // Accept the data packet from client and verification
    unsigned int len = sizeof(cli);
    int new_connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if(new_connfd < 0)
    {
        EventLogger :: SaveEvent() << "FrontCommunication: failed to accept client";
    }else
    {
        // client have sent some message

        if(this->connfd < 0)
        {
            // client was disconnected before so set new status
            this->setClientConnectedStatus(true);
        }
    }
    this->connfd = new_connfd;
    return connfd;
}


int FrontCommunication :: ReceiveMessage(char * buff)
{
    int result;
    int AvalilableBytes;

    if(!this->isClientConneted()) return -1;

    result = ioctl(connfd, FIONREAD, &AvalilableBytes);   


    if(AvalilableBytes <= 0)
    {
        return 0;
    }
    result = read(connfd, buff, AvalilableBytes);


    if(result <= 0)
    {
        // cought client disconnected status
        this->setClientConnectedStatus(false);
    }else
    {
        this->setClientConnectedStatus(true);
    }

    return result;
}

void FrontCommunication::ParseUserMessage(const char *buff)
{
    uint8_t frame_id = *buff; buff++;
    uint8_t user_id = *buff; buff++;
    switch(frame_id)
    {
        case message::OPERATOR_INTERFACE_QUERY:
            for (auto & l : listeners) l->onOperatorInterfaceQuery(user_id);
        break;
        case message::ADMIN_INTERFACE_QUERY:
            for (auto & l : listeners) l->onAdminInterfaceQuery(user_id);
        break;
        case message::DEVICES_MANAGEMENT_QUERY:
            for (auto & l : listeners) l->onDevicesManagementQuery(user_id);
        break;
        case message::DEVICE_STATUS_QUERY:
        {
            uint16_t device_id = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            for (auto & l : listeners) l->onDeviceStatusQuery(user_id,device_id);
        }
        break;
        case message::ADD_DEVICE_QUERY:
        {
            int n = static_cast<int>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            for (auto & l : listeners) l->onAddDeviceQuery(user_id,buff,n);
        }
        break;
        case message::DELETE_DEVICE_QUERY:
        {
            uint16_t device_id = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            for (auto & l : listeners) l->onDeleteDeviceQuery(user_id,device_id);
        }
        break;
        case message::FFT_QUERY:
        {
            uint16_t device_id = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            uint8_t channel = static_cast<uint8_t>(*buff); buff++;
            bool repeat = static_cast<bool>(*buff); buff++;
            for (auto & l : listeners) l->onFFTQuery(user_id,device_id,channel,repeat);
        }
        break;
        case message::STOP_DEVICE_STATUS_QUERY:
        {
            uint16_t device_id = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            for (auto & l : listeners) l->onStopDeviceStatusQuery(user_id,device_id);
        }
        break;
        case message::UPDATE_DEVICES_CONFIG_QUERY:
        {
            std::string filename;
            buff = message::unpack_string(buff,&filename);
            int n = static_cast<int>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            std::vector<uint16_t> device_ID(reinterpret_cast<const uint16_t*>(buff),reinterpret_cast<const uint16_t*>(buff)+n);
            for (auto & l : listeners) l->onUpdateDevicesConfigQuery(user_id,filename,device_ID);
        }
        break;
        case message::UPDATE_DEVICES_FIRMWARE_QUERY:
        {
            std::string filename;
            buff = message::unpack_string(buff,&filename);
            int n = static_cast<int>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            std::vector<uint16_t> device_ID(reinterpret_cast<const uint16_t*>(buff),reinterpret_cast<const uint16_t*>(buff)+n);
            for (auto & l : listeners) l->onUpdateDevicesFirmwareQuery(user_id,filename,device_ID);
        }
        break;
        case message::RESET_DEVICE_QUERY:
        {
            uint16_t device_id = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            for (auto & l : listeners) l->onResetDeviceQuery(user_id,device_id);
        }
        case message::GET_LOG_QUERY:
        {
            uint16_t device_id = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(int16_t);
            for (auto & l : listeners) l->onGetLogQuery(user_id,device_id);
        }
        break;
        default:
            // unresolved command
            EventLogger :: SaveEvent() << "FrontCommunication: received unrecognized frontend message with id " + to_string(frame_id);
        break;
    }

}

uint32_t getOffset(char * begin, char * end)
{
    return static_cast<uint32_t>(reinterpret_cast<uint64_t>(end) - reinterpret_cast<uint64_t>(begin));
}


uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::radio_interference &msg)
{
    *buff = message::RADIO_INTERFERENCE;
    char * p = message::pack_int32(static_cast<int32_t>(msg.signal_ID),&buff[1]);
    p = message::pack_double(msg.longitude,p);
    p = message::pack_double(msg.latitude,p);
    p = message::pack_int16(static_cast<int16_t>(msg.height),p);
    p = message::pack_int16(static_cast<int16_t>(msg.range),p);
    p = message::pack_int16(static_cast<int16_t>(msg.heading),p);
    p = message::pack_int16(static_cast<int16_t>(msg.speed),p);

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::radio_interference *msg)
{
    buff++;
    msg->signal_ID = static_cast<uint32_t>(message::unpack_int32(buff)); buff += sizeof(uint32_t);
    msg->longitude = message::unpack_double(buff); buff += sizeof(double);
    msg->latitude = message::unpack_double(buff); buff += sizeof(double);
    msg->height = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->range = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->heading = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->speed = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::cs_status &msg)
{
    buff[0] = message::CS_STATUS;
    buff[1] = msg.health;
    buff[2] = msg.communication_status;
    buff[3] = msg.CPU_usage;
    buff[4] = msg.RAM_usage;
    buff[5] = msg.disk_usage;
    char * p = message::pack_int16(static_cast<int16_t>(msg.tasks_number),&buff[6]);

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::cs_status *msg)
{
    buff++;
    msg->health = *buff; buff++;
    msg->communication_status = *buff; buff++;
    msg->CPU_usage = *buff; buff++;
    msg->RAM_usage = *buff; buff++;
    msg->disk_usage = *buff; buff++;
    msg->tasks_number = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::dev_full_status &msg)
{
    buff[0] = message::DEVICE_FULL_STATUS;
    char * p = message::pack_int16(static_cast<int16_t>(msg.device_ID),&buff[1]);
    *p = msg.health; p++;
    p = message::pack_int16(static_cast<int16_t>(msg.motherboard_voltage),p);
    p = message::pack_int16(static_cast<int16_t>(msg.motherboard_current),p);
    *p = msg.cards; p++;
    for(uint8_t i=0;i<msg.cards;++i){
        p = message::pack_int16(static_cast<int16_t>(msg.card_voltage[i]),p);
        p = message::pack_int16(static_cast<int16_t>(msg.card_current[i]),p);
    }
    p = message::pack_int16(static_cast<int16_t>(msg.cards_health),p);
    p = message::pack_int32(static_cast<int32_t>(msg.alerts),p);
    *p = msg.radio_interference; p++;
    *p = msg.communication_status; p++;
    *p = msg.connected; p++;
    *p = msg.CPU_usage; p++;
    *p = msg.RAM_usage; p++;
    *p = msg.disk_usage; p++;
    p = message::pack_int32(static_cast<int32_t>(msg.tx_transfer_rate),p);
    p = message::pack_int32(static_cast<int32_t>(msg.rx_transfer_rate),p);
    p = message::pack_int16(static_cast<int16_t>(msg.tasks_number),p);
    *p = msg.synchro_status; p++;
    *p = msg.time_std; p++;
    memcpy(p,msg.datetime,24); p += 24; *p = 0; p++;
    *p = msg.threats_length; p++;
    for (uint8_t i=0;i<msg.threats_length;++i)
    {
        p = message::pack_int16(static_cast<int16_t>(msg.threats_table.get()[i].bandwidth),p);
        p = message::pack_int16(static_cast<int16_t>(msg.threats_table.get()[i].f0),p);
        *p = msg.threats_table.get()[i].threat_type; p++;
    }

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::dev_full_status *msg)
{
    buff++;
    msg->device_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->health = *buff; buff++;
    msg->motherboard_voltage = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->motherboard_current = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->cards = *buff; buff++;
    for(uint8_t i=0;i<msg->cards;++i){
        msg->card_voltage[i] = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
        msg->card_current[i] = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    }
    msg->cards_health = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->alerts = static_cast<uint32_t>(message::unpack_int32(buff)); buff += sizeof(uint32_t);

    msg->radio_interference = *buff; buff++;
    msg->communication_status = *buff; buff++;
    msg->connected = *buff; buff++;
    msg->CPU_usage = *buff; buff++;
    msg->RAM_usage = *buff; buff++;
    msg->disk_usage = *buff; buff++;
    msg->tx_transfer_rate = static_cast<uint32_t>(message::unpack_int32(buff)); buff += sizeof(int32_t);
    msg->rx_transfer_rate = static_cast<uint32_t>(message::unpack_int32(buff)); buff += sizeof(int32_t);
    msg->tasks_number = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->synchro_status = *buff; buff++;
    msg->time_std = *buff; buff++;
    memcpy(msg->datetime,buff,25); buff += 25;
    msg->threats_length = *buff; buff++;
    msg->threats_table.reset(new message::threat_info[msg->threats_length]);
    for(uint i=0;i<msg->threats_length;++i)
    {
        msg->threats_table.get()[i].bandwidth = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
        msg->threats_table.get()[i].f0 = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
        msg->threats_table.get()[i].threat_type = *buff; buff++;
    }

}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::dev_short_status &msg)
{
    buff[0] = message::DEVICE_SHORT_STATUS;
    char * p = message::pack_int16(static_cast<int16_t>(msg.device_ID),&buff[1]);
    *p = msg.health; p++;
    *p = msg.communication_status; p++;
    *p = msg.radio_interference; p++;

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::dev_short_status *msg)
{
    buff++;
    msg->device_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->health = *buff; buff++;
    msg->communication_status = *buff; buff++;
    msg->radio_interference = *buff; buff++;
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::dev_info *devices, uint16_t n)
{
    buff[0] = message::DEVICE_LIST;
    char * p = message::pack_int16(static_cast<int16_t>(n),&buff[1]);
    for(int i=0;i<n;++i)
    {
        p = message::pack_device_info(devices[i],p);
    }

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::dev_info **devices, uint16_t *n)
{
    buff++;
    *n = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    *devices = new message::dev_info[*n];
    for(uint16_t i=0; i<*n;++i)
    {
        buff = message::unpack_device_info(buff,&(*devices)[i]);
    }
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::fft &msg)
{
    buff[0] = message::FFT;
    char * p = message::pack_int16(static_cast<int16_t>(msg.device_ID),&buff[1]);
    *p = msg.channel; p++;
    p = message::pack_int16(static_cast<int16_t>(msg.f_begin),p);
    p = message::pack_int16(static_cast<int16_t>(msg.f_end),p);
    p = message::pack_int16(static_cast<int16_t>(msg.data_number),p);
    for(uint16_t i=0; i<msg.data_number;i++)
    {
        p = message::pack_int16(static_cast<int16_t>(msg.power_density.get()[i]),p);
    }
    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::fft *msg)
{
    buff++;
    msg->device_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->channel = *buff; buff++;
    msg->f_begin = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->f_end = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->data_number = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->power_density.reset(new int16_t[msg->data_number]);
    for(uint16_t i=0;i<msg->data_number;++i)
    {
        msg->power_density.get()[i] = message::unpack_int16_t(buff); buff += sizeof(int16_t);
    }
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::add_device &msg)
{
    buff[0] = message::ADD_DEVICE;
    buff[1] = msg.status;
    char * p = message::pack_device_info(msg.device_info,&buff[2]);

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::add_device *msg)
{
    buff++;
    msg->status = *buff; buff++;
    buff = message::unpack_device_info(buff,&msg->device_info);
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::delete_device &msg)
{
    buff[0] = message::DELETE_DEVICE;
    char * p = message::pack_int16(static_cast<int16_t>(msg.device_ID),&buff[1]);
    *p = msg.status; p++;

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::delete_device *msg)
{
    buff++;
    msg->device_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->status = *buff; buff++;
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::new_file_uploaded &msg, uint8_t frame_id)
{
    buff[0] = frame_id;
    buff[1] = msg.user_ID;
    buff[2] = msg.status;
    char * p = message::packString(msg.filename,&buff[3]);

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::new_file_uploaded *msg, uint8_t *frame_id)
{
    *frame_id = *buff; buff++;
    msg->user_ID = *buff; buff++;
    msg->status = *buff; buff++;
    buff = message::unpack_string(buff,&msg->filename);
}

uint32_t FrontCommunication::message::packMessage(char* buff, const FrontCommunication::message::load_status &msg, uint8_t frame_id)
{
    buff[0] = frame_id;
    char * p = message::packString(msg.filename,&buff[1]);
    p = message::pack_int16(static_cast<int16_t>(msg.device_length),p);
    for(uint16_t i = 0; i<msg.device_length; ++i)
    {
        p = message::pack_operation_response(msg.device_status.get()[i],p);
    }

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::load_status *msg, uint8_t *frame_id)
{
    *frame_id = *buff; buff++;
    buff = message::unpack_string(buff,&msg->filename);
    msg->device_length = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    msg->device_status.reset(new message::device_operation_response[msg->device_length]);
    for(uint16_t i=0;i<msg->device_length;++i)
    {
        buff = message::unpack_operation_response(buff,&msg->device_status.get()[i]);
    }
}

uint32_t FrontCommunication::message::packMessage(char *buff, const FrontCommunication::message::alert_msg &msg)
{
    buff[0] = message::ALERT;
    char * p = message::pack_int16(static_cast<int16_t>(msg.alert_ID),&buff[1]);
    p = message::packString(msg.timestamp,p);
    p = message::packString(msg.message,p);

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::alert_msg *msg)
{
    buff++;
    msg->alert_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    buff = unpack_string(buff,&msg->timestamp);
    buff = unpack_string(buff,&msg->message);
}

uint32_t FrontCommunication::message::packMessage(char *buff, const FrontCommunication::message::get_log_response &msg)
{
    buff[0] = message::GET_LOG_RESPONSE;
    buff[1] = msg.status;
    char * p = message::pack_int16(static_cast<int16_t>(msg.device_ID),&buff[2]);
    p = message::packString(msg.log_file_name,p);

    return getOffset(buff,p);
}

void FrontCommunication::message::unpackMessage(const char *buff, FrontCommunication::message::get_log_response *msg)
{
    buff++;
    msg->status = *buff; buff++;
    msg->device_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    buff = unpack_string(buff,&msg->log_file_name);
}

FrontCommunication::Sender::Sender(FrontCommunication *parent) :
    frontCommunication(parent)
{

}

int FrontCommunication :: Sender:: SendMessage(char * buff, int n)
{
    // assert connfd
    if(!this->isClientConneted()) {
        EventLogger :: SaveEvent() << "FrontCommunication::SendMessage: WARNING Frontend is disconnected. Dropping write message.";
        return frontCommunication->connfd;
    }
    int result;
    if(n<0) n = strlen(buff);

    frontCommunication->tx_mutex.lock();

    char *ptr = (char*) buff;
    while (n > 0)
    {
        result = send(frontCommunication->connfd, ptr, n, MSG_NOSIGNAL);
        if(result < 0){
            // error
            // Detected client disconnect event
            frontCommunication->setClientConnectedStatus(false);
            break;
        }
        if (result == 0) break;
        ptr += result;
        n -= result;
    }
    frontCommunication->tx_mutex.unlock();

    return result;
}


int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::radio_interference &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::cs_status &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::dev_full_status &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::dev_short_status &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const message::dev_info *devices, uint16_t n)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,devices,n);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::fft &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::add_device &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::delete_device &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::new_file_uploaded &msg, uint8_t frame_id)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg,frame_id);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::load_status &msg, uint8_t frame_id)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg,frame_id);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::alert_msg &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}

int FrontCommunication::Sender::SendMessage(const FrontCommunication::message::get_log_response &msg)
{
    frontCommunication->tx_buffer_mutex.lock();
    auto bytes_to_write = message::packMessage(frontCommunication->tx_buffer,msg);
    auto res = SendMessage(frontCommunication->tx_buffer,bytes_to_write);
    frontCommunication->tx_buffer_mutex.unlock();
    return res;
}


int FrontCommunication :: StopConnection(void)
{
    // close the socket
    EventLogger :: SaveEvent() << "FrontCommunication: closing frontend connection";
    close(sockfd);
    close(connfd);

    return 1;
}

char *FrontCommunication::message::pack_double(const double &val, char *buff)
{
    auto val_int = reinterpret_cast<const uint64_t*>(&val);
    for(int8_t i=56; i >= 0; i-=8)
    {
        *buff = static_cast<char>( (*val_int & (0xFFUL << i)) >> i);
        buff++;
    }
    return buff;
}

double FrontCommunication::message::unpack_double(const char *buff)
{
    uint64_t res=0;
    for(int8_t i=56; i >= 0; i-=8)
    {
        res |= ((static_cast<uint64_t>(*buff) & 0xFFUL) << i);
        buff ++;
    }
    return *reinterpret_cast<double*>(&res);
}

char *FrontCommunication::message::pack_int32(const int32_t &val, char *buff)
{
    for(int8_t i=24; i >= 0; i-=8)
    {
        *buff = static_cast<char>((val & (0xFF << i)) >> i);
        buff++;
    }
    return buff;
}

int32_t FrontCommunication::message::unpack_int32(const char *buff)
{
    int32_t res=0;
    for(int8_t i=24; i >= 0; i-=8)
    {
        res |= ((static_cast<int32_t>(*buff) & 0xFF) << i);
        buff ++;
    }
    return res;
}

char *FrontCommunication::message::pack_int16(const int16_t &val, char *buff)
{
    for(int8_t i=8; i >= 0; i-=8)
    {
        *buff = static_cast<char>((val & (0xFF << i)) >> i);
        buff++;
    }
    return buff;
}

int16_t FrontCommunication::message::unpack_int16_t(const char *buff)
{
    int16_t res=0;
    for(int8_t i=8; i >= 0; i-=8)
    {
        res |= ((static_cast<int16_t>(*buff) & 0xFF) << i);
        buff ++;
    }
    return res;
}

char *FrontCommunication::message::pack_device_info(const FrontCommunication::message::dev_info &dinfo, char *buff)
{
    char *p = message::pack_int16(static_cast<int16_t>(dinfo.device_ID),buff);
    p = message::packString(dinfo.device_version,p);
    p = message::packString(dinfo.device_IP,p);
    memcpy(p,dinfo.MAC,18); p += 18;
    p = message::pack_double(dinfo.longitude,p);
    p = message::pack_double(dinfo.latitude,p);
    p = message::pack_int16(static_cast<int16_t>(dinfo.height),p);
    *p = dinfo.connected; p++;
    *p = dinfo.health; p++;

    p = message::pack_int16(static_cast<int16_t>(dinfo.data_port),p);
    p = message::pack_int16(static_cast<int16_t>(dinfo.control_port),p);
    p = message::packString(dinfo.creation_date,p);
    p = message::packString(dinfo.expiration_date,p);
    p = message::packString(dinfo.code,p);
    p = message::packString(dinfo.who,p);
    *p = dinfo.firstmess; p++;
    *p = static_cast<uint8_t>(dinfo.card_name.size()); p++;
    for(auto & card_name : dinfo.card_name){
        p = message::packString(card_name,p);
    }

    return p;
}

const char * FrontCommunication::message::unpack_device_info(const char *buff, FrontCommunication::message::dev_info *dinfo)
{
    const char * p = buff;
    dinfo->device_ID = static_cast<uint16_t>(unpack_int16_t(p)); p += sizeof(uint16_t);
    p = unpack_string(p,&dinfo->device_version);
    p = unpack_string(p,&dinfo->device_IP);
    memcpy(dinfo->MAC,p,18); dinfo->MAC[17] = 0; p += 18;
    dinfo->longitude = unpack_double(p); p += sizeof(double);
    dinfo->latitude = unpack_double(p); p += sizeof(double);
    dinfo->height = static_cast<uint16_t>(unpack_int16_t(p)); p += sizeof(uint16_t);
    dinfo->connected = *p; p++;
    dinfo->health = *p; p++;
    dinfo->data_port = static_cast<uint16_t>(message::unpack_int16_t(p)); p += sizeof(uint16_t);
    dinfo->control_port = static_cast<uint16_t>(message::unpack_int16_t(p)); p += sizeof(uint16_t);
    p = unpack_string(p,&dinfo->creation_date);
    p = unpack_string(p,&dinfo->expiration_date);
    p = unpack_string(p,&dinfo->code);
    p = unpack_string(p,&dinfo->who);
    dinfo->firstmess = *p; p++;
    uint8_t cards = *p; p++;
    dinfo->card_name.resize(cards);
    for(uint8_t i=0;i<cards;++i){
        p = unpack_string(p,&dinfo->card_name[i]);
    }

    return p;
}

char *FrontCommunication::message::packString(const std::string &str,char *buff)
{
    uint8_t n = str.size();
    *buff = n + 1; buff++;
    memcpy(buff,str.c_str(),n);
    buff[n] = 0;
    return &buff[n+1];
}

const char *FrontCommunication::message::unpack_string(const char *buff, std::string *output)
{
    uint8_t n = *buff; buff++;
    const char * end = buff + n;
    output->assign(buff, end-1);
    return const_cast<char*>(end);
}

char *FrontCommunication::message::pack_operation_response(const device_operation_response & response, char *buff)
{
    buff = message::pack_int16(static_cast<int16_t>(response.device_ID),buff);
    *buff = response.status; buff++;

    return buff;
}

const char *FrontCommunication::message::unpack_operation_response(const char *buff, FrontCommunication::message::device_operation_response *response)
{
    response->device_ID = static_cast<uint16_t>(message::unpack_int16_t(buff)); buff += sizeof(uint16_t);
    response->status = *buff; buff++;
    return buff;
}

FrontCommunication::message::alert_msg::alert_msg(uint16_t alert_ID) :
    alert_ID(alert_ID)
{
    auto current_time = time(NULL);
    this->timestamp = ctime(&current_time);
}
