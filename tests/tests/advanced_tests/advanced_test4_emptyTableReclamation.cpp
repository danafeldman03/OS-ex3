/*
 * advanced_test4_emptyTableReclamation  (Priority 1 test)
 *
 * Verifies that an intermediate table frame whose all entries have been
 * zeroed out (because all pages in its subtree were evicted) is reclaimed
 * under Priority 1, instead of evicting a live page under Priority 3.
 *
 * Config: OFFSET_WIDTH=2, PHYSICAL_ADDRESS_WIDTH=4, VIRTUAL_ADDRESS_WIDTH=6
 *   PAGE_SIZE=4, NUM_FRAMES=4, NUM_PAGES=16, TABLES_DEPTH=2
 *   L0 index = virtualAddress >> 4   (top 2 bits of page number)
 *   L1 index = (virtualAddress >> 2) & 3
 *   Virtual address of page p = p * 4
 *
 * Pages 0-3 share L0=0 (same L1 subtree, frame 1 = L1_0 table).
 * Pages 4-5 share L0=1 (different L1 subtree, frame 2 = L1_1 table).
 *
 * Sequence (hand-traced; all 4 frames are used after step 2):
 *   1. Write page 0 (val 100) + page 1 (val 200)  →  frames full
 *   2. Write page 2 (val 300)  →  evicts page 0  (dist 2 > dist of page1)
 *   3. Write page 3 (val 400)  →  evicts page 1
 *      After steps 2-3: L1_0 = [0,0,page2_frame,page3_frame]
 *   4. Write page 4 (val 500, L0=1)  →  two-level fault:
 *        depth-0 fault evicts page 2 to free a frame for new L1_1 table.
 *        depth-1 fault evicts page 3 to free a frame for page 4's data.
 *        After this: L1_0 has ALL ZEROS (both children evicted).
 *   5. Write page 5 (val 600, L0=1, L1=1)  →  depth-1 fault:
 *        DFS finds L1_0 (frame 1) is empty → Priority 1 reclaims it.
 *        Page 4 is NOT evicted (Priority 3 would have evicted it).
 *
 * Key check: after writing page 5, VMgetMapping(4) must still be non-zero
 * (page 4 is still in RAM — Priority 1 fired instead of Priority 3).
 *
 * MUST run with: OFFSET_WIDTH=2 PHYSICAL_ADDRESS_WIDTH=4 VIRTUAL_ADDRESS_WIDTH=6
 *
 * g++:  g++ -std=c++17 -DOFFSET_WIDTH=2 -DPHYSICAL_ADDRESS_WIDTH=4 \
 *            -DVIRTUAL_ADDRESS_WIDTH=6 -I src/ src/VirtualMemory.cpp \
 *            src/PhysicalMemory.cpp \
 *            tests/advanced_tests/advanced_test4_emptyTableReclamation.cpp \
 *            -o at4 && ./at4
 * school: ~os/ex/ex3/run_school_solution \
 *            tests/advanced_tests/overrides_depth2.h \
 *            tests/advanced_tests/advanced_test4_emptyTableReclamation.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // Step 1: fill RAM with pages 0 and 1 (L0=0 group)
    VMwrite(0,  100);   // page 0
    VMwrite(4,  200);   // page 1   →  all 4 frames full

    // Step 2-3: evict pages 0 and 1 one by one, filling with pages 2 and 3
    VMwrite(8,  300);   // page 2 — evicts page 0
    VMwrite(12, 400);   // page 3 — evicts page 1
    // Now L1_0 = [0, 0, frame_page2, frame_page3]

    // Step 4: access page 4 (L0=1, completely new subtree)
    //   depth-0 fault: creates L1_1 table (evicts page 2 to get a frame)
    //   depth-1 fault: places page 4   (evicts page 3 to get a frame)
    //   After this: L1_0 has ALL ZEROS → empty table
    VMwrite(16, 500);   // page 4

    // Step 5: access page 5 (L0=1, L1=1)
    //   Priority 1 should reclaim the empty L1_0 frame (frame 1)
    //   instead of evicting page 4 (Priority 3 would choose page 4,
    //   since dist(page4, pageIn=5) = 1 is the only candidate).
    VMwrite(20, 600);   // page 5

    // If Priority 1 fired: page 4 is still in RAM.
    // If Priority 3 fired (bug): page 4 was evicted, mapping returns 0.
    uint64_t map4 = VMgetMapping(4);
    std::cout << "Page 4 still in RAM: " << (map4 != 0 ? "YES" : "NO") << std::endl;

    word_t v4 = 0, v5 = 0;
    VMread(16, &v4);
    VMread(20, &v5);
    std::cout << "Page 4 value: " << v4 << std::endl;
    std::cout << "Page 5 value: " << v5 << std::endl;

    bool ok = (map4 != 0) && (v4 == 500) && (v5 == 600);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
