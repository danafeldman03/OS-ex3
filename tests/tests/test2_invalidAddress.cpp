/*
 * test2_invalidAddress
 * VMwrite and VMread must return 0 when the virtual address is out of
 * the valid range [0, VIRTUAL_MEMORY_SIZE).
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test2_invalidAddress.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // Exactly at the boundary — invalid
    int writeResult = VMwrite(VIRTUAL_MEMORY_SIZE, 42);
    std::cout << "VMwrite at VIRTUAL_MEMORY_SIZE: " << writeResult << std::endl;

    word_t value = 0xDEAD;
    int readResult = VMread(VIRTUAL_MEMORY_SIZE, &value);
    std::cout << "VMread at VIRTUAL_MEMORY_SIZE: " << readResult << std::endl;

    // One past the end — also invalid
    int writeResult2 = VMwrite(VIRTUAL_MEMORY_SIZE + 100, 99);
    std::cout << "VMwrite at VIRTUAL_MEMORY_SIZE+100: " << writeResult2 << std::endl;

    int readResult2 = VMread(VIRTUAL_MEMORY_SIZE + 100, &value);
    std::cout << "VMread at VIRTUAL_MEMORY_SIZE+100: " << readResult2 << std::endl;

    return 0;
}
