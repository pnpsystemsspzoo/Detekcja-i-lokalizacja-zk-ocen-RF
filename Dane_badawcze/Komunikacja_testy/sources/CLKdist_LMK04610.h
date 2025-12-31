#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>


class CLKdist_LMK04610
{
    private:
    public:
    CLKdist_LMK04610();
    ~CLKdist_LMK04610();
    unsigned short int Init(void);
    unsigned short int WriteRegister(unsigned short int reg, unsigned char val);
    unsigned short int ReadRegister(unsigned short int reg, unsigned char *val);
};

