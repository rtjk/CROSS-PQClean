
#pragma once

#define __variant__ 1
#define CATEGORY___nist-level__ 1
#define __target__ 1

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