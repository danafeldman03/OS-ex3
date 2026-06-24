/*
 * test5_overwrite
 * Write a value to an address, then overwrite it with a different value.
 * The read-back must return the second (latest) value, not the first.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test5_overwrite.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    uint64_t addr = 50;

    VMwrite(addr, 0xAAAAAAAA);
    VMwrite(addr, 0xBBBBBBBB);

    word_t val = 0;
    VMread(addr, &val);
    std::cout << "After overwrite at addr " << addr
              << ": 0x" << std::hex << val << std::endl;

    std::cout << (val == 0xBBBBBBBB ? "PASS" : "FAIL") << std::endl;
    return 0;
}
