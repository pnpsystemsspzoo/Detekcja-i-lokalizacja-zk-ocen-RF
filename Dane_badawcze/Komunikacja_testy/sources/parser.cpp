#include "parser.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <ctime>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool Parser::readString(INIReader &reader, const char *key, std::string *output)
{
    std::stringstream sentence(reader.Get("PROBE",key,"UNKNOWN"));
    std::getline(sentence, *output, ';');
    if ((*output).compare("UNKNOWN") == 0)
    {
        fprintf (stderr, "Cannot parse argument '%s'. Is this parameter missing?\n", key);
        return false;
    }
    return true;
}

bool Parser::readTime(INIReader &reader, const char *key, time_t *output)
{
    struct std::tm tm;
    std::string str;
    if(!readString(reader,key,&str)) return false;
    strptime(str.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = -1;
    *output = mktime(&tm);
    return true;
}


bool Parser::readInt(INIReader &reader, const char *key, uint32_t *output)
{
    *output = reader.GetInteger("PROBE",key, 0);
    if (*output == 0)
    {
        fprintf (stderr, "Cannot parse argument '%s'. Is this parameter missing?\n", key);
        return false;
    }
    return true;
}

bool Parser::readPort(INIReader &reader, const char *key, uint16_t *output)
{
    uint32_t temp;
    if(!readInt(reader,key,&temp)) return false;
    if(temp > 0xFFFF)
    {
        fprintf (stderr, "Port '%s' has invalid value: '%d'. The maximum allowed port number is 65535\n", key,temp);
        return false;
    }
    *output = static_cast<uint16_t>(temp);
    return true;
}

Parser::Parser()
{

}

bool Parser::read(INIReader &reader, Parser::config_set *decoded)
{
    if(!readInt(reader,"id",&decoded->id)) return false;
    if(!readString(reader,"code",&decoded->code)) return false;
    if(decoded->code.size() != 16)
    {
        fprintf (stderr, "Hardware code must contain exactly 16 ASCII characters. Value '%s' contains %d characters\n", decoded->code.c_str(), (int)decoded->code.size());
        return false;
    }
    if(!readString(reader,"MAC",&decoded->mac_addr)) return false;
    if(!readString(reader,"who",&decoded->who)) return false;
    if(!readTime(reader,"date",&decoded->creation_date)) return false;
    if(!readTime(reader,"expdate",&decoded->expiration_date)) return false;
    if(!readPort(reader,"ctrl_port",&decoded->control_port)) return false;
    if(!readPort(reader,"data_port",&decoded->data_port)) return false;
    uint32_t cards;
    if(!readInt(reader,"cards",&cards)) return false;
    if(cards < 1 || cards > 4)
    {
        fprintf (stderr, "'cards' has invalid value: '%d'. The value must be within range [1:4]\n", cards);
        return false;
    }
    decoded->cards = cards;
    char card_name[8];
    for(uint8_t i=0;i<decoded->cards;++i)
    {
        sprintf(card_name,"card%d",(i+1));
        if(!readString(reader,card_name,&decoded->card_name[i])) return false;
    }
    return true;
}

bool Parser::readConfig(const char *file_path, Parser::config_set *decoded)
{
    INIReader reader(file_path);

    if (reader.ParseError() < 0) {
        fprintf (stderr, "Can't load file: '%s'\n", file_path);
        return false;
    }
    if(!Parser::read(reader,decoded)) return false;

    std::cout << "Parsed license parameters:\n";
    decoded->print();

    return true;
}

bool Parser::readConfig(std::string &file_content, Parser::config_set *decoded)
{
    INIReader reader(file_content.c_str(), file_content.size());
    if (reader.ParseError() < 0) {
        fprintf (stderr, "Can't load buffer\n");
        return false;
    }
    if(!Parser::read(reader,decoded)) return false;

    return true;
}

bool Parser::getArgs(int argc, char **argv, Parser::args_set *output)
{
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "k:d:e:o:c:")) != -1)
        switch (c)
        {
        case 'c':
            output->cvalue = optarg;
            break;
        case 'k':
            output->kvalue = optarg;
            break;
        case 'd':
            output->dvalue = optarg;
            break;
        case 'e':
            output->evalue = optarg;
            break;
        case 'o':
            output->ovalue = optarg;
            break;
        case '?':
            if (optopt == 'k' || optopt == 'd'  || optopt == 'e')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            output->mode = ERROR;
            return false;
        default:
            output->mode = ERROR;
            return false;
        }

    if(!output->kvalue)
    {
        fprintf (stderr, "Option -k requires a path to the key and is mandatory.\n");
        output->mode = ERROR;
        return false;
    }
    if(output->cvalue)
    {
       output->mode = CHECK;
       return true;
    }

    if(!output->ovalue)
    {
        fprintf (stderr, "Option -o requires a path to the output file and is mandatory.\n");
        output->mode = ERROR;
        return false;
    }

    if(!output->dvalue && !output->evalue)
    {
        fprintf (stderr, "Required either -e (encode) or -d (decode) option.\n");
        output->mode = ERROR;
        return false;
    }
    if(output->dvalue && output->evalue)
    {
        fprintf (stderr, "Required either -e (encode) or -d (decode) option, not both.\n");
        output->mode = ERROR;
        return false;
    }
    if(output->dvalue)
    {
        output->mode = DECRYPT;
        return true;
    }
    output->mode = ENCRYPT;
    return true;
}

void Parser::config_set::print()
{
    std::cout << "probe id = " << this->id << "\n";
    std::cout << "probe hardware code = " << this->code << "\n";
    std::cout << "MAC address = " << this->mac_addr << "\n";
    std::cout << "data port = " << this->data_port << "\n";
    std::cout << "control port = " << this->control_port << "\n";
    std::cout << "creation date = " << std::ctime(&this->creation_date);
    std::cout << "expiration date = " << std::ctime(&this->expiration_date);
    std::cout << "creator = " << this->who << "\n";
    std::cout << "number of cards = " << static_cast<uint32_t>(this->cards) << "\n";
    for(uint8_t i=0;i<this->cards;++i) std::cout << "card " << (i+1) << " name = " << this->card_name[i] << "\n";
    std::cout << "fristmess = " << this->firstmess << "\n";
    std::cout << "longitude = " << this->longitude << "\n";
    std::cout << "latitude = " << this->latitude << "\n";
    std::cout << "height = " << this->height << "\n";

}
