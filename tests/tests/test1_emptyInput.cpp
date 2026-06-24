/*
 * test1_emptyInput
 * Read from virtual address 0 immediately after VMinitialize, without
 * any prior write.  Physical memory is zero-initialised and PMrestore
 * is a no-op for a page that was never evicted, so the value must be 0.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test1_emptyInput.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    word_t value = 0xDEAD; // pre-set to something non-zero to detect wrong 0
    VMread(0, &value);
    std::cout << "Value at address 0 (unwritten): " << value << std::endl;

    return 0;
}
