
#define __variant__ 1
#define CATEGORY___nist-level__ 1
#define __target__ 1

#if defined(SPEED)
    #define NO_TREES 1
#else
    #undef NO_TREES
#endif

/* When compiling avx2 override the behaviour of architecture_detect.h */
#define IMPLEMENTATION___implementation__
#if defined(IMPLEMENTATION_avx2)
    #define HIGH_COMPATIBILITY_X86_64
    #define HIGH_PERFORMANCE_X86_64
#endif

/* Undefine unused macros to facilitate dead code removal using unifdef */

/* Variant */
#if defined(RSDP)
    #undef RSDPG
#else
    #undef RSDP
#endif
/* Category */
#ifndef CATEGORY_1
    #undef CATEGORY_1
#endif
#ifndef CATEGORY_2
    #undef CATEGORY_2
#endif
#ifndef CATEGORY_3
    #undef CATEGORY_3
#endif
/* Target */
#ifndef BALANCED
    #undef BALANCED
#endif
#ifndef SPEED
    #undef SPEED
#endif
#ifndef SIG_SIZE
    #undef SIG_SIZE
#endif