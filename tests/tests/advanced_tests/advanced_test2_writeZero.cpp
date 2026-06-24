/*
 * advanced_test2_writeZero
 *
 * Explicitly writes the value 0 to a valid address and reads it back.
 * This verifies that the implementation does not confuse a zero DATA
 * value with the zero PAGE-TABLE sentinel ("page not in RAM").
 * The zero lives in page-data physical memory, not in a table entry,
 * so there must be no confusion.
 *
 * Expected: value == 0, VMread return code == 1 (success).
 *
 * Run with default constants.
 * g++:  g++ -std=c++17 -I src/ src/VirtualMemory.cpp src/PhysicalMemory.cpp \
 *            tests/advanced_tests/advanced_test2_writeZero.cpp -o at2 && ./at2
 * school: ~os/ex/ex3/run_school_solution \
 *            tests/advanced_tests/advanced_test2_writeZero.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    uint64_t addr = 100;
    VMwrite(addr, 0);

    word_t val = 0xDEAD;  // pre-set to non-zero to detect no-write bugs
    int rc = VMread(addr, &val);

    std::cout << "Value: " << val << std::endl;
    std::cout << "Return code: " << rc << std::endl;

    bool ok = (val == 0) && (rc == 1);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
