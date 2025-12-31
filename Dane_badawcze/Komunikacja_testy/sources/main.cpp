#include "SystemCommunication.h"
#include "probeMessagesEncoder.h"
#include "Status.h"
#include "Configurator.h"
#include "AXI.h"

//<

#define RFDC_DATA_ADC_ALIGN (256 * 1024)


int main() 
{
        volatile uint32_t * p = (volatile uint32_t*)0xb0000000;

    //Configurator configurator;
    //SystemCommunication systemCommunication;
    //Status status;
    //RadioCard * radioCard = nullptr;
    //ProbeMessagesEncoder decoder(0,0);


    //configurator.LoadLicenseFile();
    //systemCommunication.Start(configurator.params.data_port, configurator.params.control_port);
    //status.StartDiagnostics(&systemCommunication);
    //radioCard.Start(&status, &systemCommunication);
    //decoder.Start(&status, radioCard, &systemCommunication);

    //status.SaveAlert("ALERT!!!", 0);


    AXI axi;


    if(!axi.init())
    {
        printf("Cannot map memory");
        return 1;
    };

    auto base = reinterpret_cast<volatile uint8_t*>(axi.getBasePointer());


    printf("Start\n");

    while(1)
    {
        printf("M\n");
        *base = 7;
        sleep(1);
        printf("M2\n");
        *base = 0;

    }


    return 1;
} 
