
#define RSDP 1
#define CATEGORY_3 1
#define BALANCED 1

    #undef NO_TREES

/* When compiling avx2 override the behaviour of architecture_detect.h */
#define IMPLEMENTATION_clean

/* Undefine unused macros to facilitate dead code removal using unifdef */

/* VARIANT */
    #undef RSDPG
/* CATEGORY */
    #undef CATEGORY_1
    #undef CATEGORY_2
/* TARGET */
    #undef SPEED
    #undef SIG_SIZE
