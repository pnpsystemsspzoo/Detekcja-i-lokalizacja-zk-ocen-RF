#ifndef DEVICEDATABASE_H
#define DEVICEDATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <vector>
#include <string> 

#include "EventLogger.h"
#include "parser.h"
#include "Encryptor.h"
#include <ctime>

class DeviceDataBase
{
    private:    
    //const char *conninfo = "user=postgres password=qwert dbname=devices_data sslmode=disable";
    const char *conninfo = "dbname=devices_data sslmode=disable";
    PGconn *conn;
    PGresult *res;
	int nFields;
	int i;
    int j;

    /*!
     * @brief Converts time_t time into string representation
     * @param time a time_t representation of the date and time
     * @return std::string containing human-friendly string date-time representation of the input time
     */
    std::string timeToString(const time_t & time);
    /*!
     * @brief Prepares postgres insert new device query
     * @param input_data Parser::config_set structure that should be passed to the database INSERT query
     * @return std::string containing generated postgres INSERT query
     */
    std::string getDataBaseInsertQuery(const Parser::config_set & input_data);
    
    public:
    int numberOfElementsInDB;

    DeviceDataBase();
    ~DeviceDataBase();

    unsigned char ConnectToDataBase(void);
    /*!
     * @brief Appends new device into database based on the encoded license
     * @param license a pointer to the buffer where encoded license is stored.
     * @param bytes length of the license buffer in bytes
     * @return unique identifier of the probe registered inside the database. If failed to add new probe, the return value will be negative error code.
     */
    int16_t AddNewDevice(const char *license, uint32_t bytes);
    /*!
     * @brief Appends new device into database based on the encoded license
     * @param path_to_license a path to the encoded probe license file.
     * @return unique identifier of the probe registered inside the database. If failed to add new probe, the return value will be negative error code.
     */
    int16_t AddNewDevice(const char * path_to_license);
    unsigned char UpdateSelectedDevice(int DeviceID, Parser::config_set *deviceData);
    unsigned char DeleteSelectedDevice(int DeviceID);
    bool CheckSelectedDeviceExists(int DeviceID);
    unsigned char GetDataSelectedDevice(int DeviceID, Parser::config_set *deviceData);
    unsigned char GetDevicesNum(void);
    /*!
     * @brief Decodes probe license from the given buffer
     * @param license a pointer to the buffer where encoded license is stored.
     * @param bytes length of the license buffer in bytes
     * @param pathToKey a path to the file containing encypting key
     * @param devPar a pointer to the structure where decoded license parameters should be stored
     * @return True if success. Otherwise false
     */
    bool LoadLicense(const char *license, uint32_t bytes, const char * pathToKey, Parser::config_set *devPar);
};

#endif
