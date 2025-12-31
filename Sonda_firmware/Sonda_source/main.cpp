#include <iostream>
#include "RadioCard.h"


using namespace std;
RadioCard radioCard;


int main(void) 
{
    system("gpio -g mode 4 out");
    usleep(500000);

    system("gpio -g write 4 0");
    usleep(500000);

    system("gpio -g write 4 1");
    usleep(500000);

    radioCard.Init();
    radioCard.SetConfiguration("./device_config_files/radio_config_files/radiocard1/radiocard1_v1.0_20.02.2020.cfg");

    return 1;
} 

