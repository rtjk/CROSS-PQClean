
#pragma once

#define RSDP 1
#define CATEGORY_1 1
#define BALANCED 1

/* #define AES_CTR_CSPRNG
#define SHA3_HASH */
/* #define OMIT_SEED_TREE ""
#define KECCAK_EXTERNAL_ENABLE */

//  #define SHA_3_LIBKECCAK
//  #define SPEED 1
//  #define NO_TREES 1
//  #define SIG_SIZE 1

#if defined(SPEED)
    #define NO_TREES 1
#endif