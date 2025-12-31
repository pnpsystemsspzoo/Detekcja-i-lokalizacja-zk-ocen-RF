#include "PowerMeg_INA226.h"


PowerMeg_INA226 :: PowerMeg_INA226()
{

}

PowerMeg_INA226 :: ~PowerMeg_INA226()
{

}

unsigned char PowerMeg_INA226 :: Init(const char *i2c_dev_name, unsigned char i2c_addr)
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

/**
 * Writes a given register
 */
unsigned short int PowerMeg_INA226 :: writeRegisters(unsigned char reg, unsigned char *val, unsigned char size)
{
    unsigned short int result = 0;
	struct i2c_smbus_ioctl_data args;
	union i2c_smbus_data i2c_data;

	i2c_data.block[0] = size;

	for(int i = 1; i <= size; i++)
	{
		i2c_data.block[i] = val[i - 1];
	}

    args.read_write = 0;
    args.command = reg;
    args.size = I2C_SMBUS_I2C_BLOCK_DATA;
    args.data = &i2c_data;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nWrite error\n");
    }

    return result;
}

/**
 * Reads a given register
 */
unsigned short int PowerMeg_INA226 :: readRegisters(unsigned char reg, unsigned char *val, unsigned char size)
{
    unsigned short int result = 0;
	struct i2c_smbus_ioctl_data args;
	union i2c_smbus_data i2c_data;


    // unsigned char buf[4];

    // args.read_write = 0;
    // args.command =  reg;
    // args.size = 1;
    // if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    // {
    //     printf("\nWrite channel state error\n");
    // }

    // usleep(2000);

    // read(i2c_fd, val, 2);
    

    args.read_write = 1;
    args.size = 1;
    args.data = &i2c_data;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nRead channels state error\n");
    }

	args.read_write = 1;
    args.command = reg;
    args.size = I2C_SMBUS_I2C_BLOCK_DATA;
	args.data = &i2c_data;
	i2c_data.block[0] = size;
    if(ioctl(i2c_fd, I2C_SMBUS, &args) < 0)
    {
        printf("\nRead error\n");
    }

	for(int i = 1; i <= size; i++)
	{
		val[i - 1] = i2c_data.block[i];
	}

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getBusV(float *val)
{
    unsigned char result = 0;
    unsigned short int tempVal;

    result = INA226_getBusVReg(&tempVal);
    *val = (float)tempVal * INA226_BUSVOLTAGE_LSB;

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getCurrent(float *val)
{
    unsigned char result = 0;
    unsigned short int tempVal;

    result = INA226_getCurrentReg(&tempVal);
    *val = (float)tempVal * INA226_CURRENTLSB_INV;

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getPower(float *val)
{
    unsigned char result = 0;
    unsigned short int tempVal;

    result = INA226_getPowerReg(&tempVal);
    *val = (float)tempVal * INA226_POWERLSB_INV;
    
    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setConfig(unsigned short int ConfigWord)
{
    unsigned char result = 0;
    unsigned char SentTable[2];

	SentTable[0] = (ConfigWord & 0xFF00) >> 8;
	SentTable[1] = (ConfigWord & 0x00FF);

    writeRegisters(INA226_CONFIG, SentTable, 2);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getConfig(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_CONFIG, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getShuntV(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_SHUNTV, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getBusVReg(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_BUSV, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getPowerReg(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_POWER, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setCalibrationReg(unsigned short int ConfigWord)
{
    unsigned char result = 0;
    unsigned char SentTable[2];

	SentTable[0] = (ConfigWord & 0xFF00) >> 8;
	SentTable[1] = (ConfigWord & 0x00FF);
	
    writeRegisters(INA226_CALIB, SentTable, 2);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getCalibrationReg(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_CALIB, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getCurrentReg(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_CURRENT, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getManufID(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_MANUF_ID, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getDieID(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_DIE_ID, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setMaskEnable(unsigned short int ConfigWord)
{
    unsigned char result = 0;
    unsigned char SentTable[2];

	SentTable[0] = (ConfigWord & 0xFF00) >> 8;
	SentTable[1] = (ConfigWord & 0x00FF);

    writeRegisters(INA226_MASK, SentTable, 2);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getMaskEnable(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_MASK, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setAlertLimit(unsigned short int ConfigWord)
{
    unsigned char result = 0;
    unsigned char SentTable[2];

	SentTable[0] = (ConfigWord & 0xFF00) >> 8;
	SentTable[1] = (ConfigWord & 0x00FF);

    writeRegisters(INA226_ALERTL, SentTable, 2);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_getAlertLimit(unsigned short int *val)
{
    unsigned char result = 0;
	unsigned char ReceivedTable[2];

	readRegisters(INA226_ALERTL, ReceivedTable, 2);
    *val = ((unsigned short int)ReceivedTable[0] << 8 | ReceivedTable[1]);

    return result;
}



