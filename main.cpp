#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

using namespace std;

const uint32_t BLOCK_SIZE = 10 * 1024 * 1024;

unsigned char local[BLOCK_SIZE];


// Forward declarations
uint8_t* mapPhysMem(uint64_t physAddr, size_t size);
void     execute();


//=================================================================================================
// main() - Execution starts here
//=================================================================================================
int main(int argc, const char** argv)
{
    try
    {
        execute();
    }
    catch(const std::exception& e)
    {
        printf("%s\n", e.what());
        exit(1);
    }
}
//=================================================================================================



//=================================================================================================
// execute() - Does everything neccessary to begin a data transfer
//=================================================================================================
void execute()
{
    // Map the reserved RAM block into userspace
    cout << "Mapping physical RAM\n";
    uint8_t* physMem = mapPhysMem(0x100000000, 0x80000000);

    for (int i=0; i<10; ++i)
    {
        printf("Writing\n");
        memset(physMem, 0x45, BLOCK_SIZE);
    }
}
//=================================================================================================


//=================================================================================================
// mapPhysMem() - Maps phyiscal memory addresses into user-space
//=================================================================================================
uint8_t* mapPhysMem(uint64_t physAddr, size_t size)
{
    const char* filename = "/dev/mem";

    // These are the memory protection flags we'll use when mapping the device into memory
    const int protection = PROT_READ | PROT_WRITE;

    // Open the /dev/mem device
    int fd = ::open(filename, O_RDWR| O_SYNC);

    // If that open failed, we're done here
    if (fd < 0) throw std::runtime_error("Must be root.  Use sudo.");

    // Map the the physical RAM address into userspace
    void* ptr = ::mmap(0, size, protection, MAP_SHARED, fd, physAddr);

    // If a mapping error occurs, it's fatal
    if (ptr == MAP_FAILED) 
    {
        perror("mmap failed");
        throw std::runtime_error("mmap failed");
    }

    // Hand the user-space pointer to the caller
    return (uint8_t*) ptr;
}
//=================================================================================================

