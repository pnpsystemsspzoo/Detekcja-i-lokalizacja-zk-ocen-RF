#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <thread>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "EventLogger.h"
#include "Encryptor.h"
#include "DeviceDataBase.h"
#include "Device.h"
#include <sys/stat.h>
#include "json.h"
#include "ActiveThreatsManager.h"

// receive static buffer size in bytes
#define InBuffSize  (10*1024)
// transmit static buffer size in bytes
#define OutBuffSize (10*1024)

/*!
 * @brief Manages interaction between frontend user and server
 */
class UserInterface : public FrontCommunication::listener
{
    friend class UserInterfaceTest;
    friend class UploadMonitor;

    private:

    char InBuff[InBuffSize]; ///< Receive buffer
    char OutBuff[OutBuffSize]; ///< Transmit buffer
    std::string share_dir; ///< Frontend share folder directory
    DeviceDataBase * device_db; ///< devices databese manager used to add or remove device when received user request
    vector<Device> * devices; ///< Vector containing all cached devices info
    ActiveThreatsManager * active_threats_manager; ///< Pointer to the ActiveThreatsManager object. Used when creating new Device object during ADD_DEVICE_QUERY

    /*!
     * @brief Finds device by the given device id in devices cache
     * @param device_id id of the device to find
     * @return pointer to the Device object if found. Otherwise returns nullptr
     */
    Device * findDeviceByID(uint16_t device_id);
    /*!
     * @brief Makes directory. Helper method
     * @param new_directory_path a path to the newly created directory
     * @return True if success. Otherwise false and error that occoured can be read from errno variable.
     */
    static bool createDir(const char * new_directory_path) { return mkdir(new_directory_path, 0777) == 0; }
    /*!
     * @brief checks if the specified directory exists on the filesystem
     * @param dir a directory path to check
     * @return true if directory exists. Otherwise false
     */
    static bool checkDirExists(const char * dir);
    /*!
     * @brief Removes directory recursively. Helper method
     * @param dirname a path to the directory that should be removed ( and all files inside it )
     * @return True if successfully removed directory. Otherwise false and error that occoured can be read from errno variable.
     */
    static bool removeDir(const char * dirname);
    /*!
     * @brief Controls and validates firmware and configuration files uploaded by the frontend user
     */
    class UploadMonitor
    {
        std::string entry_dir; ///< Entry directory where newly uploaded files are temporarly stored before validation
        std::string firmware_dir; ///< Firmware directory where successfully validated firmware files should be moved
        std::string config_dir; ///< Configuration directory where successfully validated configuration files should be moved
        UserInterface * ui; ///< Pointer to the UserInterface
        bool work; ///< Monitor watches entry_dir as long as this variable is set to true. This variable is modyfied through API.
        thread worker_thread; ///< Handler for monitor thread
        /*!
         * @brief Monitor thread main function
         * @return Exit code. 0 if no errors
         */
        int worker();
        /*!
         * @brief Validates firmware file
         * @param filename absolute path to the file which should be validated
         * @return true if file successfully validated. Otherwise false
         */
        bool validateFirmware(const char * filename);
        /*!
         * @brief Validates configuration file
         * @param filename absolute path to the file which should be validated
         * @return true if file successfully validated. Otherwise false
         */
        bool validateConfig(const char * filename);
        /*!
         * @brief Reads chosen directory content
         * @param dir path to the folder which content should be read
         * @param result pointer to the vector where read file names should be stored. This will only hold file names without the path
         * @return True if successfully read directory content. Otherwise false
         */
        bool readDir(const char * dir, std::vector<std::string> * result);
        /*!
         * @brief Creates/updates JSON file with the list of the files inside speciefied directory.
         * These JSON files are used by frontend application in order to retrieve list of the stored firmware and configuration files
         * @param dir
         * @param filename
         */
        void generateJSONlist(const char * dir, const char *filename);
        /*!
         * @brief Generates ISO-8601 formatted datetime string for speciefied file creation time
         * @param filepath path to the file for which ISO-8601 creation datetime should be returned
         * @return std::string containing ISO-8601 formatted file creation datetime
         */
        std::string getFileISOdatetime(const char * filepath);
        /*!
         * @brief Checks if new files were placed inside temporary upload directory (entry dir) and tries to validate them.
         * Not validated files are automatically removed from the file system.
         * @param upload_dir path to the directory where newly uploaded firmware or configuration files are stored before validation
         * @param destination_dir path to the directory where successfully validated files should be moved
         * @param action defines type of the files to valide. Can be FrontCommunication::message::NEW_FIRMWARE_UPLOADED or FrontCommunication::message::NEW_CONFIGURATION_UPLOADED
         */
        void checkFolder(std::string & upload_dir, std::string & destination_dir, int action);
        /*!
         * @brief Helper method to retrive folder name where speciefied by the path parameter file is saved on the file system
         * @param path a path to the file for which parent folder name should be returned
         * @return name of the folder where speciefied by the path argument file is stored
         */
        std::string getParentFolder(std::string & path) {return path.substr(path.find_last_of("/")+1);}

        public:
            /*!
             * @brief Creates and starts new thread for monitoring uploaded by the frontend user files
             */
            void start();
            /*!
             * @brief Sets directory path where newly uploaded by the frontend user files should be looked for
             * Newly uploaded files are stored inside temporary folder called entry dir before they are validated
             * @param entry_dir a path to the entry dir where newly uploaded by the frontend user files will be stored
             */
            void setEntryDir(const char * entry_dir) {this->entry_dir = entry_dir;}
            /*!
             * @brief gets upload folder path
             * @return upload folder path
             */
            const char * getEntryDir(void) { return this->entry_dir.c_str();}
            /*!
             * @brief Sets directory path where successfully validated firmware files will be stored.
             * Note, each newly uploaded but not successfully validated firmware file will be automatically removed from the file system
             * @param firmware_dir a path to the directory where successfully validated firmware files will be moved from temporary entry dir
             */
            void setFirmwareDir(const char * firmware_dir) {this->firmware_dir = firmware_dir;}
            /*!
             * @brief Gets directory path where successfully validated firmware files are stored
             * @return reference to string containing firmware directory path
             */
            const std::string & getFirwareDir() const { return this->firmware_dir; }
            /*!
             * @brief Sets directory path where successfully validated configuration files will be stored.
             * Note, each newly uploaded but not successfully validated configuration file will be automatically removed from the file system
             * @param config_dir a path to the directory where successfully validated configuration files will be moved from temporary entry dir
             */
            void setConfigDir(const char * config_dir) {this->config_dir = config_dir;}
            /*!
             * @brief Gets directory path where successfully validated configuration files are stored
             * @return reference to string containing config directory path
             */
            const std::string & getConfigDir() const { return this->config_dir; }
            /*!
             * @brief Creates JSON file in the specified directory based on the given JSON
             * @param dir a string filename with directory (full path) where to store file in the filesystem
             * @param json a reference to the nlohmann::json object to be stringified and stored in json file
             */
            void createJSONfile(const char * dir, const nlohmann::json & json);
            /*!
             * @brief Parameterized constructor
             * @param ui a pointer to the UserInterface class which manages communication with the forntend user. This parameter is stored in order to send automatic upload status notification to the user.
             */
            UploadMonitor(UserInterface* ui);
            ~UploadMonitor();
    };

    UploadMonitor uploadMonitor; ///< Monitors and validates uploaded by the frontend user firmware and configuration files

    public:

        FrontCommunication frontCommunication; ///< Sets TCP server for frontend client connection
        Encryptor licenseParser; ///< Validates encoded license file
        uint16_t full_status_device_id; ///< Holds latest device id of the user message DEVICE_FULL_STATUS_QUERY
        uint16_t short_status_device_id = 0; ///< Holds latest device id of the user message DEVICE_SHORT_STATUS_QUERY
        bool loop_message_DEVICE_FULL_STATUS; ///< If set to true, will automatically stream DEVICE_FULL_STATUS message for the device id saved in full_status_device_id

        // Variables used in mock version
        bool OPERATOR_INTERFACE_QUERY_received;
        bool loop_message_CS_STATUS;
        bool loop_message_DEVICE_SHORT_STATUS;
        bool send_message_DEVICE_LIST;

        bool send_message_FFT;
        bool loop_message_FFT;
        bool send_message_ADD_DEVICE;
        bool send_message_DELETE_DEVICE;
        bool send_message_CONFIGURATION_LOAD_STATUS;
        bool send_message_FIRMWARE_LOAD_STATUS;

        /*!
         * @brief Creates folder for the given device_ID
         * @param device_ID probe id
         */
        void createProbeDir(uint16_t device_ID);
        /*!
         * @brief creates folders for all probes if they don't exists
         */
        void createProbesDirectories();

        /*!
         * @brief Parametrized constructor
         * @param db valid pointer to the DeviceDataBase object
         * @param devices a pointer to the vector with all cached devices info
         * @param atm a pointer to the ActiveThreatsManager. Used when creating new Device during ADD_DEVICE_QUERY
         */
        UserInterface(DeviceDataBase * db, vector<Device> *devices, ActiveThreatsManager * atm);
        /*!
         * @brief Dispatches TCP server, upload monitor and receive/transmit worker threads
         * @param ip_address TCP server ip address
         * @param port TCP server listening port number
         * @param share_path frontend share folder path. This path is used to manage uploaded files by the UploadMonitor
         * @return 1 if successfully started connection with frontend application. Otherwise 0
         */
        int StartFrontConnection(const char * ip_address, long port, const char * share_path);
        /*!
         * @brief Incoming data worker thread main function
         * @return Exit code
         */
        int FrontCommunicationInterfaceInData(void);
        /*!
         * @brief Outcoming data worker thread main function
         * @return Exit code
         */
        int FrontCommunicationInterfaceOutData();
        /*!
         * @brief Gets thread safe FrontCommunication::Sender object. This object can be used to send message to the frontend client
         * @return pointer to the FrontCommunication::Sender object
         */
        FrontCommunication::Sender * getSender() { return &this->frontCommunication.sender;}
        /*!
         * @brief Callback function for incoming OPERATOR_INTERFACE_QUERY message
         * @param user_id, an identifier of the user who sent the message
         */
        void onOperatorInterfaceQuery(uint8_t user_id);
        /*!
         * @brief Callback function for incoming ADMIN_INTERFACE_QUERY message
         * @param user_id, an identifier of the user who sent the message
         */
        void onAdminInterfaceQuery(uint8_t user_id);
        /*!
         * @brief Callback function for incoming DEVICES_MANAGEMENT_QUERY message
         * @param user_id, an identifier of the user who sent the message
         */
        void onDevicesManagementQuery(uint8_t user_id);
        /*!
         * @brief Callback function for incoming DEVICE_STATUS_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param device_id, device identifier for which status is requested
         */
        void onDeviceStatusQuery(uint8_t user_id, uint16_t device_id);
        /*!
         * @brief Callback function for incoming ADD_DEVICE_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param license_buffer, buffer containing encrypted license for device that should be added to the system
         * @param license_size number of bytes inside license_buffer (license size in bytes)
         */
        void onAddDeviceQuery(uint8_t user_id, const char *license_buffer, uint32_t license_size);
        /*!
         * @brief Callback function for incoming DELETE_DEVICE_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param device_id, identifier of the device which should be deleted
         */
        void onDeleteDeviceQuery(uint8_t user_id, uint16_t device_id);
        /*!
         * @brief Callback function for incoming FFT_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param device_id, identifier of the device for which FFT stream is requested
         * @param channel, a radio card number for which FFT stream should be send
         * @param
         */
        void onFFTQuery(uint8_t user_id, uint16_t device_id, uint8_t channel, bool repeat);
        /*!
         * @brief Callback function for incoming STOP_DEVICE_STATUS_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param device_id, device identifier for which status stream should be stopped
         */
        void onStopDeviceStatusQuery(uint8_t user_id, uint16_t device_id);
        /*!
         * @brief Callback function for incoming UPDATE_DEVICES_CONFIG_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param config_filename, a string containing requested configuration file name
         * @param device_ID, a vector containing device identifiers for which speciefied configuration should be updated
         */
        void onUpdateDevicesConfigQuery(uint8_t user_id, const std::string & config_filename, const std::vector<uint16_t> & device_ID);
        /*!
         * @brief Callback function for incoming UPDATE_DEVICES_CONFIG_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param config_filename, a string containing requested configuration file name
         * @param device_ID, a vector containing device identifiers for which speciefied firmware should be updated
         */
        void onUpdateDevicesFirmwareQuery(uint8_t user_id, const std::string & config_filename, const std::vector<uint16_t> & device_ID);
        /*!
         * @brief Callback function for incoming RESET_DEVICE_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param device_id, identifier of the device which should be restarted
         */
        void onResetDeviceQuery(uint8_t user_id, uint16_t device_id);
        /*!
         * @brief Callback function for incoming GET_LOG_QUERY message
         * @param user_id, an identifier of the user who sent the message
         * @param device_id, identifier of the device from which log file should be downloaded
         */
        void onGetLogQuery(uint8_t user_id, uint16_t device_id);
};

#endif // USER_INTERFACE_H
