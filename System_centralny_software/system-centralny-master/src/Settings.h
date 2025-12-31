#ifndef SETTINGS_H
#define SETTINGS_H

#include "json.h"

/*!
 * @brief Used to load configuration settings from settings.json file
 */
class Settings
{
    /*!
     * @brief Split dot separated multiple keys from the string
     * @param str sequence of the dot separated keys
     * @return vector of separated keys
     */
    static std::vector<std::string> splitKeys(const char * str);
    /*!
     * @brief Gets string representation for nlohmann::detail::value_t. Helper function.
     * @param type nlohmann::detail::value_t for which string representation should be return
     * @return string representation of the type
     */
    static const char* getTypeName(const nlohmann::detail::value_t type);
    /*!
     * @brief Gets sub-json from the given json. Note, this function does not if key exists.
     * @param key a key for which sub-json should be returned
     * @param j a reference to the json structure where the given key exists
     * @return reference to the sub-json
     */
    static nlohmann::json & getSubJson(const char * key, const nlohmann::json &j);
    /*!
     * @brief Checks that the given key exists inside json.
     * @param key a key which shall exist in json
     * @param j a reference to the json structure where asserted key need to be stored
     * @param print_error prints error message to the stdout if set to true. Dafault false.
     * @return True if key exists inside json. False otherwise
     */
    static bool checkKeyExists(const char * key, const nlohmann::json & j,bool print_error=false);
    /*!
     * @brief Checks if the given key contains value with the specific type.
     * @param key a key to check
     * @param j a reference to the json structure where the geiven key is stored
     * @param type nlohmann::detail::value_t enumerated type value
     * @param print_error prints error message to the stdout if set to true. Dafault false.
     * @return True if key exists and value is of the specified type. Otherwise false
     */
    static bool checkValueType(const char * key, const nlohmann::json & j, nlohmann::detail::value_t type,bool print_error=false);

    static std::string settings_path; ///< Stores path to the settings json file
    static std::string error_message; ///< Stores load settings error message

        struct ISettingsNode
        {
            private:
                std::string error_message; ///< Error message container. Holds load settings error message

            public:
                /*!
                 * @brief loads settings from json
                 * @param json_content json to parse
                 * @return True if successfully parsed settings. False otherwise
                 */
                virtual bool load(const nlohmann::json & json_content) = 0;
                /*!
                 * @brief Gets load error message.
                 * @return String containing last saved error message
                 */
                const char* getErrorMessage() const;
                /*!
                 * @brief Sets settings load error message.
                 * @param msg string containing error message
                 */
                void setErrorMessage(const char * msg);
                /*!
                 * @brief Sets settings load error message to wrong key type message for a given key
                 * @param key a key which has wrong type
                 * @param expected_type value expected type
                 * @param actual_type value actual type
                 */
                void setErrorMessageWrongType(const char * key, nlohmann::detail::value_t expected_type, nlohmann::detail::value_t actual_type);
                /*!
                 * @brief Sets settings load error message to key not exists message for a given key
                 * @param key a key for which error message should be set
                 */
                void setErrorMessageKeyNotExists(const char * key);
                /*!
                 * @brief Asserts that the given key exists inside  json structure and has the proper type.
                 * If not asserted, error message is set and can be read from getErrorMessage()
                 * @param json_content json to parse
                 * @param key a key to assert. Can be from sub-json but need to be delimited by dot e.g. "EventLogger.log_path"
                 * @param expected_type expected value type for a given key
                 * @return True if asserted. Otherwise False
                 */
                bool assertKey(const nlohmann::json & json_content, const char * key, nlohmann::detail::value_t expected_type);

        }; ///< settings load interface for the given node

public:

        struct ui_settings : public ISettingsNode
        {
            /*!
             * @brief loads UserInterface settings from json
             * @param json_content
             */
            bool load(const nlohmann::json & json_content);

            std::string TCP_server_ip; ///< Frontend TCP server ip
            long TCP_server_port; ///< Frontend TCP server listening port
            std::string share_path; ///< Path to the frontend public/share folder
        };
        static ui_settings UserInterface; ///< Saves all UserInterface class configuration settings

        struct ev_settings : public ISettingsNode
        {
            /*!
             * @brief loads UserInterface settings from json
             * @param json_content
             */
            bool load(const nlohmann::json & json_content);

            std::string log_path; ///< EventLogger logging file path
        };
        static ev_settings EventLogger; ///< Saves all EventLogger class configuration settings

        struct encryptor_settings : public ISettingsNode
        {
            /*!
             * @brief loads UserInterface settings from json
             * @param json_content
             */
            bool load(const nlohmann::json & json_content);

            std::string key_path; ///< EventLogger logging file path
        };
        static encryptor_settings Encryptor; ///< Saves all Encryptor class configuration settings

        /*!
         * @brief loads server configuration settings from the json file into the RAM memory. Note, this function will terminate program execution if settings fail to load.
         * @param settings_json_path path to the settings json file
         */
        static bool load(const char * settings_json_path);
        /*!
         * @brief Gets error message that can be used if load() fails
         * @return string containing error message
         */
        static const char * getErrorMessage() {return Settings::error_message.c_str();}
};

#endif // SETTINGS_H
