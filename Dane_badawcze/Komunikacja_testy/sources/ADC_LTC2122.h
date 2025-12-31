#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define A0_RESET                            0x00
#define A1_POWER_DOWN_MODES                 0x01
#define A2_ADC_CONTROL                      0x02
#define A3_DEVICE_ID                        0x03
#define A4_BANK_ID                          0x04
#define A5_NUMBER_OF_LANES_1                0x05
#define A6_NUMBER_OF_FRAMES_PER_MINUTE_1    0x06
#define A7_JESD204B_MODES                   0x07
#define A8_JESD204B_SUBCLASS_MODES          0x08
#define A9_PERIODIC_TEST_PATTERNS           0x09
#define A10_CML_OUTPUT_MAGNITUDE            0x0A

class ADC_LTC2122
{
    private:
    public:
    ADC_LTC2122();
    ~ADC_LTC2122();
    unsigned char Init(void);
    unsigned char WriteRegister(unsigned char reg, unsigned char val);
    unsigned char ReadData(void);
};

