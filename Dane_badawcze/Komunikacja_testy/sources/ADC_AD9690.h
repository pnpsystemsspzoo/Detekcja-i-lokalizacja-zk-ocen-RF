#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

//Analog Device SPI Registers
#define INTERFACE_CONFIG_A                         0x000
#define INTERFACE_CONFIG_B                         0x001
#define DEVICE_CONFIG                              0x002
#define CHIP_TYPE                                  0x003
#define CHIP_ID_L                                  0x004
#define CHIP_ID_H                                  0x005
#define CHIP_GRADE                                 0x006
#define DEVICE_INDEX                               0x008
#define SCRATCH_PAD                                0x00A
#define SPI_REVISION                               0x00B
#define VENDOR_ID_L                                0x00C
#define VENDOR_ID_H                                0x00D
//Analog Input Buffer Control Registers
#define ANALOG_INPUT                               0x015
#define INPUT_TERMINATION                          0x016
#define INPUT_CAPACITANCE                          0x934
#define BUFFER_CONTROL_1                           0x018
#define BUFFER_CONTROL_2                           0x019
#define BUFFER_CONTROL_3                           0x01A
#define BUFFER_CONTROL_4                           0x11A
#define BUFFER_CONTROL_5                           0x935
#define INPUT_FULL_SCALE_RANGE                     0x025
#define INPUT_FULL_SCALE_CONTROL                   0x030
//ADC Function Registers
#define V_1P0_CONTROL                              0x024
#define TEMPERATURE_DIODE                          0x028
#define PDWN_STBY_PIN_CONTROL                      0x03F
#define CHIP_PIN_CONTROL                           0x040
#define CLOCK_DIVIDER                              0x10B
#define CLOCK_DIVIDER_PHASE                        0x10C
#define CLOCK_DIVIDER_AND_SYSREF_CONTROL           0x10D
#define CLOCK_DELAY_CONTROL                        0x117
#define CLOCK_FINE_DELAY                           0x118
#define CLOCK_STATUS                               0x11C
#define SYSREF_CONTROL_1                           0x120
#define SYSREF_CONTROL_2                           0x121
#define SYSREF_TIMESTAMP_DELAY_CONTROL             0x123
#define SYSREF_STATUS_1                            0x128
#define SYSREF_AND_CLOCK_DIVIDER_STATUS            0x129
#define SYSREF_COUNTER                             0x12A
#define CHIP_SYNC_MODE                             0x1FF
#define CHIP_APPLICATION_MODE                      0x200
#define CHIP_DECIMATION_RATIO                      0x201
#define CUSTOMER_OFFSET                            0x228
#define FAST_DETECT_CONTROL                        0x245
#define FD_UPPER_THRESHOLD_LSB                     0x247
#define FD_UPPER_THRESHOLD_MSB                     0x248
#define FD_LOWER_THRESHOLD_LSB                     0x249
#define FD_LOWER_THRESHOLD_MSB                     0x24A
#define FD_DWELL_TIME_LSB                          0x24B
#define FD_DWELL_TIME_MSB                          0x24C
#define SIGNAL_MONITOR_SYBCHRONIZATION_CONTROL     0x26F
#define SIGNAL_MONITOR_CONTROL                     0x270
#define SIGNAL_MONITOR_PERIOD_REGISTER_0           0x271
#define SIGNAL_MONITOR_PERIOD_REGISTER_1           0x272
#define SIGNAL_MONITOR_PERIOD_REGISTER_2           0x273
#define SIGNAL_MONITOR_RESULT_CONTROL              0x274
#define SIGNAL_MONITOR_RESULT_REGISTER_0           0x275
#define SIGNAL_MONITOR_RESULT_REGISTER_1           0x276
#define SIGNAL_MONITOR_RESULT_REGISTER_2           0x277
#define SIGNAL_MONITOR_PERIOD_COUNTER_RESULT       0x278
#define SIGNAL_MONITOR_SPORT_OVER_JESD204B_CONTROL 0x279
#define SPORT_OVER_JESD204B_INPUT_SELECTION        0x27A
//DDC Function Registers
#define DDC_SYNCH_CONTROL                          0x300
#define DDC_0_CONTROL                              0x310
#define DDC_0_INPUT_SELECTION                      0x311
#define DDC_0_FREQUENCY_LSB                        0x314
#define DDC_0_FREQUENCY_MSB                        0x315
#define DDC_0_PHASE_LSB                            0x320
#define DDC_0_PHASE_MSB                            0x321
#define DDC_0_OUTPUT_TEST_MODE_SELECTION           0x327
#define DDC_1_CONTROL                              0x330
#define DDC_1_INPUT_SELECTION                      0x331
#define DDC_1_FREQUENCY_LSB                        0x334
#define DDC_1_FREQUENCY_MSB                        0x335
#define DDC_1_PHASE_LSB                            0x340
#define DDC_1_PHASE_MSB                            0x341
#define DDC_1_OUTPUT_TEST_MODE_SELECTION           0x347
//Digital Outputs and Test Modes
#define ADC_TEST_MODES                             0x550
#define USER_PATTERN_1_LSB                         0x551
#define USER_PATTERN_1_MSB                         0x552
#define USER_PATTERN_2_LSB                         0x553
#define USER_PATTERN_2_MSB                         0x554
#define USER_PATTERN_3_LSB                         0x555
#define USER_PATTERN_3_MSB                         0x556
#define USER_PATTERN_4_LSB                         0x557
#define USER_PATTERN_4_MSB                         0x558
#define OUTPUT_MODE_CONTROL_1                      0x559
#define OUTPUT_MODE_CONTROL_2                      0x55A
#define OUTPUT_MODE                                0x561
#define OUTPUT_AVERAGE_OR_CLEAR                    0x562
#define OUTPUT_OR_STATUS                           0x563
#define JESD204B_LANE_RATE_CONTROL                 0x56E
#define JESD204B_PLL_LOCK_STATUS                   0x56F
#define JESD204B_QUICK_CONFIGURATION               0x570
#define JESD204B_LINK_MODE_CONTROL_1               0x571
#define JESD204B_LINK_MODE_CONTROL_2               0x572
#define JESD204B_LINK_MODE_CONTROL_3               0x573
#define JESD204B_LINK_MODE_CONTROL_4               0x574
#define JESD204B_LMFC_OFFSET                       0x578
#define JESD204B_DID_CONFIG                        0x580
#define JESD204B_BID_CONFIG                        0x581
#define JESD204B_LID_CONFIG_1                      0x583
#define JESD204B_LID_CONFIG_2                      0x584
#define JESD204B_LID_CONFIG_3                      0x585
#define JESD204B_LID_CONFIG_4                      0x586
#define JESD204B_PARAMETERS_SCR_L                  0x58B
#define JESD204B_F_CONFIG                          0x58C
#define JESD204B_K_CONFIG                          0x58D
#define JESD204B_M_CONFIG                          0x58E
#define JESD204B_CS_N_CONFIG                       0x58F
#define JESD204B_N_PRIM_CONFIG                     0x590
#define JESD204B_S_CONFIG                          0x591
#define JESD204B_HD_AND_CF_CONFIG                  0x592
#define JESD204B_CHKSUM_0                          0x5A0
#define JESD204B_CHKSUM_1                          0x5A1
#define JESD204B_CHKSUM_2                          0x5A2
#define JESD204B_CHKSUM_3                          0x5A3
#define JESD204B_LANE_POWERDOWN                    0x5B0
#define JESD204B_LANE_SERDOUT_0_ASSIGN             0x5B2
#define JESD204B_LANE_SERDOUT_1_ASSIGN             0x5B3
#define JESD204B_LANE_SERDOUT_2_ASSIGN             0x5B5
#define JESD204B_LANE_SERDOUT_3_ASSIGN             0x5B6
#define JESD_SERIALIZER_DRIVE_ADJUST               0x5BF
#define DE_EMPHASIS_SELECT                         0x5C1
#define DE_EMPHASIS_SETTING_FOR_SERDOUT_0          0x5C2
#define DE_EMPHASIS_SETTING_FOR_SERDOUT_1          0x5C3
#define DE_EMPHASIS_SETTING_FOR_SERDOUT_2          0x5C4
#define DE_EMPHASIS_SETTING_FOR_SERDOUT_3          0x5C5


class ADC_AD9690
{
    private:
    public:
    ADC_AD9690();
    ~ADC_AD9690();
    unsigned short int Init(void);
    unsigned short int WriteRegister(unsigned short int reg, unsigned char val);
    unsigned short int ReadRegister(unsigned short int reg, unsigned char *val);
    unsigned short int ReadData(void);
};

