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
int translateVirtualAddress(uint64_t virtualAddress, uint64_t* physicalAddress) {
    uint64_t addr = 0;
    uint64_t rootIndex = virtualAddress >> (TABLES_DEPTH * OFFSET_WIDTH);
    if(!PMread(rootIndex, &addr))
    {
        return 0;
    }

    for(int level = 1; level < TABLES_DEPTH; ++level)
    {
        if(addr == 0)
        {
            return pageFaultHandler(virtualAddress);
        }
        int shift = (TABLES_DEPTH - level) * OFFSET_WIDTH;
        uint64_t index =(virtualAddress >> shift)&((1ULL << OFFSET_WIDTH) - 1);
        if(!PMread(addr * PAGE_SIZE + index, &addr))
        {
            return 0;
        }
    }
    if(addr == 0)
    {
        return pageFaultHandler(virtualAddress);
    }
    uint64_t offset = virtualAddress & ((1ULL << OFFSET_WIDTH) - 1);
    *physicalAddress = addr * PAGE_SIZE + offset;
    if(virtualAddress >= VIRTUAL_MEMORY_SIZE)
    {
        return 0;
    }
    return 1;
}

int pageFaultHandler(uint64_t virtualAddress) {
    //placeholder for page fault handling logic
    return 0; // Return 0 to indicate failure for now
}




//--------------------------------API---------------------------------------------
void VMinitialize() {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i)
    {
        PMwrite(i, 0);
    }
}

int VMread(uint64_t virtualAddress, word_t* value) {
    uint64_t physicalAddress;
    if(translateVirtualAddress(virtualAddress, &physicalAddress))
    {
        PMread(physicalAddress, value);
        return 1;
    }
    return 0;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t physicalAddress;
    if(translateVirtualAddress(virtualAddress, &physicalAddress))
    {
        PMwrite(physicalAddress, value);
        return 1;
    }
    return 0;
}

//TODO
uint64_t VMgetMapping(uint64_t virtualPage) {} 
