
#define RSDPG 1
#define CATEGORY_3 1
#define BALANCED 1

#undef NO_TREES

/* When compiling avx2 override the behaviour of architecture_detect.h */
#define IMPLEMENTATION_avx2
#define HIGH_COMPATIBILITY_X86_64
#define HIGH_PERFORMANCE_X86_64

/* PQClean-edit: avoid VLA */
#define SIZEOF_UINT16 2

/* Undefine unused macros to facilitate dead code removal using unifdef */

#undef SHA_3_LIBKECCAK

/* Variant */
#undef RSDP
/* Category */
#undef CATEGORY_1
#undef CATEGORY_5
/* Target */
#undef SPEED
#undef SIG_SIZE
