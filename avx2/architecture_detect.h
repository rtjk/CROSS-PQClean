
#pragma once

/* liboqs-edit: avoid checking for ISA extensions
 * when compiling avx2 just assume that Intel Instrinsics are available */
#if defined(HIGH_COMPATIBILITY_X86_64)
#include <immintrin.h>
#endif
#if defined(HIGH_PERFORMANCE_X86_64)
#include <pmmintrin.h>
#endif
#if defined(HIGH_COMPATIBILITY_X86_64)
#include <stdalign.h>
#include <wmmintrin.h>
//#include <x86intrin.h> // liboqs-edit: x86intrin.h is not available on Windows
#endif

#define EPI8_PER_REG 32
#define EPI16_PER_REG 16
