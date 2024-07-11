
#define RSDP 1
#define CATEGORY_3 1
#define SPEED 1

#define NO_TREES 1

/* When compiling avx2 override the behaviour of architecture_detect.h */
#define IMPLEMENTATION_clean

/* PQClean-edit: avoid VLA */
#define SIZEOF_UINT16 2

/* Undefine unused macros to facilitate dead code removal using unifdef */

#undef SHA_3_LIBKECCAK

/* Variant */
#undef RSDPG
/* Category */
#undef CATEGORY_1
#undef CATEGORY_2
/* Target */
#undef BALANCED
#undef SIG_SIZE
