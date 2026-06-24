/*
 * test6_firstAndLastAddr
 * Write to the very first (0) and the very last (VIRTUAL_MEMORY_SIZE-1)
 * valid virtual addresses and read them both back.
 * Tests that address translation works at both extremes of the address
 * space (page 0 and the last page, with all-15 table indices).
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test6_firstAndLastAddr.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    uint64_t first = 0;
    uint64_t last  = VIRTUAL_MEMORY_SIZE - 1;

    VMwrite(first, 0xABCDEF01);
    VMwrite(last,  0xFEDCBA98);

    word_t v1 = 0, v2 = 0;
    VMread(first, &v1);
    VMread(last,  &v2);

    std::cout << "First addr (0): 0x"   << std::hex << v1 << std::endl;
    std::cout << "Last  addr (0x" << std::hex << last << "): 0x" << v2 << std::endl;

    bool ok = (v1 == 0xABCDEF01) && (v2 == 0xFEDCBA98);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
