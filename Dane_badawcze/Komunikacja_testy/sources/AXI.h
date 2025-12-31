#ifndef AXI_H
#define AXI_H

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

//"/dev/plmem8"

#define SCRATCHPAD_REG 0xB0000000
#define MAP_SIZE 0xFFFF
#define MAP_MASK (MAP_SIZE - 1)

class AXI
{
    int fd_scratchpad;
    void* map_scratchreg;

public:

    bool init();
    volatile void* getBasePointer() const {return this->map_scratchreg;}
    bool deinit();
    bool isInitialized() const {return this->map_scratchreg != nullptr;}


};

#endif // AXI_H
