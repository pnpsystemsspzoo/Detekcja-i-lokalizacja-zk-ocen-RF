#include "Settings.h"
#include<iostream>
#include<vector>
#include<sstream>
#include <fstream>
// t
using namespace std;

string Settings::settings_path;
string Settings::error_message;
Settings::ui_settings Settings::UserInterface;
Settings::ev_settings Settings::EventLogger;
Settings::encryptor_settings Settings::Encryptor;

std::vector<string> Settings::splitKeys(const char *str)
{
    vector<string> result;
    stringstream s_stream(str);
    while(s_stream.good()) {
        string substr;
        getline(s_stream, substr, '.');
        result.push_back(substr);
    }
    return result;
}

const char *Settings::getTypeName(const nlohmann::detail::value_t type)
{
    switch (type)
    {
    case nlohmann::detail::value_t::null:
        return "null";
    case nlohmann::detail::value_t::object:
        return "object";
    case nlohmann::detail::value_t::array:
        return "array";
    case nlohmann::detail::value_t::string:
        return "string";
    case nlohmann::detail::value_t::boolean:
        return "boolean";
    case nlohmann::detail::value_t::binary:
        return "binary";
    case nlohmann::detail::value_t::discarded:
        return "discarded";
    default:
        return "number";
    }
}

nlohmann::json &Settings::getSubJson(const char *key, const nlohmann::json &j)
{
    auto keys = splitKeys(key);
    int n = keys.size();
    nlohmann::json * subtree = const_cast<nlohmann::json*>(&j);
    for(int i=0; i<n;++i)
    {
        subtree = &((*subtree)[keys[i]]);
    }
    return *subtree;
}

bool Settings::checkKeyExists(const char *key, const nlohmann::json &j, bool print_error)
{
    auto keys = splitKeys(key);
    int n = keys.size();
    nlohmann::json * subtree = const_cast<nlohmann::json *>(&j);
    for(int i=0; i<n;++i)
    {
        auto it_key = subtree->find(keys[i].c_str());
        if(it_key == subtree->end())
        {
            if(print_error)
            {
                std::string key_path = keys[0];
                for(int idx=1;idx<=i;idx++)
                {
                    key_path += ".";
                    key_path += keys[idx];
                }
                printf("Settings: could not find key %s inside settings file %s",key_path.c_str(),Settings::settings_path.c_str());
            }
            return false;
        }
        subtree = &((*subtree)[keys[i]]);
    }
    return true;
}


bool Settings::checkValueType(const char *key, const nlohmann::json &j, nlohmann::detail::value_t type, bool print_error)
{
    if(!checkKeyExists(key,j,print_error)) return false;
    auto val = Settings::getSubJson(key,j);
    if(val.type() != type)
    {
        if(print_error)
        {
            printf("Settings: value has wrong type for key %s inside file %s. Expected %s but got %s instead",key, Settings::settings_path.c_str(), Settings::getTypeName(type),val.type_name());
        }
        return false;
    }
    return true;
}

bool Settings::load(const char *settings_json_path)
{
    // set settings path
    Settings::settings_path = settings_json_path;

    // open settings file
    nlohmann::json settings;
    std::ifstream i(settings_json_path);
    if(i.fail()){
        error_message = "Failed to read settings json file at the given path '";
        error_message += settings_json_path;
        error_message += "'";
        return false;
    }

    // try to parse json
    try
    {
        i >> settings;
    }
    catch (nlohmann::detail::exception & e)
    {
        error_message = "Json syntax error. Failed to parse settings json file at the given path '";
        error_message += settings_json_path;
        error_message += "'. Parser error: ";
        error_message += e.what();
        return false;
    }

    // json syntax is ok
    // try read all settings from json to Settings class
    std::string err_prefix;
    err_prefix = "Failed to load settings from json file '";
    err_prefix += settings_json_path;
    err_prefix += "' for ";

    if(!Settings::UserInterface.load(settings))
    {
        // failed to load some settings (missing keys or wrong types)
        error_message = err_prefix + "UserInterface. " + Settings::UserInterface.getErrorMessage();
        return false;
    }

    if(!Settings::EventLogger.load(settings))
    {
        // failed to load some settings (missing keys or wrong types)
        error_message = err_prefix + "EventLogger. " + Settings::EventLogger.getErrorMessage();
        return false;
    }

    if(!Settings::Encryptor.load(settings))
    {
        // failed to load some settings (missing keys or wrong types)
        error_message = err_prefix + "Encryptor. " + Settings::Encryptor.getErrorMessage();
        return false;
    }

    // settings loaded
    return true;
}

bool Settings::ui_settings::load(const nlohmann::json & json_content)
{
    if(!assertKey(json_content,"UserInterface.TCP_server_ip",nlohmann::detail::value_t::string)) return false;
    if(!assertKey(json_content,"UserInterface.TCP_server_port",nlohmann::detail::value_t::number_unsigned)) return false;
    if(!assertKey(json_content,"UserInterface.share_path",nlohmann::detail::value_t::string)) return false;


    TCP_server_ip = getSubJson("UserInterface.TCP_server_ip",json_content);
    TCP_server_port = getSubJson("UserInterface.TCP_server_port",json_content);
    share_path = getSubJson("UserInterface.share_path",json_content);

    return true;
}

const char * Settings::ISettingsNode::getErrorMessage() const
{
    return error_message.c_str();
}

void Settings::ISettingsNode::setErrorMessage(const char * msg)
{
    error_message = msg;
}

void Settings::ISettingsNode::setErrorMessageWrongType(const char *key,nlohmann::detail::value_t expected_type, nlohmann::detail::value_t actual_type)
{
    error_message = "Value has wrong type for key '";
    error_message += key;
    error_message += "'. Expected '";
    error_message += Settings::getTypeName(expected_type);
    error_message += "' but got '";
    error_message += Settings::getTypeName(actual_type);
    error_message += "' instead";
}

void Settings::ISettingsNode::setErrorMessageKeyNotExists(const char *key)
{
    error_message = "Could not find key '";
    error_message += key;
    error_message += "'";
}

bool Settings::ISettingsNode::assertKey(const nlohmann::json &json_content, const char *key, nlohmann::detail::value_t expected_type)
{
    if(!checkKeyExists(key,json_content))
    {
        // key not exist
        setErrorMessageKeyNotExists(key);
        return false;
    }
    if(!checkValueType(key,json_content,expected_type))
    {
        // wrong key type
        auto json = getSubJson(key,json_content);
        setErrorMessageWrongType(key,expected_type,json.type());
        return false;
    }

    // asserted
    return true;
}

bool Settings::ev_settings::load(const nlohmann::json &json_content)
{
    if(!assertKey(json_content,"EventLogger.log_path",nlohmann::detail::value_t::string)) return false;
    log_path = getSubJson("EventLogger.log_path",json_content);

    return true;
}

bool Settings::encryptor_settings::load(const nlohmann::json &json_content)
{
    if(!assertKey(json_content,"Encryptor.key_path",nlohmann::detail::value_t::string)) return false;
    key_path = getSubJson("Encryptor.key_path",json_content);

    return true;
}
