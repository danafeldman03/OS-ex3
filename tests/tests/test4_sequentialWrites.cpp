/*
 * test4_sequentialWrites
 * Write PAGE_SIZE distinct values to all word-addresses within a single
 * page (addresses 0 .. PAGE_SIZE-1) and read them all back.
 * Tests that offset bits are handled correctly inside one page.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test4_sequentialWrites.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // Write distinct values to every word in the first page
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        VMwrite(i, (word_t)(i * 10 + 1));
    }

    bool passed = true;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        word_t val = 0;
        VMread(i, &val);
        word_t expected = (word_t)(i * 10 + 1);
        std::cout << "addr " << i << ": expected " << expected
                  << " got " << val << std::endl;
        if (val != expected) {
            passed = false;
        }
    }

    std::cout << (passed ? "PASS" : "FAIL") << std::endl;
    return 0;
}
