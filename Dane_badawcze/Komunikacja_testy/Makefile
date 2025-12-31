APP = sonda

INCLUDE = -I./sources -I./libs
LDLIBS += -lpthread -static app/libs/openssl/libssl.a -static app/libs/openssl/libcrypto.a -static app/libs/ffll/ffll.a

# Add any other object files to this list below
APP_OBJS = sources/INIReader.o sources/GNSS_MAX2771.o sources/main.o sources/SpectrumDetermination.o sources/probeMessagesEncoder.o sources/EventLogger.o sources/parser.o sources/I2C2SPI_SC18IS602.o sources/RadioTrack.o sources/GPIO_EXP_MCP23017.o sources/SignatureDetermination.o sources/CLKdist_LMK04610.o sources/Encryptor.o sources/ini.o sources/I2C_SWD_PCA9546.o sources/Status.o sources/SystemCommunication.o sources/RadioCard.o sources/PowerMeg_INA226.o sources/ADC_LTC2122.o sources/ADC_AD9690.o 

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CXX) $(INCLUDE) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)
