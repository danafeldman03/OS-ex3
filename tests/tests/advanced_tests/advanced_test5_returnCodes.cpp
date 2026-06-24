/*
 * advanced_test5_returnCodes
 *
 * Verifies that VMread and VMwrite return 1 on a valid address
 * and 0 on an out-of-range address, as stated in the spec.
 * (Previous tests check values but never assert the return codes for
 * the successful path.)
 *
 * Run with default constants.
 * g++:  g++ -std=c++17 -I src/ src/VirtualMemory.cpp src/PhysicalMemory.cpp \
 *            tests/advanced_tests/advanced_test5_returnCodes.cpp -o at5 && ./at5
 * school: ~os/ex/ex3/run_school_solution \
 *            tests/advanced_tests/advanced_test5_returnCodes.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    word_t val = 0;
    uint64_t valid   = 42;
    uint64_t invalid = VIRTUAL_MEMORY_SIZE;

    int rw1 = VMwrite(valid, 99);
    int rr1 = VMread(valid, &val);
    int rw0 = VMwrite(invalid, 99);
    int rr0 = VMread(invalid, &val);

    std::cout << "VMwrite valid addr:   " << rw1 << std::endl;
    std::cout << "VMread  valid addr:   " << rr1 << std::endl;
    std::cout << "VMwrite invalid addr: " << rw0 << std::endl;
    std::cout << "VMread  invalid addr: " << rr0 << std::endl;

    bool ok = (rw1 == 1) && (rr1 == 1) && (rw0 == 0) && (rr0 == 0);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
