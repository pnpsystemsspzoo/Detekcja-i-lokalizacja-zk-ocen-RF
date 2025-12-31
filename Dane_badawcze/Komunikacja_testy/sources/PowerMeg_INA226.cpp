#include "PowerMeg_INA226.h"


PowerMeg_INA226 :: PowerMeg_INA226()
{

}

PowerMeg_INA226 :: ~PowerMeg_INA226()
{

}

unsigned char PowerMeg_INA226 :: Init(int radioCard_i2c_fd, unsigned char i2c_addr)
{
    unsigned char result = 0;
    i2c_fd = radioCard_i2c_fd;
    return result;
}

unsigned short int PowerMeg_INA226 :: WriteRegister(unsigned short int reg, unsigned char val)
{
    unsigned short int result = 0;
    return result;
}

unsigned short int PowerMeg_INA226 :: ReadRegister(unsigned short int reg, unsigned char *val)
{
    unsigned short int result = 0;
    return result;
}

float PowerMeg_INA226 :: INA226_getBusV(void)
{
    float result = 0;
    result = INA226_getBusVReg();
    return result;
}

float PowerMeg_INA226 :: INA226_getCurrent(void)
{
    float result = 0;
    result = INA226_getCurrentReg() * INA226_CURRENTLSB_INV;
    return result;
}

float PowerMeg_INA226 :: INA226_getPower(void)
{
    float result = 0;
    result = INA226_getPowerReg() * INA226_POWERLSB_INV;
    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setConfig(unsigned short int ConfigWord)
{
    unsigned char result = 0;

    unsigned char SentTable[3];
	SentTable[0] = INA226_CONFIG;
	SentTable[1] = (ConfigWord & 0xFF00) >> 8;
	SentTable[2] = (ConfigWord & 0x00FF);
	//return HAL_I2C_Master_Transmit(I2CHandler, DevAddress, SentTable, 3, INA226_I2CTIMEOUT);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getConfig(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_CONFIG};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getShuntV(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_SHUNTV};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getBusVReg(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_BUSV};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getPowerReg(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_POWER};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setCalibrationReg(unsigned short int ConfigWord)
{
    unsigned char result = 0;

    unsigned char SentTable[3];
	SentTable[0] = INA226_CALIB;
	SentTable[1] = (ConfigWord & 0xFF00) >> 8;
	SentTable[2] = (ConfigWord & 0x00FF);
	//return HAL_I2C_Master_Transmit(I2CHandler, DevAddress, SentTable, 3, INA226_I2CTIMEOUT);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getCalibrationReg(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_CALIB};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getCurrentReg(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_CURRENT};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getManufID(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_MANUF_ID};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getDieID(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_DIE_ID};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setMaskEnable(unsigned short int ConfigWord)
{
    unsigned char result = 0;

    unsigned char SentTable[3];
	SentTable[0] = INA226_MASK;
	SentTable[1] = (ConfigWord & 0xFF00) >> 8;
	SentTable[2] = (ConfigWord & 0x00FF);
	//return HAL_I2C_Master_Transmit(I2CHandler, DevAddress, SentTable, 3, INA226_I2CTIMEOUT);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getMaskEnable(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_MASK};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}

unsigned char PowerMeg_INA226 :: INA226_setAlertLimit(unsigned short int ConfigWord)
{
    unsigned char result = 0;

    unsigned char SentTable[3];
	SentTable[0] = INA226_ALERTL;
	SentTable[1] = (ConfigWord & 0xFF00) >> 8;
	SentTable[2] = (ConfigWord & 0x00FF);
	//return HAL_I2C_Master_Transmit(I2CHandler, DevAddress, SentTable, 3, INA226_I2CTIMEOUT);

    return result;
}

unsigned short int PowerMeg_INA226 :: INA226_getAlertLimit(void)
{
    unsigned short int result = 0;

    unsigned char SentTable[1] = {INA226_ALERTL};
	unsigned char ReceivedTable[2];
	//HAL_I2C_Master_Transmit(I2CHandler,DevAddress, SentTable, 1, INA226_I2CTIMEOUT);
	//if (HAL_I2C_Master_Receive(I2CHandler,DevAddress, ReceivedTable, 2, INA226_I2CTIMEOUT) != HAL_OK) return 0xFF;
	//else return ((uint16_t)ReceivedTable[0]<<8 | ReceivedTable[1]);

    return result;
}



