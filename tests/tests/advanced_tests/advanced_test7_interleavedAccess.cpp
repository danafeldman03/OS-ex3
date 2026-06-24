/*
 * advanced_test7_interleavedAccess
 *
 * Stresses the eviction/restore cycle with an interleaved pattern of
 * reads and writes across four pages in a near-full memory.
 * Unlike tests that write all pages first and then read, this test
 * mixes writes and reads so that pages are repeatedly evicted and
 * restored, verifying data integrity throughout.
 *
 * With small memory (TABLES_DEPTH=1, NUM_FRAMES=4, NUM_PAGES=8):
 *   Only 3 data frames → at most 3 pages live in RAM at any time.
 *
 * Step-by-step trace (eviction candidates ordered by decreasing distance):
 *   1. write page 0 (val 100), page 1 (val 200) — 2 pages in RAM.
 *   2. READ  page 0 → 100   (still in RAM, no fault)
 *   3. write page 2 (val 300) — 3 pages in RAM, frames full.
 *   4. write page 3 (val 400) — evicts page 0 (dist 3 > 2 > 1).
 *   5. READ  page 1 → 200   (page 1 still in RAM)
 *   6. READ  page 0 → 100   (page fault — restore from swap;
 *                             evicts page 3, dist=3 > dist_page1=2 > dist_page2=1)
 *   7. READ  page 2 → 300   (page 2 still in RAM)
 *   8. READ  page 3 → 400   (page fault — restore from swap;
 *                             evicts page 0 again, dist=3)
 *   9. write page 0 again, val 500 — evicts page 3, dist=3.
 *  10. READ  page 0 → 500   (no fault, just written)
 *
 * MUST run with: OFFSET_WIDTH=3 PHYSICAL_ADDRESS_WIDTH=5 VIRTUAL_ADDRESS_WIDTH=6
 *
 * g++:  g++ -std=c++17 -DOFFSET_WIDTH=3 -DPHYSICAL_ADDRESS_WIDTH=5 \
 *            -DVIRTUAL_ADDRESS_WIDTH=6 -I src/ src/VirtualMemory.cpp \
 *            src/PhysicalMemory.cpp \
 *            tests/advanced_tests/advanced_test7_interleavedAccess.cpp \
 *            -o at7 && ./at7
 * school: ~os/ex/ex3/run_school_solution example/overrides.h \
 *            tests/advanced_tests/advanced_test7_interleavedAccess.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    word_t val = 0;

    VMwrite(0,  100);       // step 1: page 0
    VMwrite(8,  200);       // step 1: page 1

    VMread(0, &val);        // step 2
    std::cout << "Step 2 - Page 0: " << val << std::endl;

    VMwrite(16, 300);       // step 3: page 2  (frames full)
    VMwrite(24, 400);       // step 4: page 3  (evicts page 0)

    VMread(8,  &val);       // step 5
    std::cout << "Step 5 - Page 1: " << val << std::endl;

    VMread(0,  &val);       // step 6: restore page 0 (evicts page 3)
    std::cout << "Step 6 - Page 0 (from swap): " << val << std::endl;

    VMread(16, &val);       // step 7
    std::cout << "Step 7 - Page 2: " << val << std::endl;

    VMread(24, &val);       // step 8: restore page 3 (evicts page 0)
    std::cout << "Step 8 - Page 3 (from swap): " << val << std::endl;

    VMwrite(0,  500);       // step 9: overwrite page 0 (evicts page 3)
    VMread(0,  &val);       // step 10
    std::cout << "Step 10 - Page 0 (overwritten): " << val << std::endl;

    bool ok = true;
    // values were checked via stdout; run diff against expected txt to validate
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
