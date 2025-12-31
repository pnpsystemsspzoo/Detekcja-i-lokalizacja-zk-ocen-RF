#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <string>
#include <vector>
#include <stdint.h>
#include "openssl/aes.h"
#include "parser.h"

class Encryptor
{
    std::string key;
    static const unsigned char ivec[];

public:

    void processBuffer(const char *source, char * destination, uint32_t bytes, const int operation_type);
    bool processFile(const char * input_file, const char * output_file, const int operation_type);

    static bool loadFileToBuffer(const char * file_path, char **buffer, uint32_t * bytes_written);
    static bool saveBufferToFile(char * buffer, uint32_t n, const char * file_path);


    Encryptor();
    void setKey(const char * ascii_key) {key = ascii_key;}
    bool setKeyFromFile(const char * file_name);
    bool encryptFile(const char * input_file, const char * output_file) {return processFile(input_file,output_file,AES_ENCRYPT);}
    bool decryptFile(const char * input_file, const char * output_file) {return processFile(input_file,output_file,AES_DECRYPT);}
    bool decryptFile(const char * input_file, std::string * output);
    bool decryptFile(const char * input_file, Parser::config_set * output);
    bool decryptBuffer(const char *source, uint32_t bytes, Parser::config_set * output);
};

#endif // ENCRYPTOR_H
