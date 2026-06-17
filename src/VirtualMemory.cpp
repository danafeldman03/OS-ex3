#include "VirtualMemory.h"
#include "MemoryConstants.h"

//------------------------------Helper Functions------------------------------------

/**
* Translates a virtual address to a physical address.
* @param virtualAddress The virtual address to translate.
* @param physicalAddress A pointer to store the resulting physical address.
* @return 1 on success, 0 on failure (if the address cannot be
* mapped to a physical address for any reason).
*/
int translateVirtualAddress(uint64_t virtualAddress, word_t* physicalAddress) {
    word_t addr = 0;
    uint64_t rootIndex = virtualAddress >> (TABLES_DEPTH * OFFSET_WIDTH);
    if(!PMread(rootIndex, &addr))
    {
        return 0;
    }

    for(int level = 1; level < TABLES_DEPTH; ++level)
    {
        int shift = (TABLES_DEPTH - level) * OFFSET_WIDTH;
        uint64_t index =(virtualAddress >> shift)&((1ULL << OFFSET_WIDTH) - 1);
        if(!PMread(addr * PAGE_SIZE + index, &addr))
        {
            return 0;
        }
    }
    uint64_t offset = virtualAddress & ((1ULL << OFFSET_WIDTH) - 1);
    *physicalAddress = addr * PAGE_SIZE + offset;
    return 1;
}





//--------------------------------API---------------------------------------------
void VMinitialize() {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i)
    {
        PMwrite(i, 0);
    }
}

int VMread(uint64_t virtualAddress, word_t* value) {
    word_t physicalAddress;
    if(translateVirtualAddress(virtualAddress, &physicalAddress))
    {
        PMread(physicalAddress, value);
        return 1;
    }
    return 0;

}


int VMwrite(uint64_t virtualAddress, word_t value) {
    word_t physicalAddress;
    if(translateVirtualAddress(virtualAddress, &physicalAddress))
    {
        PMwrite(physicalAddress, value);
        return 1;
    }
    return 0;
}

uint64_t VMgetMapping(uint64_t virtualPage) {} 
