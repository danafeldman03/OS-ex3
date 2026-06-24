/*
 * advanced_test3_VMgetMappingAfterEviction
 *
 * Verifies that VMgetMapping returns 0 for a page that was evicted.
 *
 * With small memory (TABLES_DEPTH=1, NUM_FRAMES=4):
 *   - Write pages 0, 1, 2  →  fills all 3 data frames.
 *   - Write page 3  →  evicts page 0 (max cyclical distance from 3).
 *     Distances: page0=min(3,5)=3, page1=min(2,6)=2, page2=min(1,7)=1
 *   - VMgetMapping(0) must now return 0 (page 0 is no longer in RAM).
 *
 * MUST run with: OFFSET_WIDTH=3 PHYSICAL_ADDRESS_WIDTH=5 VIRTUAL_ADDRESS_WIDTH=6
 *
 * g++:  g++ -std=c++17 -DOFFSET_WIDTH=3 -DPHYSICAL_ADDRESS_WIDTH=5 \
 *            -DVIRTUAL_ADDRESS_WIDTH=6 -I src/ src/VirtualMemory.cpp \
 *            src/PhysicalMemory.cpp \
 *            tests/advanced_tests/advanced_test3_VMgetMappingAfterEviction.cpp \
 *            -o at3 && ./at3
 * school: ~os/ex/ex3/run_school_solution example/overrides.h \
 *            tests/advanced_tests/advanced_test3_VMgetMappingAfterEviction.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // PAGE_SIZE=8, NUM_FRAMES=4 — only 3 data frames available
    VMwrite(0,  100);   // page 0 → frame 1
    VMwrite(8,  200);   // page 1 → frame 2
    VMwrite(16, 300);   // page 2 → frame 3   (all full)

    uint64_t before = VMgetMapping(0);
    std::cout << "Before eviction: " << (before != 0 ? "YES" : "NO") << std::endl;

    // Insert page 3 → evicts page 0 (highest cyclical distance = 3)
    VMwrite(24, 400);

    uint64_t after = VMgetMapping(0);
    std::cout << "After eviction: " << (after != 0 ? "YES" : "NO") << std::endl;

    bool ok = (before != 0) && (after == 0);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
