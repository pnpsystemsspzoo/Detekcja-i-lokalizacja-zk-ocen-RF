#include "GPIO_EXP_MCP23017.h"

GPIO_EXP_MCP23017 :: GPIO_EXP_MCP23017()
{

}

GPIO_EXP_MCP23017 :: ~GPIO_EXP_MCP23017()
{
    close(i2c_fd);
}

/**
 * Bit number associated to a give Pin
 */
unsigned char GPIO_EXP_MCP23017 :: bitForPin(unsigned char pin)
{
    unsigned char result = 0;

    result = pin % 8;

    return result;
}

/**
 * 
 */
unsigned char GPIO_EXP_MCP23017 :: bitWrite(unsigned char *reg, unsigned char bit, unsigned char val)
{
	unsigned char result = 0;

	if((bit >= 0) && (bit <= 7) && ((val == 1) || (val == 0)))
	{
		*reg ^= (-val ^ *reg) & (1UL << bit);
	}
	else
	{
		result = 1;
	}
	

    return result;
}

/**
 * 
 */
unsigned char GPIO_EXP_MCP23017 :: bitRead(unsigned char *reg, unsigned char bit, unsigned char *val)
{
	unsigned char result = 0;

	if((bit >= 0) && (bit <= 7))
	{
		*val = (*reg >> bit) & 1U;
	}
	else
	{
		result = 1;
	}
	

    return result;
}

/**
 * Register address, port dependent, for a given PIN
 */
unsigned char GPIO_EXP_MCP23017 :: regForPin(unsigned char pin, unsigned char portAaddr, unsigned char portBaddr)
{
    unsigned char result = 0;

    result = (pin < 8) ?portAaddr:portBaddr;

    return result;
}

/**
 * Writes a given register
 */
unsigned short int GPIO_EXP_MCP23017 :: writeRegisters(unsigned char reg, unsigned char *val, unsigned char size)
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
	//printf("\nWrite data %d\n", i2c_data.block[1]);
    return result;
}

/**
 * Reads a given register
 */
unsigned short int GPIO_EXP_MCP23017 :: readRegisters(unsigned char reg, unsigned char *val, unsigned char size)
{
    unsigned short int result = 0;
	struct i2c_smbus_ioctl_data args;
	union i2c_smbus_data i2c_data;

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


/**
 * Helper to update a single bit of an A/B register.
 * - Reads the current register value
 * - Writes the new register value
 */
unsigned char GPIO_EXP_MCP23017 :: updateRegisterBit(unsigned char pin, unsigned char pValue, unsigned char portAaddr, unsigned char portBaddr)
{
    unsigned char result = 0;

    unsigned char regValue;
	unsigned char regAddr = regForPin(pin, portAaddr, portBaddr);
	unsigned char bit = bitForPin(pin);
	readRegisters(regAddr, &regValue, 1);

	// set the value for the particular bit
	bitWrite(&regValue, bit, pValue);
	writeRegisters(regAddr, &regValue, 1);

    return result;
}

/**
 * Initializes the MCP23017 given its HW selected address, see datasheet for Address selection.
 */
unsigned char GPIO_EXP_MCP23017 :: Init(const char *i2c_dev_name, unsigned char i2c_addr)
{
    unsigned char result = 0;
	unsigned char tempVal = 0;

	i2c_fd = open(i2c_dev_name, O_RDWR);
    if(i2c_fd <= 0)
    {
        printf("\nCannot open %s device!", i2c_dev_name);
    }

    if(ioctl(i2c_fd, I2C_SLAVE, i2c_addr) < 0) 
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }


	// set defaults!
	// all inputs on port A and B
	tempVal = 0xff;
	writeRegisters(MCP23017_IODIRA, &tempVal, 1);
	writeRegisters(MCP23017_IODIRB, &tempVal, 1);

    return result;
}


/**
 * SelfTest
 */
unsigned char GPIO_EXP_MCP23017 :: SelfTest(void)
{
    unsigned char result = 0;
	unsigned char tab[10];
	unsigned short int regValin = 0;
	unsigned short int regValout = 0;

	//while(1)
	{
	
	pinMode(0, OUTPUT);//A
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);//B
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
	pinMode(14, OUTPUT);
	pinMode(15, OUTPUT);

	regValin = 0x0000;

	writeGPIOAB(regValin);
    readGPIOAB(&regValout);

	printf("\nReg Val out:  %d\n", regValout);


	// tab[0] = 0x0f;
	// writeRegisters(0, tab, 1);
	// readRegisters(0, tab, 1);
	// printf("\nReg Val out:  %d\n", tab[0]);

	usleep(1000000);
	}

    return result;
}


/**
 * Sets the pin mode to either INPUT or OUTPUT
 */
unsigned char GPIO_EXP_MCP23017 :: pinMode(unsigned char p, unsigned char d)
{
    unsigned char result = 0;

    updateRegisterBit(p, (d == INPUT), MCP23017_IODIRA, MCP23017_IODIRB);

    return result;
}

unsigned char GPIO_EXP_MCP23017 :: digitalWrite(unsigned char pin, unsigned char d)
{
    unsigned char result = 0;

    unsigned char gpio;
	unsigned char bit = bitForPin(pin);

	// read the current GPIO output latches
	unsigned char regAddr = regForPin(pin, MCP23017_OLATA, MCP23017_OLATB);
	readRegisters(regAddr, &gpio, 1);

	// set the pin and direction
	bitWrite(&gpio, bit, d);

	// write the new GPIO
	regAddr = regForPin(pin, MCP23017_GPIOA, MCP23017_GPIOB);
	writeRegisters(regAddr, &gpio, 1);

    return result;
}

unsigned char GPIO_EXP_MCP23017 :: pullUp(unsigned char p, unsigned char d)
{
    unsigned char result = 0;

    updateRegisterBit(p, d, MCP23017_GPPUA, MCP23017_GPPUB);

    return result;
}

unsigned char GPIO_EXP_MCP23017 :: digitalRead(unsigned char pin, unsigned char *val)
{
    unsigned char result = 0;

    unsigned char bit = bitForPin(pin);
	unsigned char regAddr = regForPin(pin, MCP23017_GPIOA, MCP23017_GPIOB);
	readRegisters(regAddr, val, 1);
	*val = (*val >> bit) & 0x1;

    return result;
}

/**
 * Writes all the pins in one go. This method is very useful if you are implementing a multiplexed matrix and want to get a decent refresh rate.
 */
unsigned char GPIO_EXP_MCP23017 :: writeGPIOAB(unsigned short int ba)
{
    unsigned char result = 0;
	unsigned char out[2];

	out[0] = ba & 0xFF;
	out[1] = (ba >> 8);

	writeRegisters(MCP23017_GPIOA, out, 2);

    return result;
}

/**
 * Reads all 16 pins (port A and B) into a single 16 bits variable.
 */
unsigned char GPIO_EXP_MCP23017 :: readGPIOAB(unsigned short int *val)
{
    unsigned char result = 0;
    unsigned short int ba = 0;
	unsigned char a;
	unsigned char tab[2];

	// read the current GPIO output latches
	readRegisters(MCP23017_GPIOA, tab, 2);

	a = tab[0];
	ba = tab[1];
	ba <<= 8;
	ba |= a;
	*val = ba;

    return result;
}

/**
 * Read a single port, A or B, and return its current 8 bit value.
 * Parameter b should be 0 for GPIOA, and 1 for GPIOB.
 */
unsigned char GPIO_EXP_MCP23017 :: readGPIO(unsigned char b, unsigned char *val)
{
    unsigned char result = 0;
	unsigned char tab = 0;

    // read the current GPIO output latches
	if(b == 0)
	{
		readRegisters(MCP23017_GPIOA, &tab, 1);
	}
	else 
	{
		readRegisters(MCP23017_GPIOB, &tab, 1);
	}
	*val = tab;

    return result;
}

/**
 * Configures the interrupt system. both port A and B are assigned the same configuration.
 * Mirroring will OR both INTA and INTB pins.
 * Opendrain will set the INT pin to value or open drain.
 * polarity will set LOW or HIGH on interrupt.
 * Default values after Power On Reset are: (false, false, LOW)
 * If you are connecting the INTA/B pin to arduino 2/3, you should configure the interupt handling as FALLING with
 * the default configuration.
 */
unsigned char GPIO_EXP_MCP23017 :: setupInterrupts(unsigned char mirroring, unsigned char openDrain, unsigned char polarity)
{
    unsigned char result = 0;
	unsigned char ioconfValue;

    // configure the port A
	readRegisters(MCP23017_IOCONA, &ioconfValue, 1);
	bitWrite(&ioconfValue, 6, mirroring);
	bitWrite(&ioconfValue, 2, openDrain);
	bitWrite(&ioconfValue, 1, polarity);
	writeRegisters(MCP23017_IOCONA, &ioconfValue, 1);

	// Configure the port B
	readRegisters(MCP23017_IOCONB, &ioconfValue, 1);
	bitWrite(&ioconfValue, 6, mirroring);
	bitWrite(&ioconfValue, 2, openDrain);
	bitWrite(&ioconfValue, 1, polarity);
	writeRegisters(MCP23017_IOCONB, &ioconfValue, 1);

    return result;
}

/**
 * Set's up a pin for interrupt. uses arduino MODEs: CHANGE, FALLING, RISING.
 *
 * Note that the interrupt condition finishes when you read the information about the port / value
 * that caused the interrupt or you read the port itself. Check the datasheet can be confusing.
 *
 */
unsigned char GPIO_EXP_MCP23017 :: setupInterruptPin(unsigned char pin, unsigned char mode)
{
    unsigned char result = 0;

    // set the pin interrupt control (0 means change, 1 means compare against given value);
	updateRegisterBit(pin, (mode != 0), MCP23017_INTCONA, MCP23017_INTCONB);
	// if the mode is not CHANGE, we need to set up a default value, different value triggers interrupt

	// In a RISING interrupt the default value is 0, interrupt is triggered when the pin goes to 1.
	// In a FALLING interrupt the default value is 1, interrupt is triggered when pin goes to 0.
	updateRegisterBit(pin, (mode == 1), MCP23017_DEFVALA, MCP23017_DEFVALB);

	// enable the pin for interrupt
	updateRegisterBit(pin, HIGH, MCP23017_GPINTENA, MCP23017_GPINTENB);

    return result;
}

unsigned char GPIO_EXP_MCP23017 :: getLastInterruptPin(unsigned char *val)
{
    unsigned char result = 0;
    unsigned char intf;
	unsigned char tempVal;

	// try port A
	readRegisters(MCP23017_INTFA, &intf, 1);
	for(int i = 0; i < 8; i++) 
	{
		bitRead(&intf, i, &tempVal);
		if(tempVal) 
		{
			*val = i;
		}
		else
		{
			*val = MCP23017_INT_ERR;
		}
		
	}

	// try port B
	readRegisters(MCP23017_INTFB, &intf, 1);
	for(int i = 0; i < 8; i++) 
	{
		bitRead(&intf, i, &tempVal);
		if(tempVal) 
		{
			*val = i + 8;
		}
		else
		{
			*val = MCP23017_INT_ERR;
		}
		
	}

    return result;
}

unsigned char GPIO_EXP_MCP23017 :: getLastInterruptPinValue(unsigned char *val)
{
    unsigned char result = 0;
	unsigned char tempVal;
    unsigned char intPin;

	getLastInterruptPin(&intPin);
	if(intPin != MCP23017_INT_ERR)
	{
		unsigned char intcapreg = regForPin(intPin, MCP23017_INTCAPA, MCP23017_INTCAPB);
		unsigned char bit = bitForPin(intPin);
		readRegisters(intcapreg, &tempVal, 1);
		*val = (tempVal >> bit) & (0x01);
	}
	else
	{
		*val = MCP23017_INT_ERR;
	}

    return result;
}

