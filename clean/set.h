
#pragma once

#define __variant__ 1
#define CATEGORY___nist-level__ 1
#define __target__ 1

#if defined(SPEED)
    #define NO_TREES 1
#endif

// when compiling avx2 override the behaviour of architecture_detect.h
#define IMPLEMENTATION___implementation__
#if defined(IMPLEMENTATION_avx2)
    #define HIGH_COMPATIBILITY_X86_64
    #define HIGH_PERFORMANCE_X86_64
#endif