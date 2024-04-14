#pragma once

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE (168)
#define SHAKE256_RATE (136)
#define SHA3_256_RATE (136)
#define SHA3_384_RATE (104)
#define SHA3_512_RATE ( 72)


// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} shake128incctx;

// Context for non-incremental API
typedef struct {
   uint64_t ctx[25];
} shake128ctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} shake256incctx;

// Context for non-incremental API
typedef struct {
   uint64_t ctx[25];
} shake256ctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} sha3_256incctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} sha3_384incctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} sha3_512incctx;

/* Initialize the state and absorb the provided input.
 * This function does not support being called multiple times
 * with the same state. */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_absorb(shake128ctx *state, const uint8_t *input, size_t inlen);
/* Squeeze output out of the sponge.
 * Supports being called multiple times */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_squeezeblocks(uint8_t *output, size_t nblocks,
                            shake128ctx *state);


/* Initialize incremental hashing API */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_init(shake128incctx *state);
/* Absorb more information into the XOF.
 * Can be called multiple times. */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_absorb(shake128incctx *state, const uint8_t *input,
                         size_t inlen);
/* Finalize the XOF for squeezing */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_finalize(shake128incctx *state);
/* Squeeze output out of the sponge.
 * Supports being called multiple times */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_squeeze(uint8_t *output, size_t outlen,
                          shake128incctx *state);


/* Initialize the state and absorb the provided input.
 * This function does not support being called multiple times
 * with the same state. */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_absorb(shake256ctx *state, const uint8_t *input, size_t inlen);
/* Squeeze output out of the sponge.
 * Supports being called multiple times */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_squeezeblocks(uint8_t *output, size_t nblocks,
                            shake256ctx *state);

/* Initialize incremental hashing API */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_init(shake256incctx *state);
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_absorb(shake256incctx *state, const uint8_t *input,
                         size_t inlen);
/* Prepares for squeeze phase */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_finalize(shake256incctx *state);
/* Squeeze output out of the sponge.
 * Supports being called multiple times */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_squeeze(uint8_t *output, size_t outlen,
                          shake256incctx *state);


/* One-stop SHAKE128 call */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen);
/* One-stop SHAKE256 call */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen);

/* Initialize the incremental hashing state */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_256_inc_init(sha3_256incctx *state);
/* Absorb blocks into SHA3 */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_256_inc_absorb(sha3_256incctx *state, const uint8_t *input,
                         size_t inlen);
/* Obtain the output of the function and free `state` */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_256_inc_finalize(uint8_t *output, sha3_256incctx *state);
/* One-stop SHA3-256 shop */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_256(uint8_t *output, const uint8_t *input, size_t inlen);

/* Initialize the incremental hashing state */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_384_inc_init(sha3_384incctx *state);
/* Absorb blocks into SHA3 */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_384_inc_absorb(sha3_384incctx *state, const uint8_t *input,
                         size_t inlen);
/* Obtain the output of the function and free `state` */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_384_inc_finalize(uint8_t *output, sha3_384incctx *state);
/* One-stop SHA3-384 shop */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_384(uint8_t *output, const uint8_t *input, size_t inlen);

/* Initialize the incremental hashing state */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_512_inc_init(sha3_512incctx *state);
/* Absorb blocks into SHA3 */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_512_inc_absorb(sha3_512incctx *state, const uint8_t *input,
                         size_t inlen);
/* Obtain the output of the function and free `state` */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_512_inc_finalize(uint8_t *output, sha3_512incctx *state);
/* One-stop SHA3-512 shop */
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_sha3_512(uint8_t *output, const uint8_t *input, size_t inlen);

void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_ctx_clone(shake128ctx *dest, const shake128ctx *src);
void PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_ctx_clone(shake256ctx *dest, const shake256ctx *src);
