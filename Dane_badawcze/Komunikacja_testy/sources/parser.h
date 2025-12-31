#ifndef PARSER_H
#define PARSER_H

#include "INIReader.h"
#include <ctime>
#include <iostream>

#define MAX_NUMBER_OF_RADIO_CARDS 5

class Parser
{
private:

        static bool readString(INIReader & reader,const char * key, std::string * output);
        static bool readTime(INIReader & reader,const char * key, time_t * output);
        static bool readInt(INIReader & reader, const char * key, uint32_t * output);
        static bool readPort(INIReader & reader, const char * key, uint16_t * output);


    public:
        Parser();

        struct config_set
        {
            uint32_t id; // probe identifier in the system
            std::string mac_addr; // MAC address
            uint16_t data_port; // data streaming TCP port
            uint16_t control_port; // control commands TCP port
            time_t creation_date; // license creation date and time
            time_t expiration_date; // license expiration date and time
            std::string code; // probe hardware code
            std::string who; // license creator
            uint8_t cards; // number of radio cards
            std::string card_name[MAX_NUMBER_OF_RADIO_CARDS]; // radio card name
            uint32_t firstmess;
            double longitude;
            double latitude;
            uint32_t height;
            std::string ip_address;

            void print();
        };

        static bool read(INIReader & reader, config_set * decoded);

        enum MODE
        {
            ENCRYPT,
            DECRYPT,
            CHECK,
            ERROR
        };

        struct args_set
        {
            char *kvalue = NULL;
            char *evalue = NULL;
            char *dvalue = NULL;
            char *cvalue = NULL;
            char *ovalue = NULL;
            MODE mode;
        };

        static bool readConfig(const char * file_path, config_set * decoded);
        static bool readConfig(std::string & file_content, config_set * decoded);
        static bool getArgs(int argc, char **argv, args_set *output);
};

#endif // PARSER_H
