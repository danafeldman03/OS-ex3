/*
 * advanced_test6_multiLevelFault
 *
 * Verifies that the 4-level page-fault chain (all four table levels
 * missing at once) is handled correctly without accidentally evicting
 * a table just created at an earlier level in the same chain.
 *
 * The PDF tip warns: "Make sure that you create these from top to bottom
 * and that you don't accidentally evict a table you have just created."
 *
 * With default constants (TABLES_DEPTH=4):
 *   - Page 0  (L0=0, L1=0, L2=0, L3=0): accessing it for the first time
 *     creates four new frames in sequence (L1, L2, L3 tables + page frame).
 *     Each new frame is an "empty table" w.r.t. the DFS — the parentFrameToAvoid
 *     guard must protect it from being reclaimed by the next level's search.
 *
 *   - Page 4096 (L0=1, L1=0, L2=0, L3=0, virtual addr 65536):
 *     entirely new L0 subtree → again triggers all four levels of faults,
 *     each needing to allocate a new frame while the previous level's frame
 *     appears empty to the DFS. Tests that the guard works across all 4 levels.
 *
 * Both pages' values must survive and be readable.
 *
 * Run with default constants.
 * g++:  g++ -std=c++17 -I src/ src/VirtualMemory.cpp src/PhysicalMemory.cpp \
 *            tests/advanced_tests/advanced_test6_multiLevelFault.cpp -o at6 && ./at6
 * school: ~os/ex/ex3/run_school_solution \
 *            tests/advanced_tests/advanced_test6_multiLevelFault.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // Page 0: all four table levels created fresh
    uint64_t addrPage0    = 0;
    VMwrite(addrPage0, 12345);

    // Page 4096 (virtual addr = 4096 * PAGE_SIZE = 65536, L0=1):
    // completely different L0 subtree → all four levels fault again
    uint64_t addrPage4096 = (uint64_t)4096 * PAGE_SIZE;
    VMwrite(addrPage4096, 67890);

    word_t v0 = 0, v4096 = 0;
    VMread(addrPage0,    &v0);
    VMread(addrPage4096, &v4096);

    std::cout << "Page 0 value: "    << v0    << std::endl;
    std::cout << "Page 4096 value: " << v4096 << std::endl;

    bool ok = (v0 == 12345) && (v4096 == 67890);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
