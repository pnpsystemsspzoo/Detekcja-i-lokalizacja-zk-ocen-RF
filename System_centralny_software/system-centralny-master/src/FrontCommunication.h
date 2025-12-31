#ifndef FRONT_COMMUNICATION_H
#define FRONT_COMMUNICATION_H

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#include <mutex>

#include <iostream>
#include <fstream>
#include "EventLogger.h"


using namespace std;

#define FRONT_COMMUNICATION_TX_BUFFER_SIZE_KB 280
#ifndef UNUSED
    #define UNUSED(x) (void)(x)
#endif // UNUSED
/*!
 * @brief Class responsible for communication with frontend application
 */
class FrontCommunication
{
    friend class FrontCommunicationTest;
    friend class Sender;

    private:
        char * tx_buffer; ///< Dynamically allocated buffer for transmit operations
        bool client_connected; ///< Saves last state of the client connection status
        int sockfd; ///< Socket file handler
        int connfd; ///< Connection file handler for TCP port
        struct sockaddr_in servaddr; ///< TCP server address
        struct sockaddr_in cli; ///< Connected client address
        std::mutex tx_mutex; ///< Mutex for concurrent message transmission
        std::mutex tx_buffer_mutex; ///< Mutex for concurrent tx_buffer usage
        /*!
         * @brief sets last known frontend client connection status and writes verbose log
         * @param is_connected new status to save
         */
        void setClientConnectedStatus(bool is_connected);

    public:
        long int port; ///< Connection port number at which TCP server will be listening. This is configuration value
        std::string ipaddr; ///< TCP server address. This is configuration value

    /*!
     * @brief Frontend communication events listener interface
     */
    class listener
    {
        public:
            /*!
             * @brief Callback function for incoming OPERATOR_INTERFACE_QUERY message
             * @param user_id, an identifier of the user who sent the message
             */
            virtual void onOperatorInterfaceQuery(uint8_t user_id) {UNUSED(user_id);}
            /*!
             * @brief Callback function for incoming ADMIN_INTERFACE_QUERY message
             * @param user_id, an identifier of the user who sent the message
             */
            virtual void onAdminInterfaceQuery(uint8_t user_id) {UNUSED(user_id);}
            /*!
             * @brief Callback function for incoming DEVICES_MANAGEMENT_QUERY message
             * @param user_id, an identifier of the user who sent the message
             */
            virtual void onDevicesManagementQuery(uint8_t user_id) {UNUSED(user_id);}
            /*!
             * @brief Callback function for incoming DEVICE_STATUS_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param device_id, device identifier for which status is requested
             */
            virtual void onDeviceStatusQuery(uint8_t user_id, uint16_t device_id) {UNUSED(user_id);UNUSED(device_id);}
            /*!
             * @brief Callback function for incoming ADD_DEVICE_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param license_buffer, buffer containing encrypted license for device that should be added to the system
             * @param license_size number of bytes inside license_buffer (license size in bytes)
             */
            virtual void onAddDeviceQuery(uint8_t user_id, const char *license_buffer, uint32_t license_size) {UNUSED(user_id);UNUSED(license_buffer);UNUSED(license_size);}
            /*!
             * @brief Callback function for incoming DELETE_DEVICE_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param device_id, identifier of the device which should be deleted
             */
            virtual void onDeleteDeviceQuery(uint8_t user_id, uint16_t device_id) {UNUSED(user_id);UNUSED(device_id);}
            /*!
             * @brief Callback function for incoming FFT_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param device_id, identifier of the device for which FFT stream is requested
             * @param channel, a radio card number for which FFT stream should be send
             * @param
             */
            virtual void onFFTQuery(uint8_t user_id, uint16_t device_id, uint8_t channel, bool repeat) {UNUSED(user_id);UNUSED(device_id);UNUSED(channel);UNUSED(repeat);}
            /*!
             * @brief Callback function for incoming STOP_DEVICE_STATUS_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param device_id, device identifier for which status stream should be stopped
             */
            virtual void onStopDeviceStatusQuery(uint8_t user_id, uint16_t device_id) {UNUSED(user_id);UNUSED(device_id);}
            /*!
             * @brief Callback function for incoming UPDATE_DEVICES_CONFIG_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param config_filename, a string containing requested configuration file name
             * @param device_ID, a vector containing device identifiers for which speciefied configuration should be updated
             */
            virtual void onUpdateDevicesConfigQuery(uint8_t user_id, const std::string & config_filename, const std::vector<uint16_t> & device_ID) {UNUSED(user_id);UNUSED(config_filename);UNUSED(device_ID);}
            /*!
             * @brief Callback function for incoming UPDATE_DEVICES_CONFIG_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param config_filename, a string containing requested configuration file name
             * @param device_ID, a vector containing device identifiers for which speciefied firmware should be updated
             */
            virtual void onUpdateDevicesFirmwareQuery(uint8_t user_id, const std::string & config_filename, const std::vector<uint16_t> & device_ID) {UNUSED(user_id);UNUSED(config_filename);UNUSED(device_ID);}
            /*!
             * @brief Callback function for incoming RESET_DEVICE_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param device_id, identifier of the device which should be restarted
             */
            virtual void onResetDeviceQuery(uint8_t user_id, uint16_t device_id) {UNUSED(user_id);UNUSED(device_id);}
            /*!
             * @brief Callback function for incoming GET_LOG_QUERY message
             * @param user_id, an identifier of the user who sent the message
             * @param device_id, identifier of the device from which log file should be downloaded
             */
            virtual void onGetLogQuery(uint8_t user_id, uint16_t device_id) {UNUSED(user_id);UNUSED(device_id);}
    };

    std::list<listener*> listeners; ///< List of frontend incoming messages listeners
    /*!
     * @brief Adds frontend messages listener to the listeners list
     * Each listener will be notified when new message from fronted is received
     * @param l, a pointer to the listener that should be added to the listeners list
     */
    void addListener(listener * l);
    /*!
     * @brief Removes frontend messages listener from the listeners list
     * @param l, a pointer to the listener that should be removed from the listeners list
     */
    void removeListener(listener * l);

    /*!
     * @brief Defines frontend communication messages and all related operations
     */
    class message
    {
        public:
            /*!
             * @brief Message identifier enumerator
             */
            enum ID
            {
                RADIO_INTERFERENCE=0x01,
                CS_STATUS=0x02,
                DEVICE_FULL_STATUS=0x03,
                DEVICE_SHORT_STATUS=0x04,
                DEVICE_LIST=0x05,
                FFT=0x06,
                ADD_DEVICE=0x07,
                DELETE_DEVICE=0x08,
                NEW_CONFIGURATION_UPLOADED=0x09,
                CONFIGURATION_LOAD_STATUS=0x0A,
                NEW_FIRMWARE_UPLOADED=0x0B,
                FIRMWARE_LOAD_STATUS=0x0C,
                ALERT=0x0D,
                GET_LOG_RESPONSE=0x0E,
                OPERATOR_INTERFACE_QUERY=0x80,
                ADMIN_INTERFACE_QUERY=0x81,
                DEVICES_MANAGEMENT_QUERY=0x82,
                DEVICE_STATUS_QUERY=0x83,
                ADD_DEVICE_QUERY=0x84,
                DELETE_DEVICE_QUERY=0x85,
                FFT_QUERY=0x86,
                STOP_DEVICE_STATUS_QUERY=0x87,
                UPDATE_DEVICES_CONFIG_QUERY=0x88,
                UPDATE_DEVICES_FIRMWARE_QUERY=0x89,
                RESET_DEVICE_QUERY=0x8A,
                GET_LOG_QUERY=0x8B
            };
            /*!
             * @brief RADIO_INTERFERENCE message structure
             */
            struct radio_interference
            {
                uint32_t signal_ID;
                double longitude;
                double latitude;
                uint16_t height;
                uint16_t range;
                uint16_t heading;
                uint16_t speed;
            };
            /*!
             * @brief CS_STATUS message structure
             */
            struct cs_status
            {
                uint8_t health;
                uint8_t communication_status;
                uint8_t CPU_usage;
                uint8_t RAM_usage;
                uint8_t disk_usage;
                uint16_t tasks_number;
            };
            /*!
             * @brief Helper structure for RADIO_INTERFERENCE message structure.
             * Defines threat data
             */
            struct threat_info
            {
                uint16_t bandwidth;
                uint16_t f0;
                uint8_t threat_type;
            };
            /*!
             * @brief DEVICE_FULL_STATUS message structure
             */
            struct dev_full_status
            {
                uint16_t device_ID;
                uint8_t health;
                uint16_t motherboard_voltage;
                uint16_t motherboard_current;
                uint8_t cards;
                uint16_t card_voltage[5];
                uint16_t card_current[5];
                uint16_t cards_health;
                uint32_t alerts;
                uint8_t radio_interference;
                uint8_t communication_status;
                uint8_t connected;
                uint8_t CPU_usage;
                uint8_t RAM_usage;
                uint8_t disk_usage;
                uint32_t tx_transfer_rate;
                uint32_t rx_transfer_rate;
                uint16_t tasks_number;
                uint8_t synchro_status;
                uint8_t time_std;
                char datetime[25];
                uint8_t threats_length;
                std::unique_ptr<threat_info[]> threats_table;
            };
            /*!
             * @brief DEVICE_SHORT_STATUS message structure
             */
            struct dev_short_status
            {
                uint16_t device_ID;
                uint8_t health;
                uint8_t communication_status;
                uint8_t radio_interference;

            };
            /*!
             * @brief Helper structure for DEVICE_LIST message structure.
             * Defines each device data structure
             */
            struct dev_info
            {
                uint16_t device_ID;
                std::string device_version;
                std::string device_IP;
                uint8_t MAC[18];
                double longitude;
                double latitude;
                uint16_t height;
                uint8_t connected;
                uint8_t health;
                uint16_t data_port;
                uint16_t control_port;
                std::string creation_date;
                std::string expiration_date;
                std::string code;
                std::string who;
                uint8_t firstmess;
                std::vector<std::string> card_name;
            };
            /*!
             * @brief FFT message structure
             */
            struct fft
            {
                uint16_t device_ID;
                uint8_t channel;
                uint16_t f_begin; //kHz
                uint16_t f_end; //kHz
                uint16_t data_number;
                std::unique_ptr<int16_t[]> power_density;
            };
            /*!
             * @brief ADD_DEVICE message structure
             */
            struct add_device
            {
                uint8_t status;
                dev_info device_info;
            };
            /*!
             * @brief DELETE_DEVICE message structure
             */
            struct delete_device
            {
                uint16_t device_ID;
                uint8_t status;
            };
            /*!
             * @brief NEW_CONFIGURATION_UPLOADED or NEW_FIRMWARE_UPLOADED message structure
             */
            struct new_file_uploaded
            {
                uint8_t user_ID;
                uint8_t status;
                std::string filename;
            };
            /*!
             * @brief Helper structure for CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS message structure.
             * Defines device response data structure
             */
            struct device_operation_response
            {
                uint16_t device_ID;
                uint8_t status;
            };
            /*!
             * @brief CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS message structure
             */
            struct load_status
            {
                std::string filename;
                uint16_t device_length;
                std::unique_ptr<device_operation_response[]> device_status;
            };
            /*!
             * @brief ALERT message structure
             */
            struct alert_msg
            {
                enum SCOPE {
                    INFO    = (0 << 13),
                    WARNING = (1 << 13),
                    ERROR   = (2 << 13)
                }; ///< ENUMERATED alerts scopes
                enum ID {
                    UNKNOWN = 0,
                    RESET_DEVICE = 1
                }; ///< Enumerated alerts ID's

                uint16_t alert_ID;
                std::string timestamp;
                std::string message;
                /*!
                 * @brief Parametrized constructor. Fills timestamp automatically to current time.
                 * @param alert_ID ID of the alert. Should be used from alert_msg::ID enumerator.
                 * Can be OR'ed with alert_msg::SCOPE. If not passed default value is INFO | UNKNOWN.
                 */
                alert_msg(uint16_t alert_ID = INFO | UNKNOWN);
            };
            /*!
             * @brief GET_LOG_RESPONSE message structure
             */
            struct get_log_response
            {
                uint8_t status;
                uint16_t device_ID;
                std::string log_file_name;
            };
            /*!
             * @brief Packs double value to the output buffer and preserves big endian notation
             * @param val double value to pack
             * @param buff pointer to the buffer where data should be written
             * @return incremented buffer pointer by size of the written data. This is the address in buff where next data can be written.
             */
            static char * pack_double(const double & val, char * buff);
            /*!
             * @brief Unpacks double value from the buffer and preserves big endian notation
             * @param buff pointer to the buffer from where data should be decoded
             * @return decoded double number
             */
            static double unpack_double(const char * buff);
            /*!
             * @brief Packs int32_t value to the output buffer and preserves big endian notation
             * @param val int32_t value to pack
             * @param buff pointer to the buffer where data should be written
             * @return incremented buffer pointer by size of the written data. This is the address in buff where next data can be written.
             */
            static char * pack_int32(const int32_t & val, char * buff);
            /*!
             * @brief Unpacks int32_t value from the buffer and preserves big endian notation
             * @param buff pointer to the buffer from where data should be decoded
             * @return decoded int32_t number
             */
            static int32_t unpack_int32(const char * buff);
            /*!
             * @brief Packs int16_t value to the output buffer and preserves big endian notation
             * @param val int16_t value to pack
             * @param buff pointer to the buffer where data should be written
             * @return incremented buffer pointer by size of the written data. This is the address in buff where next data can be written.
             */
            static char * pack_int16(const int16_t & val, char * buff);
            /*!
             * @brief Unpacks int16_t value from the buffer and preserves big endian notation
             * @param buff pointer to the buffer from where should be decoded
             * @return decoded int16_t number
             */
            static int16_t unpack_int16_t(const char * buff);
            /*!
             * @brief Packs dev_info structure to the output buffer and preserves big endian notation
             * @param dinfo dev_info structure to pack
             * @param buff pointer to the buffer where data should be written
             * @return incremented buffer pointer by size of the written data. This is the address in buff where next data can be written.
             */
            static char * pack_device_info(const dev_info & dinfo, char * buff);
            /*!
             * @brief Unpacks dev_info structure from the buffer and preserves big endian notation
             * @param buff pointer to the buffer from where data should be decoded
             * @param dinfo pointer to the dev_info structure where decoded data should be written
             * @return incremented buffer pointer by size of the read data. This is the address in buff where next data can be read.
             */
            static const char *unpack_device_info(const char * buff, dev_info * dinfo);
            /*!
             * @brief Packs std::string to the output buffer and preserves big endian notation
             * @param str std::string to pack
             * @param buff pointer to the buffer where data should be written
             * @return incremented buffer pointer by size of the written data. This is the address in buff where next data can be written.
             */
            static char * packString(const std::string & str, char * buff);
            /*!
             * @brief Unpacks std::string from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param output pointer to the std::string where decoded data should be written
             * @return incremented buffer pointer by size of the read data. This is the address in buff where next data can be read.
             */
            static const char * unpack_string(const char * buff,std::string * output);
            /*!
             * @brief Packs device_operation_response structure to the output buffer and preserves big endian notation
             * @param response a device_operation_response structure to pack
             * @param buff pointer to the buffer where data should be written
             * @return incremented buffer pointer by size of the written data. This is the address in buff where next data can be written.
             */
            static char * pack_operation_response(const FrontCommunication::message::device_operation_response &response, char * buff);
            /*!
             * @brief Unpacks device_operation_response structure from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param output pointer to the device_operation_response structure where decoded data should be written
             * @return incremented buffer pointer by size of the read data. This is the address in buff where next data can be read.
             */
            static const char *unpack_operation_response(const char * buff, FrontCommunication::message::device_operation_response *response);
            /*!
             * @brief Packs RADIO_INTERFERENCE message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a radio_interference structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::radio_interference & msg);
            /*!
             * @brief Unpacks RADIO_INTERFERENCE message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the radio_interference structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::radio_interference * msg);
            /*!
             * @brief Packs CS_STATUS message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a cs_status structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::cs_status & msg);
            /*!
             * @brief Unpacks CS_STATUS message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the cs_status structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::cs_status * msg);
            /*!
             * @brief Packs DEVICE_FULL_STATUS message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a dev_full_status structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::dev_full_status &msg);
            /*!
             * @brief Unpacks DEVICE_FULL_STATUS message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the dev_full_status structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::dev_full_status *msg);
            /*!
             * @brief Packs DEVICE_SHORT_STATUS message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a dev_short_status structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::dev_short_status &msg);
            /*!
             * @brief Unpacks DEVICE_SHORT_STATUS message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the dev_short_status structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::dev_short_status *msg);
            /*!
             * @brief Packs DEVICE_LIST message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param devices a pointer to the dev_info array
             * @param n number of items in devices array
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::dev_info * devices, uint16_t n);
            /*!
             * @brief Unpacks DEVICE_LIST message from the buffer
             * This function will automatically allocate required array to hold all decoded data. The dynamically allocated buffer must released manually.
             * @param buff pointer to the buffer from where data should be decoded
             * @param devices a double pointer to the dev_info structure where decoded data will be written.
             * @param n a pointer to the variable where number of written dev_info items will be stored.
             */
            static void unpackMessage(const char * buff, message::dev_info **devices, uint16_t *n);
            /*!
             * @brief Packs FFT message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a fft structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::fft & msg);
            /*!
             * @brief Unpacks FFT message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the fft structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::fft * msg);
            /*!
             * @brief Packs ADD_DEVICE message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a add_device structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::add_device & msg);
            /*!
             * @brief Unpacks ADD_DEVICE message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the add_device structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::add_device * msg);
            /*!
             * @brief Packs DELETE_DEVICE message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a delete_device structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::delete_device & msg);
            /*!
             * @brief Unpacks DELETE_DEVICE message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the delete_device structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::delete_device * msg);
            /*!
             * @brief Packs NEW_CONFIGURATION_UPLOADED or NEW_FIRMWARE_UPLOADED message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a new_file_uploaded structure to pack
             * @param frame_id frame identifier. Can be NEW_CONFIGURATION_UPLOADED or NEW_FIRMWARE_UPLOADED
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::new_file_uploaded & msg, uint8_t frame_id);
            /*!
             * @brief Unpacks NEW_CONFIGURATION_UPLOADED or NEW_FIRMWARE_UPLOADED message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the new_file_uploaded structure where decoded data should be written.
             * @param frame_id a pointer to the variable where decoded frame identifier will be stored
             */
            static void unpackMessage(const char * buff, message::new_file_uploaded * msg, uint8_t *frame_id);
            /*!
             * @brief Packs CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a load_status structure to pack
             * @param frame_id frame identifier. Can be CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::load_status & msg, uint8_t frame_id);
            /*!
             * @brief Unpacks CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the load_status structure where decoded data should be written.
             * @param frame_id a pointer to the variable where decoded frame identifier will be stored
             */
            static void unpackMessage(const char * buff, message::load_status * msg, uint8_t *frame_id);
            /*!
             * @brief Packs ALERT message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a load_status structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::alert_msg & msg);
            /*!
             * @brief Unpacks ALERT message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the alert_msg structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::alert_msg * msg);
            /*!
             * @brief Packs GET_LOG_RESPONSE message to the output buffer and preserves big endian notation
             * @param buff a pointer to the buffer where packed message should be written
             * @param msg a load_status structure to pack
             * @return Number of bytes written to the output buffer
             */
            static uint32_t packMessage(char * buff, const message::get_log_response & msg);
            /*!
             * @brief Unpacks ALERT message from the buffer
             * @param buff pointer to the buffer from where data should be decoded
             * @param msg a pointer to the alert_msg structure where decoded data should be written.
             */
            static void unpackMessage(const char * buff, message::get_log_response * msg);
    };
    /*!
     * @brief Thread safe class used to send messages to the frontend
     */
    class Sender {

        FrontCommunication * frontCommunication;

        public:
        /*!
         * @brief Constructor
         * @param parent a pointer to the FrontCommunication instance that controls TCP server connection with the fronted
         */
        Sender(FrontCommunication * parent);
        /*!
         * @brief Checks if frontend client is connected to the TCP server.
         * @details This function should be called before SendMessage to avoid unnecessery warning logs when trying to send message to not present frontend
         * @return True if client is connected. Otherwise false
         */
        bool isClientConneted() { return frontCommunication->isClientConneted(); }
        /*!
         * @brief Sends arbitrary data to the connected client
         * @param buff a pointer to the buffer which should be transmitted
         * @param n number of bytes to send. If buff points to the string terminated with '\0' character this argument can be omitted
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(char * buff, int n=-1);
        /*!
         * @brief Sends RADIO_INTERFERENCE message to the connected client
         * @param msg a radio_interference structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::radio_interference & msg);
        /*!
         * @brief Sends CS_STATUS message to the connected client
         * @param msg a cs_status structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::cs_status & msg);
        /*!
         * @brief Sends DEVICE_FULL_STATUS message to the connected client
         * @param msg a dev_full_status structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::dev_full_status &msg);
        /*!
         * @brief Sends DEVICE_SHORT_STATUS message to the connected client
         * @param msg a dev_short_status structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::dev_short_status &msg);
        /*!
         * @brief Sends DEVICE_LIST message to the connected client
         * @param devices a pointer to the dev_info array to send
         * @param n number of items in devices array
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::dev_info * devices, uint16_t n);
        /*!
         * @brief Sends FFT message to the connected client
         * @param msg a fft structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::fft & msg);
        /*!
         * @brief Sends ADD_DEVICE message to the connected client
         * @param msg a add_device structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::add_device & msg);
        /*!
         * @brief Sends DELETE_DEVICE message to the connected client
         * @param msg a delete_device structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::delete_device & msg);
        /*!
         * @brief Sends NEW_CONFIGURATION_UPLOADED or NEW_FIRMWARE_UPLOADED message to the connected client
         * @param msg a new_file_uploaded structure to send
         * @param frame_id a frame identifier. Can be NEW_CONFIGURATION_UPLOADED or NEW_FIRMWARE_UPLOADED
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::new_file_uploaded & msg, uint8_t frame_id);
        /*!
         * @brief Sends CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS message to the connected client
         * @param msg a load_status structure to send
         * @param frame_id a frame identifier. Can be CONFIGURATION_LOAD_STATUS or FIRMWARE_LOAD_STATUS
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::load_status & msg, uint8_t frame_id);
        /*!
         * @brief Sends ALERT message to the connected client
         * @param msg a alert_msg structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::alert_msg & msg);
        /*!
         * @brief Sends GET_LOG_RESPONSE message to the connected client
         * @param msg a get_log_response structure to send
         * @return Number of bytes written to the transmit buffer
         */
        int SendMessage(const message::get_log_response & msg);
    };
    Sender sender; ///< Used to send data to the frontend client. Thread safe
    /*!
     * @brief Default constructor
     */
    FrontCommunication();
    ~FrontCommunication();
    /*!
     * @brief Creates TCP server and starts listen on the port specified in FrontCommunication::port
     * @return True if successfully created TCP server. Otherwise false
     */
    bool InitConnection(void);
    /*!
     * @brief Checks if TCP server is up and listening to the incoming connenctions
     * @return True if server is listening. Otherwise false
     */
    bool isListening(void);
    /*!
     * @brief Accepts requested client connection to the TCP server
     * @return Any positive number if client was successfully accepted
     */
    int AcceptClientMessage(void);
    /*!
     * @brief Checks if the new message has come from frondend client and reads it
     * @param buff a output buffer where received message should be written
     * @return Number of bytes written to the output buffer
     */
    int ReceiveMessage(char * buff);
    /*!
     * @brief Decodes saved prior client message and notifies all listeners about new message
     * @param buff a output buffer where received message should be written
     * @return Number of bytes written to the output buffer
     */
    void ParseUserMessage(const char *buff);
    /*!
     * @brief Disconnects client and closes TCP server
     * @return 1 if successfully closed TCP server
     */
    int StopConnection(void);
    /*!
     * @brief Checks if frontend client is connected to the TCP server
     * @return True if client is connected. Otherwise false
     */
    bool isClientConneted() { return client_connected; }
};
  
#endif // FRONT_COMMUNICATION_H
