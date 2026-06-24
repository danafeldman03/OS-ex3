/*
 * advanced_test1_evictionTieBreaking
 *
 * Directly exercises the tie-breaking rule stated in the PDF (Section 4):
 *   "pages 1, 5, 7 are occupied and page 2 is inserted →
 *    distances 1, 3, 3 → select page 5 (lowest page number among tied)."
 *
 * With small memory (TABLES_DEPTH=1):
 *   PAGE_SIZE=8, NUM_FRAMES=4, NUM_PAGES=8
 *   Frame 0 = root table (8 entries, one per page).
 *   Only frames 1-3 hold data pages → at most 3 pages in RAM.
 *
 * Virtual address of page p = p * 8.
 * Distances from page 2 (NUM_PAGES=8):
 *   page 1 → min(|2-1|, 8-1) = min(1,7) = 1
 *   page 5 → min(|2-5|, 8-3) = min(3,5) = 3
 *   page 7 → min(|2-7|, 8-5) = min(5,3) = 3   ← TIE with page 5
 * Tie resolved by lower page number → evict page 5, keep page 7.
 *
 * MUST run with: OFFSET_WIDTH=3 PHYSICAL_ADDRESS_WIDTH=5 VIRTUAL_ADDRESS_WIDTH=6
 * (same constants as example/overrides.h)
 *
 * g++:  g++ -std=c++17 -DOFFSET_WIDTH=3 -DPHYSICAL_ADDRESS_WIDTH=5 \
 *            -DVIRTUAL_ADDRESS_WIDTH=6 -I src/ src/VirtualMemory.cpp \
 *            src/PhysicalMemory.cpp tests/advanced_tests/advanced_test1_evictionTieBreaking.cpp \
 *            -o at1 && ./at1
 * school: ~os/ex/ex3/run_school_solution example/overrides.h \
 *            tests/advanced_tests/advanced_test1_evictionTieBreaking.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    VMwrite(8,  111);   // page 1 → frame 1
    VMwrite(40, 555);   // page 5 → frame 2
    VMwrite(56, 777);   // page 7 → frame 3   (all data frames full)

    // Insert page 2 — must evict page 5, NOT page 7
    VMwrite(16, 222);

    uint64_t m1 = VMgetMapping(1);
    uint64_t m5 = VMgetMapping(5);
    uint64_t m7 = VMgetMapping(7);

    std::cout << "Page 1 in RAM: " << (m1 != 0 ? "YES" : "NO") << std::endl;
    std::cout << "Page 5 in RAM: " << (m5 != 0 ? "YES" : "NO") << std::endl;
    std::cout << "Page 7 in RAM: " << (m7 != 0 ? "YES" : "NO") << std::endl;

    bool ok = (m1 != 0) && (m5 == 0) && (m7 != 0);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
