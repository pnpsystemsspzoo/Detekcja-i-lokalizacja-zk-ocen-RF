#include <iostream>
#include <vector>
#include "Device.h"
#include "DeviceDataBase.h"
#include "UserInterface.h"
#include "Settings.h"
#include "EventLogger.h"
#include "ActiveThreatsManager.h"

#define MAXdevices 1000

using namespace std;

int main()
{
    // load settings
    if(!Settings::load("settings.json"))
    {
        printf("Failed to load settings.json file: %s", Settings::getErrorMessage());
        exit(1);
    }

    ActiveThreatsManager atm; atm.start();
    DeviceDataBase deviceDataBase;
    vector<Device> device;

    device.reserve(MAXdevices);
    if(deviceDataBase.numberOfElementsInDB > 0)
    {
        for(int i = 1; i < MAXdevices; i++)
        {
            if(deviceDataBase.CheckSelectedDeviceExists(i) == true)
            {
                device.emplace_back(i, &deviceDataBase, &atm);
            }
        }
    }

    UserInterface ui(&deviceDataBase, &device, &atm);
    ui.StartFrontConnection(
            Settings::UserInterface.TCP_server_ip.c_str(),
            Settings::UserInterface.TCP_server_port,
            Settings::UserInterface.share_path.c_str());

    usleep(1000000);


    while(1);

    return 1;
}

