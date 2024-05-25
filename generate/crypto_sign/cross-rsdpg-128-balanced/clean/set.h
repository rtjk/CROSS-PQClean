
#pragma once

#define RSDPG 1
#define CATEGORY_1 1
#define BALANCED 1

#if defined(SPEED)
    #define NO_TREES 1
#endif

#define IMPLEMENTATION_clean
#if defined(IMPLEMENTATION_avx2)
    #define HIGH_COMPATIBILITY_X86_64
    #define HIGH_PERFORMANCE_X86_64
#endif