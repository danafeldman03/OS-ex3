#include "VirtualMemory.h"
#include "MemoryConstants.h"
#include "PhysicalMemory.h"
#include <algorithm>

//------------------------------Declerations------------------------------------
struct frameSearchInfo{
    //case 1: frame is empty
    bool foundEmptyTable = false;
    word_t emptyFrame = 0;
    word_t emptyFrameParent = 0;
    uint64_t emptyFrameIndex = 0;

    //case 2: unused frame
    word_t maxFrameIndex = 0;

    //case 3: evictFrame
    word_t evictFrame = 0;
    word_t evictFrameParent = 0;
    uint64_t evictParentIndex = 0;
    uint64_t evictPage = 0;
    uint64_t distance = 0;
    bool foundEvictVictim = false;
};

uint64_t getIndex(uint64_t virtualAddress, int level);
bool isEmptyTable(word_t frame);
uint64_t calculateDistance(uint64_t a, uint64_t b);
frameSearchInfo findFrame(uint64_t virtualPage, word_t noEvictFrame);
void dfs(word_t frame, word_t parentFrame, uint64_t parentIndex, int level,
     uint64_t currentPage, uint64_t targetPage, word_t noEvictFrame, frameSearchInfo& info);
word_t pageFaultHandler(uint64_t virtualPage, word_t parentFrame, uint64_t parentIndex, 
    int level, word_t noEvictFrame);
int translateVirtualAddress(uint64_t virtualAddress, uint64_t* physicalAddress);





//------------------------------Helper Functions------------------------------------

/**
 * Gets the index for a given level in the virtual address.
 * @param virtualAddress The virtual address.
 * @param level The level in the page table.
 * @return The index for the given level.
 */
uint64_t getIndex(uint64_t virtualAddress, int level){
    uint64_t shiftAmount = (TABLES_DEPTH - level) * OFFSET_WIDTH;
    uint64_t indexBits;
    if(level == 0){
        indexBits = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH * TABLES_DEPTH;
    }
    else{
        indexBits = OFFSET_WIDTH;
    }
    return (virtualAddress >> shiftAmount) & ((1ULL << indexBits) - 1);
}


bool isEmptyTable(word_t frame){
        for (uint64_t i = 0; i < PAGE_SIZE; i++){
        word_t value;
        PMread(frame * PAGE_SIZE + i, &value);
        if (value != 0){
            return false;
        }
    }
    return true;
}

uint64_t calculateDistance(uint64_t a, uint64_t b) {
    // calculate abs value
    uint64_t diff = (a > b) ? (a - b) : (b - a);
    // return min
    return (diff < NUM_PAGES - diff) ? diff : (NUM_PAGES - diff);
}


/**
 * Finds a suitable frame for the virtual page by searching through the page 
 tables.
 * @param virtualPage The virtual page that needs a frame.
 * @param noEvictFrame A frame that should not be evicted (if applicable).
 * @return A structure containing information about the found frame.
 */
frameSearchInfo findFrame(uint64_t virtualPage, word_t noEvictFrame) {
    frameSearchInfo info;
    dfs(0, 0, 0, 0, 0, virtualPage, noEvictFrame, info);
    return info;
}


//TODO
void dfs(word_t frame, word_t parentFrame, uint64_t parentIndex, int level,
        uint64_t currentPage, uint64_t targetPage, word_t noEvictFrame, 
        frameSearchInfo &info) {
    // Update max 
    info.maxFrameIndex = std::max(info.maxFrameIndex, frame);
    // if we got to a leaf - deal with eviction and reurn
    if (level == TABLES_DEPTH){
        if(frame != noEvictFrame){
            uint64_t dist = calculateDistance(targetPage, currentPage);
            if (!info.foundEvictVictim || dist > info.distance || 
                (dist == info.distance && currentPage < info.evictPage)){
                info.foundEvictVictim = true;
                info.evictFrame = frame;
                info.evictFrameParent = parentFrame;
                info.evictParentIndex = parentIndex;
                info.evictPage = currentPage;
                info.distance = dist;
            }
        }
        return;
    }
    // not a leaf - internal level table
    bool isEmpty = true;
    for (uint64_t i = 0; i < PAGE_SIZE; i++)
    {
        word_t nextFrame;
        PMread(frame * PAGE_SIZE + i, &nextFrame);
        if (nextFrame != 0){
            isEmpty = false;
            dfs(nextFrame, frame, i, level + 1, 
                (currentPage << OFFSET_WIDTH)|i, targetPage, noEvictFrame, info);
        }
    }
    // if it is an empty table
    if (isEmpty && frame != 0 && frame != noEvictFrame){
        info.foundEmptyTable = true;
        info.emptyFrame = frame;
        info.emptyFrameParent = parentFrame;
        info.emptyFrameIndex = parentIndex;
    }
}





/**
 * Handles a page fault by finding a suitable frame for the virtual page.
 * @param virtualPage The virtual page that caused the page fault.
 * @param parentFrame The frame of the parent page table.
 * @param parentIndex The index in the parent page table.
 * @param level The current level in the page table hierarchy.
 * @param noEvictFrame A frame that should not be evicted (if applicable).
 * @return The frame index where the virtual page is now mapped.
 */
word_t pageFaultHandler(uint64_t virtualPage, word_t parentFrame, uint64_t parentIndex, 
int level, word_t noEvictFrame) {
    frameSearchInfo searchInfo = findFrame(virtualPage, noEvictFrame);
    word_t frame = 0;

    //case 1
    if(searchInfo.foundEmptyTable){
        PMwrite(searchInfo.emptyFrameParent * PAGE_SIZE + searchInfo.emptyFrameIndex, 0);
        frame = searchInfo.emptyFrame;
    }

    //case 2
    else if(searchInfo.maxFrameIndex < NUM_FRAMES - 1){
        frame = searchInfo.maxFrameIndex + 1;
    }

    //case 3
    else{
        PMevict(searchInfo.evictFrame, searchInfo.evictPage);
        PMwrite(searchInfo.evictFrameParent * PAGE_SIZE + searchInfo.evictParentIndex, 0);
        frame = searchInfo.evictFrame;
    }

    if(level == TABLES_DEPTH - 1){
        PMrestore(frame, virtualPage);
    }
    else{
        for(uint64_t i = 0; i < PAGE_SIZE; i++){
            PMwrite(frame * PAGE_SIZE + i, 0);
        }
    }
    PMwrite(parentFrame * PAGE_SIZE + parentIndex, frame);
    return frame;
}


/**
* Translates a virtual address to a physical address.
* @param virtualAddress The virtual address to translate.
* @param physicalAddress A pointer to store the resulting physical address.
* @return 1 on success, 0 on failure (if the address cannot be
* mapped to a physical address for any reason).
*/
int translateVirtualAddress(uint64_t virtualAddress, uint64_t* physicalAddress) {
    if(virtualAddress >= VIRTUAL_MEMORY_SIZE){
        return 0;
    }
    uint64_t virtualPage = virtualAddress >> OFFSET_WIDTH;
    uint64_t offset = virtualAddress & ((1ULL << OFFSET_WIDTH) - 1);
    word_t currFrame = 0; // root frame
    for(int level = 0; level < TABLES_DEPTH; level++){
        uint64_t currentIndex = getIndex(virtualAddress, level);
        word_t nextFrame = 0;
        PMread(currFrame * PAGE_SIZE + currentIndex, &nextFrame);
        if(nextFrame == 0){
            nextFrame = pageFaultHandler(virtualPage, currFrame, currentIndex,
                 level, currFrame);
        }
        currFrame = nextFrame;
    }
    *physicalAddress = currFrame * PAGE_SIZE + offset;
    return 1;
}








//--------------------------------API---------------------------------------------
void VMinitialize() {
    for (uint64_t i = 0; i < PAGE_SIZE; i++)
    {
        PMwrite(i, 0);
    }
}

int VMread(uint64_t virtualAddress, word_t* value) {
    uint64_t physicalAddress;
    if(!translateVirtualAddress(virtualAddress, &physicalAddress))
        return 0;
    PMread(physicalAddress, value);
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t physicalAddress;
    if(!translateVirtualAddress(virtualAddress, &physicalAddress))
        return 0;
    PMwrite(physicalAddress, value);
    return 1;
}

//TODO
/* Returns the physical frame index that virtualPage currently maps to,
 * or 0 if the page is not resident in RAM (never written or was evicted).
 * Does not allocate or restore anything — purely a read-only table walk.
 */
uint64_t VMgetMapping(uint64_t virtualPage) {
    word_t frame = 0;
    for (int level = 0; level < TABLES_DEPTH; level++){
        uint64_t index = getIndex(virtualPage << OFFSET_WIDTH, level);
        word_t nextFrame;
        PMread(frame * PAGE_SIZE + index, &nextFrame);  
        if (nextFrame == 0){
            return 0;
        }
        frame = nextFrame;
    }
    return frame;
} 
