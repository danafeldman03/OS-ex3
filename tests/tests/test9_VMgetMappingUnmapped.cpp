/*
 * test9_VMgetMappingUnmapped
 * VMgetMapping must return 0 for pages that have never been written to
 * (i.e. they are not resident in RAM).
 * It must NOT cause a page fault or allocate any frames.
 *
 * Run: ~os/ex/ex3/run_school_solution tests/test9_VMgetMappingUnmapped.cpp
 */
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <iostream>

int main() {
    VMinitialize();

    // No writes at all — every page is unmapped
    uint64_t m0 = VMgetMapping(0);
    uint64_t m1 = VMgetMapping(1);
    uint64_t mLast = VMgetMapping(NUM_PAGES - 1);

    std::cout << "Mapping of page 0 (unmapped): "    << m0    << std::endl;
    std::cout << "Mapping of page 1 (unmapped): "    << m1    << std::endl;
    std::cout << "Mapping of last page (unmapped): " << mLast << std::endl;

    bool ok = (m0 == 0) && (m1 == 0) && (mLast == 0);
    std::cout << (ok ? "PASS" : "FAIL") << std::endl;
    return 0;
}
