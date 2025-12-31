#include "ADC_AD9690.h"


ADC_AD9690 :: ADC_AD9690()
{

}

ADC_AD9690 :: ~ADC_AD9690()
{

}

unsigned short int ADC_AD9690 :: Init(void)
{
    unsigned short int result = 0;
    unsigned char register_value = 0x00;
    unsigned int sleepTimeus = 1000;

    
    /*
    INTERFACE_ CONFIG_A
    B7 - Soft reset (self clearing) 
    B6 - LSB first 0 = MSB 1 = LSB
    B5 - Address ascension
    B4 - 0
    B3 - 0
    B2 - Address ascension
    B1 - LSB first 0 = MSB 1 = LSB
    B0 - Soft reset (self clearing)  
    Default: 0x00
    */ 
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(INTERFACE_CONFIG_A, register_value);
    /*
    INTERFACE_ CONFIG_B
    B7 - Single instruction
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - Datapath soft reset (self clearing)
    B0 - 0  
    Default: 0x00
    */ 
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(INTERFACE_CONFIG_B, register_value); 
    /*
    DEVICE_ CONFIG
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B10 - 00 = normal operation 10 = standby 11 = power-down  
    Default: 0x00
    */ 
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DEVICE_CONFIG   , register_value); 
    /*
    CHIP_TYPE
    B76543210 - 011 = high speed ADC 
    0x03 
    Read only
    */ 
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_TYPE, register_value); 
    /*
    CHIP_ID (low byte)
    B7 - 1
    B6 - 1
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 1
    B1 - 0
    B0 - 1
    Default: 0xD6
    Read only
    */ 
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_ID_L, register_value); 
    /*
    CHIP_ID (high byte)
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0  
    Default: 0x00
    Read only
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_ID_H, register_value); 
    /*
    CHIP_ GRADE
    B7654 - 1010 = 1000 MSPS 0101 = 500 MSPS
    B3 - X
    B2 - X
    B1 - X
    B0 - X  
    Read only
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_GRADE, register_value); 
    /*
    Device index
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 1  
    Default: 0x01
    Reserved
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DEVICE_INDEX, register_value); 
    /*
    Scratch pad
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0    
    Default: 0x00
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SCRATCH_PAD, register_value); 
    /*
    SPI revision
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 1  
    Default: 0x01
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SPI_REVISION, register_value); 
    /*
    Vendor ID (low byte)
    B7 - 0
    B6 - 1
    B5 - 0
    B4 - 1
    B3 - 0
    B2 - 1
    B1 - 1
    B0 - 0
    Default: 0x56
    Read only
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(VENDOR_ID_L, register_value); 
    /*
    Vendor ID (high byte)
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 1
    B1 - 0
    B0 - 0  
    Default: 0x04
    Read only
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(VENDOR_ID_H, register_value); 
    /*
    Analog input
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - Input disable 0 = normal operation 1 = input disabled  
    Default: 0x00 
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(ANALOG_INPUT, register_value); 
    /*
    Input termination
    B7654 - Analog input differential termination 0000 = 400 Ω (default) 0001 = 200 Ω 0010 = 100 Ω 0110 = 50 Ω
    B3210 - 1110 = AD9690-1000 1100 = AD9690-500  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(INPUT_TERMINATION, register_value); 
    /*
    Input capacitance
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - 0x1F = 3 pF to GND (default) 0x00 = 1.5 pF to GND 
    Default: 0x1F 
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(INPUT_CAPACITANCE, register_value); 
    /*
    Buffer Control 1
    B7654 - 0000 = 1.0× buffer current 0001 = 1.5× buffer current 0010 = 2.0× buffer current (default for AD9690-500) 0011 = 2.5× buffer current 0100 = 3.0× buffer current (default for AD9690-1000) 0101 = 3.5× buffer current … 1111 = 8.5× buffer current
    B3210 - 0100 = AD9690-1000 0010 = AD9690-500  
    Default: 0x44 for AD9690-1000; 0x22 for AD9690-500
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(BUFFER_CONTROL_1, register_value); 
    /*
    Buffer Control 2
    B7654 - 0100 = setting 1 0101 = setting 2 (default for AD9690-1000) 0110 = setting 3 (default for AD9690-500) 0111 = setting 4
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0  
    Default: 0x50 for AD9690-1000; 0x60 for AD9690-500
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(BUFFER_CONTROL_2, register_value); 
    /*
    Buffer Control 3
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - 1000 = setting 1 1001 = setting 2 (default for AD9690-1000) 1010 = setting 3 (default for AD9690-500)
    Default: 0x09 for AD9690-1000; 0x0A for AD9690-500  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(BUFFER_CONTROL_3, register_value); 
    /*
    Buffer Control 4
    B7 - 0
    B6 - 0
    B5 - High frequency setting 0 = OFF (default) 1 = ON
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(BUFFER_CONTROL_4, register_value); 
    /*
    Buffer Control 5
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - Low Frequency Operation 0 = off 1 = on (default)
    B1 - 0
    B0 - 0  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(BUFFER_CONTROL_5, register_value); 
    /*
    Input full-scale range
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - Full-scale adjust 0000 = 1.94 V 1000 = 1.46 V 1001 = 1.58 V 1010 = 1.70 V (default for AD9690-1000) 1011 = 1.82 V 1100 = 2.06 V (default for AD9690-500) 
    Default: 0x0A for AD9690-1000; 0x0C for AD9690-500 
    V p-p differ-ential; use in conjunc-tion with Reg. 0x030
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(INPUT_FULL_SCALE_RANGE, register_value); 
    /*
    Input full-scale control
    B7 - 0
    B6 - 0
    B5 - 0
    B432 - Full-scale control See Table 10 for recommended settings for different frequency bands; default values: AD9690-1000 = 110 AD9690-500 = 001 AD9690-500 = 110 (for <1.82 V)
    B1 - 0
    B0 - 0
    Used in conjunc-tion with Reg. 0x025  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(INPUT_FULL_SCALE_CONTROL, register_value); 
    /*
    V_1P0 control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 1.0 V reference select 0 = internal 1 = external  
    Default: 0x00
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(V_1P0_CONTROL, register_value); 
    /*
    Temperature diode
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - Diode selection 0 = no diode selected 1 = temper-ature diode selected
    Default: 0x00
    Used in conjunc-tion with Reg. 0x040  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(TEMPERATURE_DIODE, register_value); 
    /*
    PDWN/ STBY pin control
    B7 - 0 = PDWN/ STBY enabled 1 = disabled
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used in conjunc-tion with Reg. 0x040  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(PDWN_STBY_PIN_CONTROL, register_value); 
    /*
    Chip pin control
    B76 - PDWN/STBY function 00 = power down 01 = standby 10 = disabled
    B543 - 111 = Reserved
    B210 - Fast Detect (FD) 000 = Fast Detect output 001 = JESD204B LMFC output 010 = JESD204B internal SYNC~ output 011 = temperature diode 111 = disabled
    Default: 0x3F  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_PIN_CONTROL, register_value); 
    /*
    Clock divider
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B210 - 000 = divide by −1 001 = divide by 2 011 = divide by 4 111 = divide by 8
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CLOCK_DIVIDER, register_value); 
    /*
    Clock divider phase
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - Independently controls ADC clock divider phase offset 0000 = 0 input clock cycles delayed 0001 = ½ input clock cycles delayed 0010 = 1 input clock cycles delayed 0011 = 1½ input clock cycles delayed 0100 = 2 input clock cycles delayed 0101 = 2½ input clock cycles delayed … 1111 = 7½ input clock cycles delayed
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CLOCK_DIVIDER_PHASE, register_value); 
    /*
    Clock divider and SYSREF control
    B7 - Clock divider auto phase adjust 0 = disabled 1 = enabled
    B6 - 0
    B5 - 0
    B4 - 0
    B32 - Clock divider negative skew window 00 = no negative skew 01 = 1 device clock of negative skew 10 = 2 device clocks of negative skew 11 = 3 device clocks of negative skew
    B10 - Clock divider positive skew window 00 = no positive skew 01 = 1 device clock of positive skew 10 = 2 device clocks of positive skew 11 = 3 device clocks of positive skew
    Default: 0x00
    Clock divider must be >1  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CLOCK_DIVIDER_AND_SYSREF_CONTROL, register_value); 
    /*
    Clock delay control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - Clock fine delay adjust enable 0 = disabled 1 = enabled
    Default: 0x00
    Enabling the clock fine delay adjust causes a datapath reset  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CLOCK_DELAY_CONTROL, register_value); 
    /*
    Clock fine delay
    B76543210 - Clock fine delay adjust[7:0], twos complement coded control to adjust the fine sample clock skew in ~1.7 ps steps ≤ −88 = −151.7 ps skew −87 = −150 ps skew … 0 = 0 ps skew … ≥ +87 = +150 ps skew
    Default: 0x00
    Used in con-junction with Reg. 0x0117  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CLOCK_FINE_DELAY, register_value); 
    /*
    Clock status
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0 = no input clock detected 1 = input clock detected
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CLOCK_STATUS, register_value); 
    /*
    SYSREF± Control 1
    B7 - 0
    B6 - SYSREF± flag reset 0 = normal operat-ion 1 = flags held in reset
    B5 - 0
    B4 - SYSREF± transition select 0 = low to high 1 = high to low
    B3 - CLK± edge select 0 = rising 1 = falling
    B21 - SYSREF± mode select 00 = disabled 01 = continuous 10 = N shot
    B0 - 0
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SYSREF_CONTROL_1, register_value); 
    /*
    SYSREF± Control 2
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - SYSREF N-shot ignore counter select 0000 = next SYSREF± only 0001 = ignore the first SYSREF± transitions 0010 = ignore the first two SYSREF± transitions … 1111 = ignore the first 16 SYSREF± transitions
    Default: 0x00
    Mode select (Reg 0x120, Bits [2:1]) must be N-shot  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SYSREF_CONTROL_2, register_value); 
    /*
    SYSREF± timestamp delay control
    B6543210 - SYSREF± timestamp delay, Bits[6:0] 0x00 = no delay 0x01 = 1 clock delay … 0x7F = 127 clocks delay
    Default: 0x00
    Ignored when Reg. 0x01FF = 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SYSREF_TIMESTAMP_DELAY_CONTROL, register_value); 
    /*
    SYSREF± Status 1
    B7654 - SYSREF± hold status, Register 0x128[7:4], refer to Table 28
    B3210 - SYSREF± setup status, Register 0x128[3:0], refer to Table 28
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SYSREF_STATUS_1, register_value); 
    /*
    SYSREF± and clock divider status
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - Clock divider phase when SYSREF± was captured 0000 = in-phase 0001 = SYSREF± is ½ cycle delayed from clock 0010 = SYSREF± is 1 cycle delayed from clock 0011 = 1½ input clock cycles delayed 0100 = 2 input clock cycles delayed 0101 = 2½ input clock cycles delayed … 1111 = 7½ input clock cycles delayed
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SYSREF_AND_CLOCK_DIVIDER_STATUS, register_value); 
    /*
    SYSREF± counter
    B76543210 - SYSREF counter, Bits[7:0] increments when a SYSREF± is captured
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SYSREF_COUNTER, register_value); 
    /*
    Chip sync mode
    B10 - Synchronization mode 00 = normal 01 = timestamp
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_SYNC_MODE, register_value); 
    /*
    Chip application mode
    B7 - 0
    B6 - 0
    B5 - Chip Q ignore 0 = normal (I/Q) 1 = ignore (I – only)
    B4 - 0
    B3 - 0
    B2 - 0
    B10 - Chip operating mode 00 = full bandwidth mode 01 = DDC 0 on 10 = DDC 0 and DDC 1 on
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_APPLICATION_MODE, register_value); 
    /*
    Chip decimation ratio
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B210 - Chip decimation ratio select 000 = full sample rate (decimate = 1) 001 = decimate by 2 010 = decimate by 4 011 = decimate by 8 100 = decimate by 16
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CHIP_DECIMATION_RATIO, register_value); 
    /*
    Customer offset
    B76543210 - Offset adjust in LSBs from +127 to −128 (twos complement format)
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(CUSTOMER_OFFSET, register_value); 
    /*
    Fast detect (FD) control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - Force FD pin; 0 = normal function; 1 = force to value
    B2 - Force value of FD pin if force pins is true, this value is output on FD pin
    B1 - 0
    B0 - Enable fast detect output
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FAST_DETECT_CONTROL, register_value); 
    /*
    FD upper threshold LSB
    B76543210 - Fast detect upper threshold, Bits[7:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FD_UPPER_THRESHOLD_LSB, register_value); 
    /*
    FD upper threshold MSB
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Fast detect upper threshold, Bits[12:8]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FD_UPPER_THRESHOLD_MSB, register_value); 
    /*
    FD lower threshold LSB
    B76543210 - Fast detect lower threshold, Bits[7:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FD_LOWER_THRESHOLD_LSB, register_value); 
    /*
    FD lower threshold MSB
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Fast detect lower threshold, Bits[12:8]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FD_LOWER_THRESHOLD_MSB, register_value); 
    /*
    FD dwell time LSB
    B76543210 - Fast detect dwell time, Bits[7:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FD_DWELL_TIME_LSB, register_value); 
    /*
    FD dwell time MSB
    B76543210 - Fast detect dwell time, Bits[15:8]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(FD_DWELL_TIME_MSB, register_value); 
    /*
    Signal monitor synchroniza-tion control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B10 - Synchronization mode 00 = disabled 01 = continuous 11 = one shot
    Default: 0x00
    Refer to the Signal Monitor section  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_SYBCHRONIZATION_CONTROL, register_value); 
    /*
    Signal monitor control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - Peak detector 0 = disabled 1 = enabled
    B0 - 0
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_CONTROL, register_value); 
    /*
    Signal Monitor Period Register 0
    B76543210 - Signal monitor period, Bits[7:0]
    Default: 0x80
    In deci-mated output clock cycles  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_PERIOD_REGISTER_0, register_value); 
    /*
    Signal Monitor Period Register 1
    B76543210 - Signal monitor period, Bits[15:8]
    Default: 0x00
    In deci-mated output clock cycles  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_PERIOD_REGISTER_1, register_value); 
    /*
    Signal Monitor Period Register 2
    B76543210 - Signal monitor period, Bits[23:16]
    Default: 0x00
    In deci-mated output clock cycles  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_PERIOD_REGISTER_2, register_value); 
    /*
    Signal monitor result control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - Result update 1 = update results (self clear)
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - Result selection 0 = reserved 1 = peak detector
    Default: 0x01  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_RESULT_CONTROL, register_value); 
    /*
    Signal Monitor Result Register 0
    B76543210 - Signal monitor result, Bits[7:0] When Register 0x0274[0] = 1, result bits [19:7] = peak detector absolute value [12:0]; result bits [6:0] = 0
    Read only
    Updated based on Reg. 0x274[4]  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_RESULT_REGISTER_0, register_value); 
    /*
    Signal Monitor Result Register 1
    B76543210 - Signal monitor result, Bits[15:8]
    Read only
    Updated based on Reg. 0x274[4]  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_RESULT_REGISTER_1, register_value); 
    /*
    Signal Monitor Result Register 1
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - Signal monitor result, Bits[19:16]
    Read only
    Updated based on Reg. 0x274[4]  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_RESULT_REGISTER_2, register_value); 
    /*
    Signal monitor period counter result
    B - Period count result, Bits[7:0]
    Read only
    Updated based on Reg. 0x274[4]  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_PERIOD_COUNTER_RESULT, register_value); 
    /*
    Signal monitor SPORT over JESD204B control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B10 - 00 = reserved 11 = enable
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SIGNAL_MONITOR_SPORT_OVER_JESD204B_CONTROL, register_value); 
    /*
    SPORT over JESD204B input selection
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - Peak detector 0 = disabled 1 = enabled
    B0 - 0
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(SPORT_OVER_JESD204B_INPUT_SELECTION, register_value); 
    /*
    DDC synch control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - DDC NCO soft reset 0 = normal operation 1 = reset
    B3 - 0
    B2 - 0
    B10 - Synchronization mode (triggered by SYSREF±) 00 = disabled 01 = continuous 11 = 1-shot  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_SYNCH_CONTROL, register_value); 
    /*
    DDC 0 control
    B7 - Mixer select 0 = real mixer 1 = complex mixer
    B6 - Gain select 0 = 0 dB gain 1 = 6 dB gain
    B54 - IF (intermediate frequency) mode 00 = variable IF mode (mixers and NCO enabled) 01 = 0 Hz IF mode (mixer bypassed, NCO disabled) 10 = fADC/4 Hz IF mode (fADC/4 down-mixing mode) 11 = test mode (mixer inputs forced to +FS, NCO enabled)
    B3 - Complex to real enable 0 = disabled 1 = enabled
    B2 - 0
    B10 - Decimation rate select (complex—real disabled) 11 = decimate by 2 00 = decimate by 4 01 = decimate by 8 10 = decimate by 16 (complex—real enabled) 11 = decimate by 1 00 = decimate by 2 01 = decimate by 4 10 = decimate by 8
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_CONTROL, register_value); 
    /*
    DDC 0 input selection
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Refer to the DDC section  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_INPUT_SELECTION, register_value); 
    /*
    DDC 0 frequency LSB
    B76543210 - DDC 0 NCO frequency value, Bits[7:0] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_FREQUENCY_LSB, register_value); 
    /*
    DDC0 frequency MSB
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3210 - DDC 0 NCO frequency value, Bits[11:8] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_FREQUENCY_MSB, register_value); 
    /*
    DDC 0 phase LSB
    B76543210 - DDC 0 NCO phase value, Bits[7:0] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_PHASE_LSB, register_value); 
    /*
    DDC 0 phase MSB
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3210 - DDC 0 NCO phase value, Bits[11:8] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_PHASE_MSB, register_value); 
    /*
    DDC 0 output test mode selection
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - I output test mode enable 0 = disabled 1 = enabled
    Default: 0x00
    Refer to the DDC section  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_0_OUTPUT_TEST_MODE_SELECTION, register_value); 
    /*
    DDC 1 control
    B7 - Mixer select 0 = real mixer 1 = complex mixer
    B6 - Gain select 0 = 0 dB gain 1 = 6 dB gain
    B54 - IF (intermediate frequency) mode 00 = variable IF mode (mixers and NCO enabled) 01 = 0 Hz IF mode(mixer bypassed, NCO disabled) 10 = fADC/4 Hz IF mode (fADC/4 downmixing mode) 11 = test mode (mixer inputs forced to +FS, NCO enabled)
    B3 - Complex to real enable 0 = disabled 1 = enabled
    B2 - 0
    B10 - Decimation rate select (complex—real disabled) 11 = decimate by 2 00 = decimate by 4 01 = decimate by 8 10 = decimate by 16 (complex—real enabled) 11 = decimate by 1 00 = decimate by 2 01 = decimate by 4 10 = decimate by 8
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_CONTROL, register_value); 
    /*
    DDC 1 input selection
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Refer to the DDC section  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_INPUT_SELECTION, register_value); 
    /*
    DDC 1 frequency LSB
    B76543210 - DDC 1 NCO frequency value, Bits[7:0] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_FREQUENCY_LSB, register_value); 
    /*
    DDC 1 frequency MSB
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3210 - DDC 1 NCO frequency value, Bits[11:8] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_FREQUENCY_MSB, register_value); 
    /*
    DDC 1 phase LSB
    B76543210 - DDC 1 NCO phase value, Bits[7:0] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_PHASE_LSB, register_value); 
    /*
    DDC 1 phase MSB
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3210 - DDC 1 NCO phase value, Bits[11:8] twos complement
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_PHASE_MSB, register_value); 
    /*
    DDC 1 output test mode selection
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - I output test mode enable 0 = disabled 1 = enabled
    Default: 0x00
    Refer to the DDC section  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DDC_1_OUTPUT_TEST_MODE_SELECTION, register_value); 
    /*
    ADC test modes
    B7 - User pattern selection 0 = cont-inuous repeat 1 = single pattern
    B6 - 0
    B5 - Reset PN long gen 0 = long PN enable 1 = long PN reset
    B4 - Reset PN short gen 0 = short PN enable 1 = short PN reset
    B3210 - Test mode selection 0000 = off, normal operation 0001 = midscale short 0010 = positive full-scale 0011 = negative full-scale 0100 = alternating checker board 0101 = PN sequence, long 0110 = PN sequence, short 0111 = 1/0 word toggle 1000 = the user pattern test mode (used with Register 0x0550, Bit 7 and user pattern 1, 2, 3, 4 registers) 1111 = ramp output
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(ADC_TEST_MODES, register_value); 
    /*
    User Pattern 1 LSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_1_LSB, register_value); 
    /*
    User Pattern 1 MSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_1_MSB, register_value); 
    /*
    User Pattern 2 LSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_2_LSB, register_value); 
    /*
    User Pattern 2 MSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_2_MSB, register_value); 
    /*
    User Pattern 3 LSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_3_LSB, register_value); 
    /*
    User Pattern 3 MSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_3_MSB, register_value); 
    /*
    User Pattern 4 LSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_4_LSB, register_value); 
    /*
    User Pattern 4 MSB
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Used with Reg. 0x550 and Reg. 0x573  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(USER_PATTERN_4_MSB, register_value); 
    /*
    Output Mode Control 1
    B7 - 0
    B654 - Converter control Bit 1 selection 000 = tie low (1’b0) 001 = overrange bit 011 = fast detect (FD) bit 101 = SYSREF± Only used when CS (Register 0x58F) = 2 or 3
    B3 - 0
    B210 - Converter control Bit 0 selection 000 = tie low (1’b0) 001 = overrange bit 011 = fast detect (FD) bit 101 = SYSREF± Only used when CS (Register 0x58F) = 3
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(OUTPUT_MODE_CONTROL_1, register_value); 
    /*
    Output Mode Control 2
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B210 - Converter control Bit 2 selection 000 = tie low (1’b0) 001 = overrange bit 011 = fast detect (FD) bit 101 = SYSREF Used when CS (Register 0x58F) = 1, 2, or 3
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(OUTPUT_MODE_CONTROL_2, register_value); 
    /*
    Output mode
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - Sample invert 0 = normal 1 = sample invert
    B10 - Data format select 00 = offset binary 01 = twos complement
    Default: 0x01  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(OUTPUT_MODE, register_value); 
    /*
    Output overrange (OR) clear
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - Virtual Convert-er 3 OR 0 = OR bit enabled 1 = OR bit cleared
    B2 - Virtual Converter 2 OR 0 = OR bit enabled 1 = OR bit cleared
    B1 - Virtual Converter 1 OR 0 = OR bit enabled 1 = OR bit cleared
    B0 - Virtual Converter 0 OR 0 = OR bit enabled 1 = OR bit cleared
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(OUTPUT_AVERAGE_OR_CLEAR, register_value); 
    /*
    Output OR status
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - Virtual Convert-er 3 OR 0 = no OR 1 = OR occurred
    B2 - Virtual Converter 2 OR 0 = no OR 1 = OR occurred
    B1 - Virtual Converter 1 OR 0 = no OR 1 = OR occurred
    B0 - Virtual Converter 0 OR 0 = no OR 1 = OR occurred
    Default: 0x00
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(OUTPUT_OR_STATUS, register_value); 
    /*
    JESD204B lane rate control
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0 = serial lane rate ≥6.25 Gbps and ≤12.5 Gbps 1 = serial lane rate must be ≥ 3.125 Gbps and ≤6.25 Gbps
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x10  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LANE_RATE_CONTROL, register_value); 
    /*
    JESD204B PLL lock status
    B7 - PLL lock 0 = not locked 1 = locked
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B1 - 0
    B0 - 0
    Default: 0x00
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_PLL_LOCK_STATUS, register_value); 
    /*
    JESD204B quick config-uration
    B76543210 - JESD204B quick configuration L = number of lanes = 2Register 0x570, Bits[7:6] M = number of converters = 2Register 0x570, Bits[5:3] F = number of octets/frame = 2 Register 0x570, Bits[2:0]
    Default: 0x80 for AD9690-1000; 0x40 for AD9690-500
    Refer to Table 26 and Table 27  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_QUICK_CONFIGURATION, register_value); 
    /*
    JESD204B Link Mode Control 1
    B7 - Standby mode 0 = all converter outputs 0 1 = CGS (/K28.5/)
    B6 - Tail bit (t) PN 0 = disable 1 = enable T = N΄ − N − CS
    B5 - Long transport layer test 0 = disable 1 = enable
    B4 - Lane synch-ronization 0 = disable FACI uses /K28.7/ 1 = enable FACI uses /K28.3/ and /K28.7/
    B32 - ILAS sequence mode 00 = ILAS disabled 01 = ILAS enabled 11 = ILAS always on test mode
    B1 - FACI 0 = enabled 1 = disabled
    B0 - Link control 0 = active 1 = power down
    Default: 0x14  
    */  
    usleep(sleepTimeus); 
    register_value = 0x00;
    result = WriteRegister(JESD204B_LINK_MODE_CONTROL_1, register_value); 
    /*
    JESD204B Link Mode Control 2
    B76 - SYNCINB± pin control 00 = normal 10 = ignore SYNCINB± (force CGS) 11 = ignore SYNCINB± (force ILAS/user data)
    B5 - SYNCINB± pin invert 0 = active low 1 = active high
    B4 - SYNCINB± pin type 0 = differential 1 = CMOS
    B3 - 0
    B2 - 8-bit/10-bit bypass 0 = normal 1 = bypass
    B1 - 8-/10-bit bit invert 0 = normal 1 = invert the abcd efghij symbols
    B0 - 0
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LINK_MODE_CONTROL_2, register_value); 
    /*
    JESD204B Link Mode Control 3
    B76 - CHKSUM mode 00 = sum of all 8-bit link config registers 01 = sum of individual link config fields 10 = checksum set to zero
    B54 - Test injection point 00 = N΄ sample input 01 = 10-bit data at 8-bit/10-bit output (for PHY testing) 10 = 8-bit data at scrambler input
    B3210 - JESD204B test mode patterns 0000 = normal operation (test mode disabled) 0001 = alternating checker board 0010 = 1/0 word toggle 0011 = 31-bit PN sequence—X31 + X28 + 1 0100 = 23-bit PN sequence—X23 + X18 + 1 0101 = 15-bit PN sequence—X15 + X14 + 1 0110 = 9-bit PN sequence—X9 + X5 + 1 0111 = 7-bit PN sequence—X7 + X6 + 1 1000 = ramp output 1110 = continuous/repeat user test 1111 = single user test
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LINK_MODE_CONTROL_3, register_value); 
    /*
    JESD204B Link Mode Control 4
    B7654 - ILAS delay 0000 = transmit ILAS on first LMFC after SYNCINB± deasserted 0001 = transmit ILAS on second LMFC after SYNCINB± deasserted … 1111 = transmit ILAS on 16th LMFC after SYNCINB± deasserted
    B3 - 0
    B210 - Link layer test mode 000 = normal operation (link layer test mode disabled) 001 = continuous sequence of /D21.5/ characters 100 = modified RPAT test sequence 101 = JSPAT test sequence 110 = JTSPAT test sequence
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LINK_MODE_CONTROL_4, register_value); 
    /*
    JESD204B LMFC offset
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - LMFC phase offset value[4:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LMFC_OFFSET, register_value); 
    /*
    JESD204B DID config
    B76543210 - JESD204B Tx DID value[7:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_DID_CONFIG, register_value); 
    /*
    JESD204B BID config
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - JESD204B Tx BID value, Bits[3:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_BID_CONFIG, register_value); 
    /*
    JESD204B LID Config 1
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Lane 0 LID value, Bits[4:0]
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LID_CONFIG_1, register_value); 
    /*
    JESD204B LID Config 2
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Lane 1 LID value, Bits[4:0]
    Default: 0x01  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LID_CONFIG_2, register_value); 
    /*
    JESD204B LID Config 3
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Lane 2 LID value, Bits[4:0]
    Default: 0x01  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LID_CONFIG_3, register_value); 
    /*
    JESD204B LID Config 4
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Lane 3 LID value, Bits[4:0]
    Default: 0x03  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LID_CONFIG_4, register_value); 
    /*
    JESD204B parameters SCR/L
    B7 - JESD204B scrambling (SCR) 0 = disabled 1 = enabled
    B6 - 0
    B5 - 0
    B4 - 0
    B3 - 0
    B2 - 0
    B10 - JESD204B lanes (L) 00 = 1 lane 01 = 2 lanes 11 = 4 lanes Read only, see Register 0x570
    Default: 0x8X  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_PARAMETERS_SCR_L, register_value); 
    /*
    JESD204B F config
    B76543210 - Number of octets per frame, F = Register 0x58C[7:0] + 1
    Default: 0x88
    Read only, see Reg. 0x570  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_F_CONFIG, register_value); 
    /*
    JESD204B K config
    B7 - 0
    B6 - 0
    B5 - 0
    B43210 - Number of frames per multiframe, K = Register 0x58D[4:0] + 1. Only values where (F × K) mod 4 = 0 are supported
    Default: 0x1F
    See Reg. 0x570  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_K_CONFIG, register_value); 
    /*
    JESD204B M config
    B76543210 - Number of converters per link[7:0] 0x00 = link connected to one virtual converter (M = 1) 0x01 = link connected to two virtual converters (M = 2) 0x03 = link connected to four virtual converters (M = 4)
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_M_CONFIG, register_value); 
    /*
    JESD204B CS/N config
    B76 - Number of control bits (CS) per sample 00 = no control bits (CS = 0) 01 = 1 control bit (CS = 1); Control Bit 2 only 10 = 2 control bits (CS = 2); Control Bit 2 and 1 only 11 = 3 control bits (CS = 3); all control bits (2, 1, 0)
    B5 - 0
    B43210 - ADC converter resolution (N) 0x0D = 14-bit resolution 0x0F = 16-bit resolution  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_CS_N_CONFIG, register_value); 
    /*
    JESD204B N’ config
    B7 - 0
    B6 - 0
    B5 - Subclass support (Subclass V) 0 = Subclass 0 (no deter-ministic latency) 1 = Subclass 1
    B43210 - ADC number of bits per sample (N’) 0x7 = 8 bits 0xF = 16 bits
    Default: 0x2F  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_N_PRIM_CONFIG, register_value); 
    /*
    JESD204B S config
    B7 - 0
    B6 - 0
    B5 - 1
    B43210 - Samples per converter frame cycle (S) S value = Register 0x591[4:0] + 1
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_S_CONFIG, register_value); 
    /*
    JESD204B HD and CF config
    B7 - HD value 0 = disabled 1 = enabled
    B6 - 0
    B5 - 0
    B43210 - Control words per frame clock cycle per link (CF) CF value = Register 0x592, Bits[4:0]
    Default: 0x80
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_HD_AND_CF_CONFIG, register_value); 
    /*
    JESD204B CHKSUM 0
    B76543210 - CHKSUM value for SERDOUT0±, Bits[7:0]
    Default: 0x81
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_CHKSUM_0, register_value); 
    /*
    JESD204B CHKSUM 1
    B76543210 - CHKSUM value for SERDOUT1±, Bits[7:0]
    Default: 0x82
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_CHKSUM_1, register_value); 
    /*
    JESD204B CHKSUM 2
    B76543210 - CHKSUM value for SERDOUT2±, Bits[7:0]
    Default: 0x82
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_CHKSUM_2, register_value); 
    /*
    JESD204B CHKSUM 3
    B76543210 - CHKSUM value for SERDOUT3±, Bits[7:0]
    Default: 0x84
    Read only  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_CHKSUM_3, register_value); 
    /*
    JESD204B lane power-down
    B7 - 1
    B6 - SERD-OUT3± 0 = on 1 = off
    B5 - 1
    B4 - SERD-OUT2± 0 = on 1 = off
    B3 - 1
    B2 - SERD- OUT1± 0 = on 1 = off
    B1 - 1
    B0 - SERD-OUT0± 0 = on 1 = off
    Default: 0xAA  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LANE_POWERDOWN, register_value); 
    /*
    JESD204B lane SERDOUT0± assign
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3 - 0
    B210 - SERDOUT0± lane assignment 000 = Logical Lane 0 001 = Logical Lane 1 010 = Logical Lane 2 011 = Logical Lane 3
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LANE_SERDOUT_0_ASSIGN, register_value); 
    /*
    JESD204B lane SERDOUT1± assign
    B7 - X
    B6 - X
    B5 - X   
    B4 - X
    B3 - 0
    B210 - SERDOUT1± lane assignment 000 = Logical Lane 0 001 = Logical Lane 1 010 = Logical Lane 2 011 = Logical Lane 3
    Default: 0x11  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LANE_SERDOUT_1_ASSIGN, register_value); 
    /*
    JESD204B lane SERDOUT2± assign
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3 - 0
    B210 - SERDOUT2± lane assignment 000 = Logical Lane 0 001 = Logical Lane 1 010 = Logical Lane 2 011 = Logical Lane 3
    Default: 0x22  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LANE_SERDOUT_2_ASSIGN, register_value); 
    /*
    JESD204B lane SERDOUT3± assign
    B7 - X
    B6 - X
    B5 - X
    B4 - X
    B3 - 0
    B210 - SERDOUT3± lane assignment 000 = Logical Lane 0 001 = Logical Lane 1 010 = Logical Lane 2 011 = Logical Lane 3
    Default: 0x33  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD204B_LANE_SERDOUT_3_ASSIGN, register_value); 
    /*
    JESD serializer drive adjust
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - Swing voltage 0000 = 237.5 mV 0001 = 250 mV 0010 = 262.5 mV 0011 = 275 mV 0100 = 287.5 mV 0101 = 300 mV 0110 = 312.5 mV 0111 = 325 mV 1000 = 337.5 mV 1001 = 350 mV 1010 = 362.5 mV 1011 = 375 mV 1100 = 387.5 mV 1101 = 400 mV 1110 = 412.5 mV 1111 = 425 mV  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(JESD_SERIALIZER_DRIVE_ADJUST, register_value); 
    /*
    De-emphasis select
    B7 - 0
    B6 - SERD-OUT3± 0 = disable 1 = enable
    B5 - 0
    B4 - SERD-OUT2± 0 = disable 1 = enable
    B3 - 0
    B2 - SERDOUT1± 0 = disable 1 = enable
    B1 - 0
    B0 - SERDOUT0± 0 = disable 1 = enable
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DE_EMPHASIS_SELECT, register_value); 
    /*
    De-emphasis setting for SERDOUT0±
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - SERDOUT0± de-emphasis settings: 0000 = 0 dB, 0001 = 0.3 dB, 0010 = 0.8 dB, 0011 = 1.4 dB, 0100 = 2.2 dB, 0101 = 3.0 dB, 0110 = 4.0 dB, 0111 = 5.0 dB
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DE_EMPHASIS_SETTING_FOR_SERDOUT_0, register_value); 
    /*
    De-emphasis setting for SERDOUT1±
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - SERDOUT1± de-emphasis settings: 0000 = 0 dB, 0001 = 0.3 dB, 0010 = 0.8 dB, 0011 = 1.4 dB, 0100 = 2.2 dB, 0101 = 3.0 dB, 0110 = 4.0 dB, 0111 = 5.0 dB
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DE_EMPHASIS_SETTING_FOR_SERDOUT_1, register_value); 
    /*
    De-emphasis setting for SERDOUT2±
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - SERDOUT2± de-emphasis settings: 0000 = 0 dB, 0001 = 0.3 dB, 0010 = 0.8 dB, 0011 = 1.4 dB, 0100 = 2.2 dB, 0101 = 3.0 dB, 0110 = 4.0 dB, 0111 = 5.0 dB
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DE_EMPHASIS_SETTING_FOR_SERDOUT_2, register_value); 
    /*
    De-emphasis setting for SERDOUT3±
    B7 - 0
    B6 - 0
    B5 - 0
    B4 - 0
    B3210 - SERDOUT3± de-emphasis settings: 0000 = 0 dB, 0001 = 0.3 dB, 0010 = 0.8 dB, 0011 = 1.4 dB, 0100 = 2.2 dB, 0101 = 3.0 dB, 0110 = 4.0 dB, 0111 = 5.0 dB
    Default: 0x00  
    */  
    usleep(sleepTimeus);  
    register_value = 0x00;
    result = WriteRegister(DE_EMPHASIS_SETTING_FOR_SERDOUT_3, register_value); 


    return result;
}


unsigned short int ADC_AD9690 :: WriteRegister(unsigned short int reg, unsigned char val)
{
    unsigned short int result = 0;
    return result;
}


unsigned short int ADC_AD9690 :: ReadRegister(unsigned short int reg, unsigned char *val)
{
    unsigned short int result = 0;
    return result;
}


unsigned short int ADC_AD9690 :: ReadData(void)
{
    unsigned short int result = 0;
    return result;
}

