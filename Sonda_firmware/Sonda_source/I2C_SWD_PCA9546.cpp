#include "I2C_SWD_PCA9546.h"

I2C_SWD_PCA9546 :: I2C_SWD_PCA9546()
{

}

I2C_SWD_PCA9546 :: ~I2C_SWD_PCA9546()
{
    close(i2c_fd);
}

unsigned char I2C_SWD_PCA9546 :: Init(const char *i2c_dev_name, unsigned char i2c_addr)
{
    unsigned char result = 0;

    i2c_fd = open(i2c_dev_name, O_RDWR);
    if(i2c_fd <= 0)
    {
        printf("\nCannot open %s device!", i2c_dev_name);
    }

    if(ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) 
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

    return result;
}

unsigned char I2C_SWD_PCA9546 :: EnableChannel(unsigned char ch)
{
    unsigned char result = 0;
    struct i2c_smbus_ioctl_data args;
    union i2c_smbus_data i2c_data;

    args.read_write = 1;
    args.size = 1;
    args.data = &i2c_data;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nRead channels state error\n");
    }

    usleep(2000);

    args.read_write = 0;
    args.command = i2c_data.block[0] | ch;
    args.size = 1;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nWrite channel state error\n");
    }
	
    return result;
}

unsigned char I2C_SWD_PCA9546 :: DisableChannel(unsigned char ch)
{
    unsigned char result = 0;
    struct i2c_smbus_ioctl_data args;
    union i2c_smbus_data i2c_data;

    args.read_write = 1;
    args.size = 1;
    args.data = &i2c_data;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nRead channels state error\n");
    }

    usleep(2000);

    args.read_write = 0;
    if(i2c_data.block[0] != 0x00)
    {
        args.command = i2c_data.block[0] ^ ch;
    }
    args.size = 1;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nWrite channel state error\n");
    }
	
    return result;
}

unsigned char I2C_SWD_PCA9546 :: DisableAllChannels(void)
{
    unsigned char result = 0;
    struct i2c_smbus_ioctl_data args;

    args.read_write = 0;
    args.command = 0x00;
    args.size = 1;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nWrite channel state error\n");
    }
	
    return result;
}

unsigned char I2C_SWD_PCA9546 :: GetChannelsState(unsigned char *state)
{
    unsigned char result = 0;
    struct i2c_smbus_ioctl_data args;
    union i2c_smbus_data i2c_data;

    args.read_write = 1;
    args.size = 1;
    args.data = &i2c_data;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nRead channels state error\n");
    }

    *state = i2c_data.block[0];

    return result;
}


unsigned char I2C_SWD_PCA9546 :: SelfTest(void)
{
    unsigned char result = 0;

    DisableChannel(CHANNEL0);
    DisableChannel(CHANNEL2);
    GetChannelsState(&result);
    printf("\nStat  %d\n", result);
    EnableChannel(CHANNEL0);
    EnableChannel(CHANNEL2);
    GetChannelsState(&result);
    printf("\nStat  %d\n", result);
    DisableAllChannels();
    GetChannelsState(&result);
    printf("\nStat  %d\n", result);

    return result;
}


