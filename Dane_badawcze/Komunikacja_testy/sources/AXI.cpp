#include "AXI.h"
// /dev/plmem8 < open this file
bool AXI::init()
{
    void *base;

    /* open memory file */
    this->fd_scratchpad = open("/dev/plmem8",  O_RDWR | O_NDELAY);
    if (this->fd_scratchpad < 0)
    {
        printf("file /dev/mem open failed\n");
        return false;
    }
    /* map size of memory */
    //base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,this->fd_scratchpad, (SCRATCHPAD_REG & ~MAP_MASK));
    base = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,this->fd_scratchpad, 0);
    if (base == MAP_FAILED)
    {
        perror("map");
        printf("Error mmapping the file\n");
        close(this->fd_scratchpad);
        return false;
    }
    //this->map_scratchreg = (uint8_t*)base + (SCRATCHPAD_REG & MAP_MASK);
    this->map_scratchreg = (uint8_t*)base;
    return true;
}

bool AXI::deinit()
{
    if(!isInitialized()) return true;
    if (munmap(((uint8_t*)this->map_scratchreg - (SCRATCHPAD_REG & MAP_MASK)), MAP_SIZE) == -1)
    {
            printf("unmap failed for map_scratchreg\n");
            return false;
    }
    this->map_scratchreg = nullptr;
    if (this->fd_scratchpad)
    {
        close(this->fd_scratchpad);
        this->fd_scratchpad = 0;
    }
    return true;
}
