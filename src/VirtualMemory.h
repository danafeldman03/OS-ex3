#pragma once
#include "PhysicalMemory.h"
#include "MemoryConstants.h"

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
void dfs(word_t frame, word_t parentFrame, uint64_t parentIndex, int level, uint64_t currentPage, uint64_t targetPage, word_t noEvictFrame, frameSearchInfo& info);
word_t pageFaultHandler(uint64_t virtualPage, word_t parentFrame, uint64_t parentIndex, int level, word_t noEvictFrame);
int translateVirtualAddress(uint64_t virtualAddress, uint64_t* physicalAddress);



/*
 * Initialize the virtual memory
 */
void VMinitialize();

/* reads a word from the given virtual address
 * and puts its content in *value.
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */
int VMread(uint64_t virtualAddress, word_t* value);

/* writes a word to the given virtual address
 *
 * returns 1 on success.
 * returns 0 on failure (if the address cannot be mapped to a physical
 * address for any reason)
 */

int VMwrite(uint64_t virtualAddress, word_t value);

/* Returns the physical frame index that virtualPage currently maps to,
 * or 0 if the page is not resident in RAM (never written or was evicted).
 * Does not allocate or restore anything — purely a read-only table walk.
 */
uint64_t VMgetMapping(uint64_t virtualPage);
