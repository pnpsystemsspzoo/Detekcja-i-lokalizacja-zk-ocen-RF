#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define CONFIGURATION_1                 0x00
#define CONFIGURATION_2                 0x01
#define CONFIGURATION_3                 0x02
#define PLL_CONFIGURATION               0x03
#define PLL_INTEGER_DIVISION_RATIO      0x04
#define PLL_FRACTIONAL_DIVISION_RATIO   0x05
#define RESERVED                        0x06
#define CLOCK_CONFIGURATION_1           0x07
#define TEST_MODE_1                     0x08
#define TEST_MODE_2                     0x09
#define CLOCK_CONFIGURATION_2           0x0A

class GNSS_MAX2771
{
    private:
    public:
    GNSS_MAX2771();
    ~GNSS_MAX2771();
    unsigned char Init(void);
    unsigned char ReadData(void);
};

