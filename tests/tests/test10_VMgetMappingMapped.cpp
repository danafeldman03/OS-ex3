/*
 * test10_VMgetMappingMapped
 * After a VMwrite, VMgetMapping must return a non-zero frame index for
 * the page that contains the written address.
 * Also verifies consistency: the value read through the VM interface
 * matches the value that was written.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test10_VMgetMappingMapped.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // Write to page 0 (address 0) and page 5 (address 5 * PAGE_SIZE)
    VMwrite(0,                      0xDEAD1234);
    VMwrite((uint64_t)5 * PAGE_SIZE, 0xBEEF5678);

    uint64_t map0 = VMgetMapping(0);
    uint64_t map5 = VMgetMapping(5);

    std::cout << "Page 0 mapped: " << (map0 != 0 ? "YES" : "NO") << std::endl;
    std::cout << "Page 5 mapped: " << (map5 != 0 ? "YES" : "NO") << std::endl;

    // Sanity: values readable via VM should still be correct
    word_t v0 = 0, v5 = 0;
    VMread(0,                       &v0);
    VMread((uint64_t)5 * PAGE_SIZE, &v5);

    std::cout << "Value at page 0: 0x" << std::hex << v0 << std::endl;
    std::cout << "Value at page 5: 0x" << std::hex << v5 << std::endl;

    bool ok = (map0 != 0) && (map5 != 0)
           && (v0 == 0xDEAD1234) && (v5 == 0xBEEF5678);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
