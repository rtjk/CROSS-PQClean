


#pragma once

#if defined(HIGH_COMPATIBILITY_X86_64)
#include <immintrin.h>
#endif

#if defined(HIGH_PERFORMANCE_X86_64)
#include <pmmintrin.h>
#endif

#if defined(HIGH_COMPATIBILITY_X86_64)
#include <stdalign.h>
#include <wmmintrin.h>
#include <x86intrin.h>
#endif

#define EPI8_PER_REG 32
#define EPI16_PER_REG 16
