
#define RSDP 1
#define CATEGORY_1 1
#define SIG_SIZE 1

    #undef NO_TREES

/* When compiling avx2 override the behaviour of architecture_detect.h */
#define IMPLEMENTATION_clean

/* Undefine unused macros to facilitate dead code removal using unifdef */

/* Variant */
    #undef RSDPG
/* Category */
    #undef CATEGORY_2
    #undef CATEGORY_3
/* Target */
    #undef BALANCED
    #undef SPEED
