#include "Encryptor.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include "INIReader.h"
#include <iostream>
#include <fstream>

const unsigned char Encryptor::ivec[] = "q4t7w!z^C*F-J@NcRfUjXn2r5u8x/A?D";

Encryptor::Encryptor()
{

}

bool Encryptor::setKeyFromFile(const char *file_name)
{
    std::ifstream fkey(file_name);
    if(!fkey.is_open())
    {
        return false;
    }
    this->key = std::string((std::istreambuf_iterator<char>(fkey)),std::istreambuf_iterator<char>());
    return true;
}

bool Encryptor::decryptFile(const char *input_file, std::string *output)
{
    char *source;
    uint32_t bytes_read = 0;
    if(!this->loadFileToBuffer(input_file,&source,&bytes_read)) return false;
    char *destination = reinterpret_cast<char*>(malloc(bytes_read+1));

    this->processBuffer(source,destination,bytes_read,AES_DECRYPT);

    destination[bytes_read] = 0;
    *output = destination;
    free(source);
    free(destination);
    return true;
}

bool Encryptor::decryptFile(const char *input_file, Parser::config_set *output)
{
    std::string decrypted;
    if(!this->decryptFile(input_file,&decrypted)) return false;
    if(!Parser::readConfig(decrypted,output)) return false;
    return true;
}

bool Encryptor::decryptBuffer(const char *source, uint32_t bytes, Parser::config_set *output)
{
    std::string decrypted;
    char *destination = reinterpret_cast<char*>(malloc(bytes));
    this->processBuffer(source,destination,bytes,AES_DECRYPT);
    decrypted = destination;
    free(destination);
    return Parser::readConfig(decrypted,output);
}

void Encryptor::processBuffer(const char *source, char *destination, uint32_t bytes, const int operation_type)
{
    unsigned char indata[AES_BLOCK_SIZE];

    AES_KEY key;
    AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(this->key.c_str()), 256, &key);

    const char * end = source + bytes;

    int bytes_read;
    int num = 0;
    uint32_t j = 0;
    unsigned char iv[sizeof(Encryptor::ivec)];
    memcpy(iv,Encryptor::ivec,sizeof(Encryptor::ivec));
    while (1) {
        bytes_read = 0;
        for(uint32_t i=0;i<AES_BLOCK_SIZE;i++)
        {
            indata[i] = *source;
            source++;
            bytes_read++;
            if(source == end)
            {
                break;
            }
        }
        AES_cfb128_encrypt(indata, reinterpret_cast<unsigned char*>(destination), bytes_read, &key, iv, &num,operation_type);
        destination += bytes_read;
        if (bytes_read < AES_BLOCK_SIZE)
        {
            break;
        }
    }
}

bool Encryptor::processFile(const char *input_file, const char *output_file, const int operation_type)
{
    char *source;
    uint32_t bytes_read = 0;
    if(!this->loadFileToBuffer(input_file,&source,&bytes_read)) return false;
    char *destination = reinterpret_cast<char*>(malloc(bytes_read));

    this->processBuffer(source,destination,bytes_read,operation_type);

    bool res = this->saveBufferToFile(destination,bytes_read,output_file);

    free(source);
    free(destination);
    return res;
}

bool Encryptor::loadFileToBuffer(const char *file_path, char **buffer, uint32_t *bytes_written)
{
    FILE *ifp = fopen(file_path, "rb");
    if(ifp == NULL)
    {
        fprintf(stderr,"Cannot open input file '%s'\n",file_path);
        return false;
    }
    fseek(ifp, 0, SEEK_END);
    long fsize = ftell(ifp);
    fseek(ifp, 0, SEEK_SET);  /* same as rewind(ifp); */

    *buffer = reinterpret_cast<char*>(malloc(fsize));
    *bytes_written = fread(*buffer, 1, fsize, ifp);
    if (*bytes_written != fsize)
    {
        fprintf(stderr,"Cannot read file '%s'\n",file_path);
        fclose(ifp);
        free(buffer);
        return false;
    }
    return true;
}

bool Encryptor::saveBufferToFile(char *buffer, uint32_t n, const char *file_path)
{
    FILE *ofp = fopen(file_path, "wb");
    if(ofp == NULL)
    {
        fprintf(stderr,"Cannot open output file '%s'\n",file_path);
        return false;
    }
    uint32_t bytes_written = fwrite(buffer, 1, n, ofp);
    if (bytes_written != n)
    {
        fprintf(stderr,"Cannot write to file '%s'\n",file_path);
        return false;
    }
    fclose(ofp);
    return true;
}
