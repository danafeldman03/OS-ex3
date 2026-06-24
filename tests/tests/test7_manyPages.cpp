/*
 * test7_manyPages
 * Write a distinct value to the first word of each of the first 50
 * pages, then read all 50 back and verify.
 *
 * With the DEFAULT memory constants (NUM_FRAMES=64, TABLES_DEPTH=4) the
 * 50 sequential pages require exactly 60 frames (4 shared table frames +
 * 8 table frames for pages 16-49 + 50 page frames), so no eviction is
 * triggered.  This test exercises the multi-level page-table traversal
 * across several different L2 / L3 groups.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test7_manyPages.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    const int N = 50;

    for (int i = 0; i < N; ++i) {
        VMwrite((uint64_t)i * PAGE_SIZE, (word_t)(i + 1));
    }

    bool passed = true;
    for (int i = 0; i < N; ++i) {
        word_t val = 0;
        VMread((uint64_t)i * PAGE_SIZE, &val);
        if (val != (word_t)(i + 1)) {
            std::cout << "FAIL at page " << i
                      << ": expected " << (i + 1)
                      << " got " << val << std::endl;
            passed = false;
        }
    }

    std::cout << (passed ? "PASS" : "FAIL") << std::endl;
    return 0;
}
