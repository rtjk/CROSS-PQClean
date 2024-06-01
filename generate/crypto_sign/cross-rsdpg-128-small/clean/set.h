
#pragma once

#define RSDPG 1
#define CATEGORY_1 1
#define SIG_SIZE 1

#if defined(SPEED)
    #define NO_TREES 1
#endif

// When compiling avx2 override the behaviour of architecture_detect.h
#define IMPLEMENTATION_clean
#if defined(IMPLEMENTATION_avx2)
    #define HIGH_COMPATIBILITY_X86_64
    #define HIGH_PERFORMANCE_X86_64
#endif