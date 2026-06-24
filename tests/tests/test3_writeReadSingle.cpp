/*
 * test3_writeReadSingle
 * Write a single distinctive value to an arbitrary address and read it
 * back.  Verifies the basic write → translate → read path.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test3_writeReadSingle.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    uint64_t addr = 100;
    word_t written = 0xCAFEBABE;

    VMwrite(addr, written);

    word_t readBack = 0;
    VMread(addr, &readBack);
    std::cout << "Wrote 0x" << std::hex << written
              << " to address " << std::dec << addr
              << ", read back 0x" << std::hex << readBack << std::endl;

    if (readBack == written) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL" << std::endl;
    }

    return 0;
}
