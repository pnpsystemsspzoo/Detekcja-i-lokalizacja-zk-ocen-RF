#include "UserInterface.h"
#include "Settings.h"
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

using namespace std;

#define DATE_STR_LENGTH (sizeof "2011-10-08T07:07:09Z")
#define DATE_ISO_STR_FORMAT "%Y-%m-%dT%H:%M:%SZ"
#define VOLTS_TO_MILIVOLTS(X)  (static_cast<uint16_t>((X)*1000))
#define AMPERS_TO_MILAMPERS(X)  (static_cast<uint16_t>((X)*1000))

Device *UserInterface::findDeviceByID(uint16_t device_id)
{
    for(int i=0; i< devices->size(); ++i){
        if(devices->at(i).GetIndex() == device_id)
        {
            // found
            return &devices->at(i);
        }
    }
    return nullptr;
}

bool UserInterface::checkDirExists(const char *dir)
{
    DIR* dirp = opendir(dir);
    if(dirp == NULL)
    {
        return false;
    }
    closedir(dirp);
    return true;
}

bool UserInterface::removeDir(const char *dirname)
{
    DIR *dir;
    struct dirent *entry;
    char * path = new char[PATH_MAX];

    if (path == NULL) {
        fprintf(stderr, "Out of memory error\n");
        return false;
    }
    dir = opendir(dirname);
    if (dir == NULL) {
        delete [] path;
        return false;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
            if (entry->d_type == DT_DIR) {
                if(!removeDir(path)){
                    // error
                    EventLogger :: SaveEvent() << "UserInterface.removeDir: cannot remove directory '" + std::string(dirname) + "'. " + string(strerror(errno));
                    delete [] path;
                    closedir(dir);
                    return false;
                };
            } else {
                if(remove(path) == -1)
                {
                    // error while removing
                    delete [] path;
                    closedir(dir);
                    return false;
                }
            }
        }

    }
    delete [] path;
    closedir(dir);

    if(remove(dirname) == -1)
    {
        // error while removing
        return false;
    }

    return true;
}

void UserInterface::createProbeDir(uint16_t device_ID)
{
    auto probe_dir = this->share_dir + "/probe" + to_string(device_ID);

    if(UserInterface::checkDirExists(probe_dir.c_str())) return;
    createDir(probe_dir.c_str());
}

void UserInterface::createProbesDirectories()
{
    for(auto & dev : *this->devices){
        this->createProbeDir(dev.GetIndex());
    }
}

UserInterface :: UserInterface(DeviceDataBase *db, vector<Device> *devices, ActiveThreatsManager *atm) :
    frontCommunication(),
    OPERATOR_INTERFACE_QUERY_received(false),
    loop_message_CS_STATUS(false),
    loop_message_DEVICE_SHORT_STATUS(false),
    send_message_DEVICE_LIST(false),
    loop_message_DEVICE_FULL_STATUS(false),
    send_message_FFT(false),
    loop_message_FFT(false),
    send_message_ADD_DEVICE(false),
    send_message_DELETE_DEVICE(false),
    send_message_CONFIGURATION_LOAD_STATUS(false),
    send_message_FIRMWARE_LOAD_STATUS(false),
    uploadMonitor(this),
    device_db(db),
    devices(devices),
    active_threats_manager(atm)
{
    frontCommunication.addListener(this);
    for(auto & dev : *devices){
        dev.setSender(this->getSender());
    }

    const char * key_path = Settings::Encryptor.key_path.c_str();
    if(!licenseParser.setKeyFromFile(key_path)){
        EventLogger :: SaveEvent() << "UserInterface: cannot load license key at given path: " + string(key_path);
    }
}


int UserInterface :: StartFrontConnection(const char * ip_address, long port, const char * share_path)
{
    EventLogger :: SaveEvent() << "Starting frontend connection with settings\n\t  - server address: " + string(ip_address) + ":" + to_string(port) + "\n\t  - frontend share folder dir: " + string(share_path);

    // run TCP server threads
    frontCommunication.port = port;
    frontCommunication.ipaddr = ip_address;

    thread communicationWorkerInData(&UserInterface :: FrontCommunicationInterfaceInData, this);
    communicationWorkerInData.detach();

    thread communicationWorkerOutData(&UserInterface :: FrontCommunicationInterfaceOutData, this);
    communicationWorkerOutData.detach();
    
    // run UploadMonitor
    this->share_dir = share_path;
    this->uploadMonitor.setEntryDir((std::string(share_path) + "/upload").c_str());
    this->uploadMonitor.setFirmwareDir((std::string(share_path) + "/firmware").c_str());
    this->uploadMonitor.setConfigDir((std::string(share_path) + "/config").c_str());
    this->uploadMonitor.start();

    this->createProbesDirectories();

    return 1;
}


int UserInterface :: FrontCommunicationInterfaceInData(void)
{
    int RWresult;
    
    do
    {
        while(!frontCommunication.isListening())
        {
            if(!frontCommunication.InitConnection())
            {
                EventLogger :: SaveEvent() << "UserInterface: Failed to create TCP server. Retrying again in 5 seconds.";
                sleep(5);
            }
        }

        if(frontCommunication.AcceptClientMessage() >= 0)
        {    
            do
            {
                bzero(InBuff, InBuffSize);
                RWresult = frontCommunication.ReceiveMessage(InBuff);
                if(RWresult > 0)
                {
                    // new message received from frontend user
                    frontCommunication.ParseUserMessage(InBuff);
                }
                usleep(1000);

            }
            while(RWresult >= 0);
        }   
    }
    while(1);


    return 1;
}


struct ri_mock {
    FrontCommunication::message::radio_interference ri;
    FrontCommunication * fc;
    int state;
    unsigned int counter;
    unsigned int transmit_interval;
    unsigned int wait_interval;

    ri_mock(FrontCommunication * fc){
        this->fc = fc;
        state = 0;
        counter = 0;
    }
};

void mockRadioInterference(ri_mock * data){
    switch(data->state){
        case 0:
            // mock values
            data->ri.heading = (std::rand() % 360);
            data->ri.height = (std::rand() % 1000) + 100;
            data->ri.latitude = 50.1 + static_cast<double>(std::rand() % 10000)/1000000.0;
            data->ri.longitude = 21.98 + static_cast<double>(std::rand() % 80000)/8000000.0;
            data->ri.range = 50 + (std::rand() % 500);
            data->ri.signal_ID = static_cast<uint32_t>(std::rand());
            data->ri.speed = 10 + (std::rand() % 100);

            data->counter = 0;
            data->wait_interval = (std::rand() % 100); // 10 sec max
            data->transmit_interval = data->wait_interval + 30 + (std::rand() % 70); // 3sec min 10 sec max
            data->state = 1;
        break;
        case 1:
            // wait
            if(data->counter > data->wait_interval) data->state = 2;
        break;
        case 2:
            // transmit
            if(data->fc->isClientConneted()) data->fc->sender.SendMessage(data->ri);

            // mutate some values sligthly
            data->ri.heading = static_cast<uint16_t>((data->ri.heading + ((std::rand() % 4) - 2)) % 360);
            data->ri.height = static_cast<uint16_t>((data->ri.height + ((std::rand() % 6) - 3)));
            data->ri.range = static_cast<uint16_t>((data->ri.range + ((std::rand() % 10) - 5)));
            if(data->ri.range < 20) data->ri.range = 20;
            data->ri.speed = static_cast<uint16_t>((data->ri.speed + ((std::rand() % 6) - 3)));
            if(data->ri.speed < 5) data->ri.speed = 5;
            data->ri.latitude += ((std::rand() % 200) - 100)/1000000.0;
            data->ri.longitude += ((std::rand() % 1600) - 800)/8000000.0;

            if(data->counter > data->transmit_interval) data->state = 0;
        break;
    }
    data->counter++;
}

int UserInterface :: FrontCommunicationInterfaceOutData()
{
    int interval_us = 100000; // 100 ms

    //ri_mock ri_1(&this->frontCommunication);
    //ri_mock ri_2(&this->frontCommunication);
    //ri_mock ri_3(&this->frontCommunication);

    unsigned int counter = 0;

    while(1)
    {
        counter++;
        for(int signalType = ThreatClassifier::threat_type_t::UNKNOWN; signalType != ThreatClassifier::threat_type_t::SIGNAL_TYPE_RESERVED; signalType++)
        {
            auto threats = active_threats_manager->getActiveThreats((ThreatClassifier::threat_type_t)signalType);
            for(auto threat : threats)
            {
                //EventLogger::SaveEvent() << "Sending interference data. ThreatID:"+to_string(threat->getUniqueId());
                FrontCommunication::message::radio_interference ri;
                ri.latitude = threat->position.latitude;
                ri.longitude = threat->position.longitude;
                ri.range = 250;
                ri.heading =  0;
                ri.height = 100;
                ri.speed = 200;
                ri.signal_ID = (uint32_t)threat->getUniqueId();
                if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(ri);
            }
        }
        //mockRadioInterference(&ri_1);
        //mockRadioInterference(&ri_2);
        //mockRadioInterference(&ri_3);

        // every 5 sec
        if((counter % 50) == 0) {
            FrontCommunication::message::alert_msg am;
            char tmp_buff[20];
            sprintf(tmp_buff,"%d",(counter / 50));
            am.alert_ID = (counter / 50);
            am.timestamp = "TEST_TIMESTAMP_";
            am.timestamp += tmp_buff;
            am.message = "This is alert test message ";
            am.message += tmp_buff;

            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(am);

        }


        if(OPERATOR_INTERFACE_QUERY_received)
        {
            OPERATOR_INTERFACE_QUERY_received = false;

        }
        if(loop_message_CS_STATUS && ((counter % 2) == 0))
        {
            FrontCommunication::message::cs_status cs;
            cs.health = 0x0F;
            cs.communication_status = 1;
            cs.CPU_usage = 67;
            cs.disk_usage = 50;
            cs.health = 1;
            cs.RAM_usage = 75;
            cs.tasks_number = 524;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(cs);
        }
        if(loop_message_DEVICE_SHORT_STATUS && ((counter % 2) == 0))
        {
            short_status_device_id++;
            if(short_status_device_id >= devices->size()) short_status_device_id = 0;
            auto &dev = devices->at(short_status_device_id);

            FrontCommunication::message::dev_short_status ds;
            ds.device_ID = dev.GetIndex();
            ds.health = (dev.statusMessage.systemHealthStatus << 2);
            ds.health |= (dev.statusMessage.boardHealthStatus & 0x0F);
            ds.communication_status = dev.statusMessage.softwareHealthStatus;
            ds.radio_interference = 0x0D;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(ds);
        }

        if(send_message_DEVICE_LIST)
        {
            send_message_DEVICE_LIST = false;

            FrontCommunication::message::dev_info di[2];
            di[0].device_ID = 1;
            di[0].device_version = "v1";
            di[0].device_IP = "192.168.24.154";
            memcpy(di[0].MAC,"00:0A:E6:3E:FD:E1",17); di[0].MAC[17] = 0;
            di[0].longitude = 22.014112;
            di[0].latitude = 50.117504;
            di[0].height = 7;
            di[0].connected = 1;
            di[0].health = 0x0B;

            di[1].device_ID = 2;
            di[1].device_version = "v2";
            di[1].device_IP = "192.168.24.155";
            memcpy(di[1].MAC,"00:11:22:33:44:55",17); di[1].MAC[17] = 0;
            di[1].longitude = 22.014115;
            di[1].latitude = 50.117527;
            di[1].height = 5;
            di[1].connected = 1;
            di[1].health = 0x0A;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(di,2);

        }
        if(loop_message_DEVICE_FULL_STATUS && ((counter % 2) == 0))
        {
            auto dev = findDeviceByID(full_status_device_id);
            if(dev){
                FrontCommunication::message::dev_full_status s;
                s.device_ID = full_status_device_id;
                s.health = (dev->statusMessage.boardHealthStatus << 4);
                s.health |= (dev->statusMessage.systemHealthStatus & 0x0F);
                s.motherboard_voltage = VOLTS_TO_MILIVOLTS(dev->statusMessage.mainVoltagePoints[0]);
                s.motherboard_current = AMPERS_TO_MILAMPERS(dev->statusMessage.mainCurrentPoints[0]);
                s.cards = dev->deviceParams.cards;
                s.cards_health = 0;
                for(uint8_t i=0; i<s.cards;++i)
                {
                    s.card_voltage[i] = VOLTS_TO_MILIVOLTS(dev->statusMessage.mainVoltagePoints[i+1]);
                    s.card_current[i] = AMPERS_TO_MILAMPERS(dev->statusMessage.mainCurrentPoints[i+1]);
                    s.cards_health |= ((dev->statusMessage.radioCardsHealthStatus[i] & 0x03) << 2*i);
                }
                s.alerts = dev->statusMessage.alertsNumber;
                s.radio_interference = dev->statusMessage.noiseDiscovered;
                s.communication_status = dev->statusMessage.softwareHealthStatus; // dev->deviceCommunication.isConnected() ? 0 : 2; // TODO: Where to get proper communication_status value? (now its glued with isConnected)
                s.connected = 1; // dev->deviceCommunication.isConnected() ? 1 : 0; // TODO: there is no function to get this
                s.CPU_usage = dev->statusMessage.CPUusage;
                s.RAM_usage = dev->statusMessage.RAMusage;
                s.disk_usage = dev->statusMessage.diskSpaceUsage;
                s.tx_transfer_rate = dev->statusMessage.txTransmit;
                s.rx_transfer_rate = dev->statusMessage.rxTransmit;
                s.tasks_number = static_cast<uint16_t>(dev->statusMessage.tasksNumber);
                s.synchro_status = 1; // TODO: How to get this value?
                s.time_std = 17; // TODO: How to get this value?
                memcpy(s.datetime,"27.03.2015, 14:25:05:365",25); // TODO: How to get this value?
                s.threats_length = 2; // TODO: How to get this value and how to fill ralated threats info below?
                s.threats_table.reset(new FrontCommunication::message::threat_info[2]);
                s.threats_table.get()[0].bandwidth = 20;
                s.threats_table.get()[0].f0 = 35000;
                s.threats_table.get()[0].threat_type = 1;
                s.threats_table.get()[1].bandwidth = 50;
                s.threats_table.get()[1].f0 = 25000;
                s.threats_table.get()[1].threat_type = 2;

                if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(s);

                //printf("\nIDf: %d    IDb: %d   TX: %d   CPU: %d",  full_status_device_id, dev->GetIndex(), dev->statusMessage.txTransmit, dev->statusMessage.CPUusage);
            }

        }
        if(send_message_FFT)
        {
            if(!loop_message_FFT)
            {
                send_message_FFT = false;
            }

            FrontCommunication::message::fft fft;
            fft.device_ID = 1;
            fft.channel = 2;
            fft.f_begin = 34000;
            fft.f_end = 35000;
            fft.data_number = 128;
            fft.power_density.reset(new int16_t[128]);
            for(uint16_t i=0;i<fft.data_number;i++) {
                fft.power_density.get()[i] = static_cast<int16_t>(std::rand() % 0xFFFF);
            }
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(fft);

        }

        if(send_message_ADD_DEVICE)
        {
            send_message_ADD_DEVICE = false;

            FrontCommunication::message::add_device ad;
            ad.status = 1;
            ad.device_info.device_ID = 1;
            ad.device_info.device_version = "v1";
            ad.device_info.device_IP = "192.168.24.154";
            memcpy(ad.device_info.MAC,"00:0A:E6:3E:FD:E1",17); ad.device_info.MAC[17] = 0;
            ad.device_info.longitude = 22.014112;
            ad.device_info.latitude = 50.117504;
            ad.device_info.height = 7;
            ad.device_info.connected = 1;
            ad.device_info.health = 0x0B;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(ad);
        }

        if(send_message_DELETE_DEVICE)
        {
            send_message_DELETE_DEVICE = false;
            FrontCommunication::message::delete_device dd;
            dd.device_ID = 2;
            dd.status = 1;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(dd);

        }

        if(send_message_CONFIGURATION_LOAD_STATUS)
        {
            send_message_CONFIGURATION_LOAD_STATUS = false;

            FrontCommunication::message::load_status cls;
            cls.filename = "TEST.zip";
            cls.device_length = 2;
            cls.device_status.reset(new FrontCommunication::message::device_operation_response[2]);
            cls.device_status.get()[0].device_ID = 1;
            cls.device_status.get()[0].status = 0;
            cls.device_status.get()[1].device_ID = 2;
            cls.device_status.get()[1].status = 1;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(cls,FrontCommunication::message::CONFIGURATION_LOAD_STATUS);
        }

        if(send_message_FIRMWARE_LOAD_STATUS)
        {
            send_message_FIRMWARE_LOAD_STATUS = false;

            FrontCommunication::message::load_status fls;
            fls.filename = "FIRMWARE.zip";
            fls.device_length = 2;
            fls.device_status.reset(new FrontCommunication::message::device_operation_response[2]);
            fls.device_status.get()[0].device_ID = 1;
            fls.device_status.get()[0].status = 1;
            fls.device_status.get()[1].device_ID = 2;
            fls.device_status.get()[1].status = 1;
            if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(fls,FrontCommunication::message::FIRMWARE_LOAD_STATUS);
        }


        usleep(interval_us);

    }
    
    return 1;
}

void UserInterface::onOperatorInterfaceQuery(uint8_t user_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message OPERATOR_INTERFACE_QUERY from user id: " + to_string(user_id);

    OPERATOR_INTERFACE_QUERY_received = true;
    loop_message_CS_STATUS = true;
    loop_message_DEVICE_SHORT_STATUS = true;

}

void UserInterface::onAdminInterfaceQuery(uint8_t user_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message ADMIN_INTERFACE_QUERY from user id: " + to_string(user_id);
}

void UserInterface::onDevicesManagementQuery(uint8_t user_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message DEVICES_MANAGEMENT_QUERY from user id: " + to_string(user_id);

    // count available devices
    int n = 0;
    for(int i=0; i< devices->size();++i)
    {
        if(devices->at(i).isEmpty()) continue;
        n++;
    }
    // handle special case -> there is no device at all
    if(n == 0){
        // send empty
        if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(static_cast<FrontCommunication::message::dev_info*>(nullptr),0);
        return;
    }

    // allocate descriptors buffer
    auto di = new FrontCommunication::message::dev_info[n];
    int j=0;
    n = devices->size();
    for(int i=0; i < n; i++){
        // skip empty cache items
        if(devices->at(i).isEmpty()) continue;
        auto & dev = devices->at(i);

        di[j].device_ID = dev.GetIndex();
        di[j].device_version = "unknown";
        di[j].device_IP = dev.deviceParams.ip_address;
        auto mac_size = dev.deviceParams.mac_addr.size();
        memcpy(di[j].MAC,dev.deviceParams.mac_addr.c_str(), mac_size <= 17 ? mac_size : 17); di[j].MAC[17] = 0;
        di[j].longitude = dev.deviceParams.longitude;
        di[j].latitude = dev.deviceParams.latitude;
        di[j].height = dev.deviceParams.height;
        di[j].connected = dev.statusMessage.softwareHealthStatus ? 1 : 0;
        di[j].health = dev.statusMessage.softwareHealthStatus;
        di[j].data_port = dev.deviceParams.data_port;
        di[j].control_port = dev.deviceParams.control_port;
        di[j].creation_date = std::ctime(&dev.deviceParams.creation_date);
        di[j].expiration_date = std::ctime(&dev.deviceParams.expiration_date);
        di[j].code = dev.deviceParams.code;
        di[j].who = dev.deviceParams.who;
        for(uint8_t k=0;k<dev.deviceParams.cards;++k){
            di[j].card_name.push_back(dev.deviceParams.card_name[k]);
        }
        di[j].firstmess = static_cast<uint8_t>(dev.deviceParams.firstmess);

        j++;
    }

    // send message
    if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(di,j);

    // free helper buffer
    delete [] di;

}

void UserInterface::onDeviceStatusQuery(uint8_t user_id, uint16_t device_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message DEVICES_STATUS_QUERY from user id: "+ to_string(user_id) + "; device_id: " + to_string(device_id);
    auto dev = this->findDeviceByID(device_id);
    if(dev) {
        //dev->GetStatus();
        this->full_status_device_id = device_id;
        loop_message_DEVICE_FULL_STATUS = true;
    } else {
        EventLogger::SaveUserEvent(user_id) << "Error: cannot retrive data from device with id " + to_string(device_id) + ". Device not found";
        // device not found
        loop_message_DEVICE_FULL_STATUS = false;
    }
}


void UserInterface::onAddDeviceQuery(uint8_t user_id, const char *license_buffer, uint32_t license_size)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message ADD_DEVICE_QUERY from user id: " + to_string(user_id);
    // validate license
    Parser::config_set decoded;
    FrontCommunication::message::add_device msg_response;
    msg_response.status = 0;

    if(licenseParser.decryptBuffer(license_buffer,license_size,&decoded))
    {
        // license validated
        // add this device into database
        auto device_id = device_db->AddNewDevice(license_buffer,license_size);
        if(device_id >= 0)
        {
            // successfully added new device into database

            // create local cache
            devices->emplace_back(device_id,device_db,this->active_threats_manager);
            auto & device = devices->back();


            // prepare frontend user response
            // TODO: fill missing fields like device_version
            msg_response.status = 0; // set status to failed temporally. Will be set to success if can save license on hard disk
            auto & di = msg_response.device_info;
            di.connected = 0; // not connected yet
            di.device_ID = device_id;
            di.device_IP = "0:0:0:0"; // not connected yet
            di.device_version = "unkown";
            di.health = 0; // all down / not launched yet
            di.height = device.deviceParams.height;
            di.latitude = device.deviceParams.latitude;
            di.longitude = device.deviceParams.longitude;
            di.data_port = device.deviceParams.data_port;
            di.control_port = device.deviceParams.control_port;
            di.creation_date = std::ctime(&device.deviceParams.creation_date);
            di.expiration_date = std::ctime(&device.deviceParams.expiration_date);
            di.code = device.deviceParams.code;
            di.who = device.deviceParams.who;
            for(uint8_t i=0;i<device.deviceParams.cards;++i)
            {
                di.card_name.push_back(device.deviceParams.card_name[i]);
            }
            di.firstmess = static_cast<uint8_t>(device.deviceParams.firstmess);

            auto mac_size = device.deviceParams.mac_addr.size();
            memcpy(msg_response.device_info.MAC,device.deviceParams.mac_addr.c_str(), mac_size <= 17 ? mac_size : 17);
            msg_response.device_info.MAC[17] = '\0';

            // create new folder for this probe inside 'share' directory
            std::string probe_dir = this->share_dir + "/probe" + std::to_string(device_id);

            // TODO: create separate method for the code below inside UploadMonitor
            if(this->createDir(probe_dir.c_str()) == -1)
            {
                // creating folder for the new probe failed
                EventLogger :: SaveEvent() << "UserInterface.onAddDeviceQuery: cannot create directory for new probe. Command 'mkdir " + string(probe_dir.c_str()) + "' failed with the error: " + string(strerror(errno));
            } else
            {
                // direcotry created
                msg_response.status = 1;

                // put encoded license file there
                std::string license_path = probe_dir + "/license.bin";
                if(!Encryptor::saveBufferToFile(const_cast<char*>(license_buffer),license_size,license_path.c_str()))
                {
                    // writing license file into filesystem failed
                    msg_response.status = 0;
                    EventLogger :: SaveEvent() << "UserInterface.onAddDeviceQuery: cannot write new license into filesystem at directory " + string(license_path.c_str());
                }

                // put empty configuration list
                nlohmann::json config_json;
                config_json["config_array"] = nlohmann::json::array();
                this->uploadMonitor.createJSONfile((std::string(probe_dir) + "/config_list.json").c_str(),config_json);

                // put empty logs list
                nlohmann::json logs_json;
                logs_json["logs_array"] = nlohmann::json::array();
                this->uploadMonitor.createJSONfile((std::string(probe_dir) + "/logs_list.json").c_str(),logs_json);
            }
        }
    }
    if(frontCommunication.isClientConneted()) this->frontCommunication.sender.SendMessage(msg_response);
}

void UserInterface::onDeleteDeviceQuery(uint8_t user_id, uint16_t device_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message DELETE_DEVICE_QUERY from user id: " + to_string(user_id) + "; device_id: " + to_string(device_id);
    FrontCommunication::message::delete_device dd;
    dd.device_ID = device_id;
    auto dev = findDeviceByID(device_id);
    if(dev == nullptr){
        // device with the specified id not found in cache
        dd.status = 0; // mark status fail
    }else {
        dd.status = (dev->DeleteData(device_id) < 0) ? 0 : 1;
        if(dd.status)
        {
            // deleting device from the database succeeded
            // now delete local folder with this probe

        }
    }
    // send message to the frontend
    if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(dd);
}

void UserInterface::onFFTQuery(uint8_t user_id, uint16_t device_id, uint8_t channel, bool repeat)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message FFT_QUERY from user id: " + to_string(user_id) + "; device_id: " + to_string(device_id) + "; channel: " + to_string(channel) + "; repeat: " + (repeat ? "true" : "false");
    auto dev = findDeviceByID(device_id);
    if(dev) {
        if(repeat) {
            //dev->StartStreamRawData();
            loop_message_FFT = true;
        }else{
            //dev->StopStreamRawData();
            loop_message_FFT = false;
        }
    } else {
        // device not found
    }
}

void UserInterface::onStopDeviceStatusQuery(uint8_t user_id, uint16_t device_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message STOP_DEVICE_STATUS_QUERY from user id: " + to_string(user_id) + "; device_id: " + to_string(device_id);
    loop_message_DEVICE_FULL_STATUS = false;
}

void UserInterface::onUpdateDevicesConfigQuery(uint8_t user_id, const string &config_filename, const std::vector<uint16_t> &device_ID)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message UPDATE_DEVICES_CONFIG_QUERY from user id: " + to_string(user_id) + "; config_filename: " + string(config_filename.c_str());
    uint16_t n = device_ID.size();

    FrontCommunication::message::load_status cls;
    cls.filename = config_filename;
    cls.device_length = n;
    cls.device_status.reset(new FrontCommunication::message::device_operation_response[n]);

    std::string file_path = this->uploadMonitor.getConfigDir() + "/" + config_filename;

    for (uint16_t dev_id = 0; dev_id< n;++dev_id)
    {
        auto & dev_info = cls.device_status.get()[dev_id];

        dev_info.device_ID = dev_id;
        dev_info.status = 0; // mark status fail as default

        auto dev = this->findDeviceByID(dev_id);
        if(dev == nullptr){
            EventLogger :: SaveEvent() << "WARNING: Trying to update configuraton file '" + config_filename + "' for the non-existing device with id " + to_string(dev_id);

        }
        auto res = dev->PostConfigFiles(file_path);
        if(res >=0 ){
            // success
            dev_info.status = 1;
        }
    }
    if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(cls,FrontCommunication::message::CONFIGURATION_LOAD_STATUS);
}

void UserInterface::onUpdateDevicesFirmwareQuery(uint8_t user_id, const string &config_filename, const std::vector<uint16_t> &device_ID)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message UPDATE_DEVICES_FIRMWARE_QUERY from user id: " + to_string(user_id) + "; firmware_filename: " + string(config_filename.c_str());
    uint16_t n = device_ID.size();

    FrontCommunication::message::load_status cls;
    cls.filename = config_filename;
    cls.device_length = n;
    cls.device_status.reset(new FrontCommunication::message::device_operation_response[n]);

    std::string file_path = this->uploadMonitor.getFirwareDir() + "/" + config_filename;

    for (uint16_t dev_id = 0; dev_id< n;++dev_id)
    {
        auto & dev_info = cls.device_status.get()[dev_id];

        dev_info.device_ID = dev_id;
        dev_info.status = 0; // mark status fail as default

        auto dev = this->findDeviceByID(dev_id);
        if(dev == nullptr){
            EventLogger :: SaveEvent() << "WARNING: Trying to update configuraton file '" + config_filename + "' for the non-existing device with id " + to_string(dev_id);

        }
        auto res = dev->SoftwareUpgrade(file_path);
        if(res >=0 ){
            // success
            dev_info.status = 1;
        }
    }
    if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(cls,FrontCommunication::message::FIRMWARE_LOAD_STATUS);
}

void UserInterface::onResetDeviceQuery(uint8_t user_id, uint16_t device_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message RESET_DEVICE_QUERY from user id: " + to_string(user_id) + "; device_id: " + to_string(device_id);
    typedef FrontCommunication::message::alert_msg alert;
    auto dev = findDeviceByID(device_id);
    alert am(alert::RESET_DEVICE);


    if(dev != nullptr){
        auto res = dev->Restart();
        if(res < 0){
            // Failure during device restart
            am.alert_ID |= alert::ERROR;
            am.message = "Reseting device failed with error code " + std::to_string(res);
        } else {
            // all good
            am.alert_ID |= alert::INFO;
            am.message = "Successfully reset device with id " + std::to_string(device_id);
        }
    } else {
        // device not found
        am.alert_ID |= alert::ERROR;
        am.message = "Trying to reset device with id " + std::to_string(device_id) + " that does not exist";
    }
    if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(am);
}

void UserInterface::onGetLogQuery(uint8_t user_id, uint16_t device_id)
{
    EventLogger::SaveUserEvent(user_id) << "Received frontend message GET_LOG_QUERY from user id: "+ to_string(user_id) + "; device_id: " + to_string(device_id);
    auto dev = this->findDeviceByID(device_id);
    if(dev) {
        // Device found
        dev->GetEventsList();
    } else {
        // device not found
        EventLogger::SaveUserEvent(user_id) << "Error: requested log file cannot be downloaded. Device with id " + to_string(device_id) + " not found";
        FrontCommunication::message::get_log_response response;
        response.device_ID = device_id;
        response.log_file_name = "unknown";
        response.status = 1; // set status to error
        if(frontCommunication.isClientConneted()) frontCommunication.sender.SendMessage(response);
    }
}




int UserInterface::UploadMonitor::worker()
{

    std::string firmware_upload_dir = this->entry_dir;
    firmware_upload_dir += "/firmware";

    std::string config_upload_dir = this->entry_dir;
    config_upload_dir += "/config";

    EventLogger :: SaveEvent() << "UserInterface::UploadMonitor is watching directories:\n\t  - firmware: " + string(firmware_upload_dir.c_str()) + "\n\t  - config: " + string(config_upload_dir.c_str());


    while(this->work)
    {
        // check all files inside upload firmware directory
        this->checkFolder(firmware_upload_dir,this->firmware_dir,FrontCommunication::message::NEW_FIRMWARE_UPLOADED);
        // check all files inside upload configuration directory
        this->checkFolder(config_upload_dir,this->config_dir,FrontCommunication::message::NEW_CONFIGURATION_UPLOADED);

        sleep(1);
    };

    EventLogger :: SaveEvent() << "UploadMonitor: monitor shut down gracefully";

    return 0;
}

bool UserInterface::UploadMonitor::validateFirmware(const char *filename)
{
    // TODO: implement validateFirmware function body
    return true;
}

bool UserInterface::UploadMonitor::validateConfig(const char *filename)
{
    // TODO: implement validateConfig function body
    return true;
}

bool UserInterface::UploadMonitor::readDir(const char *dir, std::vector<string> *result)
{
    // read all files inside entry dir
    result->clear();
    DIR* dirp = opendir(dir);
    if(dirp == NULL)
    {
        return false;
    }
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if(dp->d_name[0] == '.') continue;
        if(strstr(dp->d_name, ".lock") != NULL) continue;
        result->push_back(dp->d_name);
    }
    closedir(dirp);

    return true;
}

void UserInterface::UploadMonitor::createJSONfile(const char *dir, const nlohmann::json &json)
{
    // write prettified JSON to file
    std::ofstream o(dir);
    o << json << std::endl;
}



void UserInterface::UploadMonitor::generateJSONlist(const char *dir, const char * filename)
{

    std::string output_file = dir;
    std::string parent_folder = this->getParentFolder(output_file);
    output_file += "/";
    output_file += filename;

    vector<string> v;
    if(!this->readDir(dir,&v))
    {
        EventLogger :: SaveEvent() << "UserInterface::UploadMonitor: Cannot read directory '" + string(dir) + "' to update file '" + string(filename) + "'. " + string(strerror(errno));
        return;
    }
    vector<string> files;
    std::copy_if(v.begin(), v.end(), std::back_inserter(files), [&filename](string & f){return (strcmp(f.c_str(),filename) && !strstr(f.c_str(),".json"));} );

    ofstream myfile;
    myfile.open (output_file);
    myfile << "{\n  \"" << parent_folder << "_array\": [";
    bool first = true;
    for(auto file : files)
    {
        if(!first) myfile << ",";
        first = false;

        std::string file_path = dir;
        file_path += "/" + file;

        myfile << "\n    {\n";
        myfile << "      \"datetime\": \"" << getFileISOdatetime(file_path.c_str()) << "\",\n";
        myfile << "      \"filename\": \"share/" << parent_folder.c_str() << "/" << file << "\"\n";
        myfile << "    }";

    }
    myfile << "\n  ]\n}";
    myfile.close();

    EventLogger :: SaveEvent() << "UploadMonitor: updated files list: " + string(output_file.c_str());
}

string UserInterface::UploadMonitor::getFileISOdatetime(const char *filepath)
{
    struct stat attr;
    stat(filepath, &attr);
    char date[DATE_STR_LENGTH];
    strftime(date, sizeof date, DATE_ISO_STR_FORMAT, localtime(&(attr.st_ctime)));
    date[DATE_STR_LENGTH-1] = 0;
    date[DATE_STR_LENGTH-2] = 'Z';
    return std::string(date);
}


void UserInterface::UploadMonitor::checkFolder(string &upload_dir, string &destination_dir, int action)
{
    // read all files inside upload directory
    vector<string> file_names;
    if(!this->readDir(upload_dir.c_str(),&file_names))
    {
        EventLogger :: SaveEvent() << "UserInterface::UploadMonitor: Cannot access directory to monitor '" + string(upload_dir.c_str()) + "'. " + string(strerror(errno));
        return;
    }
    bool regenerate_json = false;
    if(file_names.size() > 0)
    {
        // some files has been uploaded
        for(auto file : file_names)
        {
            bool res;
            std::string file_path = upload_dir + "/" + file;
            if(action == FrontCommunication::message::NEW_FIRMWARE_UPLOADED) res = this->validateFirmware(file_path.c_str());
            else res = this->validateConfig(file_path.c_str());

            if(res)
            {
                regenerate_json = true;
                // move the file
                std::string new_location = destination_dir + "/" + file;
                std::rename(file_path.c_str(),new_location.c_str());

                // send message to the frontend
                FrontCommunication::message::new_file_uploaded ffu;
                ffu.user_ID = 0; // TODO: remove user id from this message or implement sending user id somehow from frontend when uploading files
                ffu.status = 1;
                ffu.filename = file;
                if(action == FrontCommunication::message::NEW_FIRMWARE_UPLOADED)
                {
                    EventLogger :: SaveEvent() << "UploadMonitor: new firmware uploaded: " + string(ffu.filename.c_str());
                    if(ui->frontCommunication.isClientConneted()) ui->frontCommunication.sender.SendMessage(ffu,FrontCommunication::message::NEW_FIRMWARE_UPLOADED);
                }else
                {
                    EventLogger :: SaveEvent() << "UploadMonitor: new configuration uploaded: " + string(ffu.filename.c_str());
                    if(ui->frontCommunication.isClientConneted()) ui->frontCommunication.sender.SendMessage(ffu,FrontCommunication::message::NEW_CONFIGURATION_UPLOADED);
                }
            }else
            {
                // file not validated. Remove
                std::remove(file_path.c_str());

                // send message to the frontend
                FrontCommunication::message::new_file_uploaded ffu;
                ffu.user_ID = 0; // TODO: remove user id from this message or implement sending user id somehow from frontend when uploading files
                ffu.status = 0;
                ffu.filename = file;
                if(action == FrontCommunication::message::NEW_FIRMWARE_UPLOADED)
                {
                    EventLogger :: SaveEvent() << "UploadMonitor: rejecting not validated firmware file: " + string(ffu.filename.c_str());
                    if(ui->frontCommunication.isClientConneted()) ui->frontCommunication.sender.SendMessage(ffu,FrontCommunication::message::NEW_FIRMWARE_UPLOADED);
                }else
                {
                    EventLogger :: SaveEvent() << "UploadMonitor: rejecting not validated configuration file: " + string(ffu.filename.c_str());
                    if(ui->frontCommunication.isClientConneted()) ui->frontCommunication.sender.SendMessage(ffu,FrontCommunication::message::NEW_CONFIGURATION_UPLOADED);
                }
            }

        }
        if(regenerate_json)
        {
            std::string file_name = this->getParentFolder(destination_dir);
            file_name += "_list.json";
            this->generateJSONlist(destination_dir.c_str(),file_name.c_str());
        }

    }
}

void UserInterface::UploadMonitor::start()
{
    worker_thread = std::thread(&UserInterface::UploadMonitor::worker,this);
    worker_thread.detach();
}

UserInterface::UploadMonitor::UploadMonitor(UserInterface *ui) :
    ui(ui),
    work(true)
{

}

UserInterface::UploadMonitor::~UploadMonitor()
{
    this->work = false;
    worker_thread.join();
}
