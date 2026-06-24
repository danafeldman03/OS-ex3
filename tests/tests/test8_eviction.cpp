/*
 * test8_eviction
 * Tests that data survives a page eviction and is correctly restored
 * from the swap file.
 *
 * This test MUST be compiled with the small-memory override so that
 * eviction is triggered with only a few page accesses:
 *   OFFSET_WIDTH=3  PHYSICAL_ADDRESS_WIDTH=5  VIRTUAL_ADDRESS_WIDTH=6
 *   → PAGE_SIZE=8, NUM_FRAMES=4, NUM_PAGES=8, TABLES_DEPTH=1
 *   → Frame 0 = root table; only 3 data frames (1-3) available.
 *
 * Run (with override):
 *   ~os/ex/ex3/run_school_solution example/overrides.h tests/test8_eviction.cpp
 *
 * Step-by-step:
 *   1. Fill all 3 data frames: page 0 → frame 1, page 1 → frame 2, page 2 → frame 3
 *   2. Access page 3: cyclical distances from page 3 are
 *        page 0: min(3,5)=3, page 1: min(2,6)=2, page 2: min(1,7)=1
 *      → evict page 0 (max distance).  Frame 1 now holds page 3.
 *   3. Read page 0 → must be restored from swap (value 111).
 *   4. Read page 3 → must be restored from swap (value 444).
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // Fill all 3 data frames
    VMwrite(0,  111);   // page 0 → frame 1
    VMwrite(8,  222);   // page 1 → frame 2
    VMwrite(16, 333);   // page 2 → frame 3

    // This access forces eviction of page 0
    VMwrite(24, 444);   // page 3 → frame 1 (page 0 evicted to swap)

    word_t val = 0;

    VMread(0, &val);    // restore page 0 from swap
    std::cout << "Page 0: " << val << std::endl;

    VMread(24, &val);   // restore page 3 from swap
    std::cout << "Page 3: " << val << std::endl;

    return 0;
}
