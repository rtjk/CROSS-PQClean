/**
 *
 * Reference ISO-C11 Implementation of CROSS.
 *
 * @version 1.1 (March 2023)
 *
 * @author Alessandro Barenghi <alessandro.barenghi@polimi.it>
 * @author Gerardo Pelosi <gerardo.pelosi@polimi.it>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

/* Based on the public domain implementation in
 * crypto_hash/keccakc512/simple/ from http://bench.cr.yp.to/supercop.html
 * by Ronny Van Keer
 * and the public domain "TweetFips202" implementation
 * from https://twitter.com/tweetfips202
 * by Gilles Van Assche, Daniel J. Bernstein, and Peter Schwabe */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fips202_CROSS.h"
#include "keccakf1600.h"

#define NROUNDS 24
#define ROL(a, offset) (((a) << (offset)) ^ ((a) >> (64 - (offset))))


/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_absorb
 *
 * Description: Absorb step of Keccak;
 *              non-incremental, starts by zeroeing the state.
 *
 * Arguments:   - uint64_t *s:       pointer to (uninitialized) output Keccak state
 *              - uint32_t r:        rate in bytes (e.g., 168 for SHAKE128)
 *              - const uint8_t *m:  pointer to input to be absorbed into s
 *              - size_t mlen:       length of input in bytes
 *              - uint8_t p:         domain-separation byte for different Keccak-derived functions
 **************************************************/
static void PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_absorb(uint64_t *s,
                          uint32_t r,
                          const uint8_t *m, size_t mlen,
                          uint8_t p)
{
   while (mlen >= r) {
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s, m, 0, r);
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StatePermute(s);
      mlen -= r;
      m += r;
   }

   if(mlen > 0) {
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s, m, 0, mlen);
   }

   if(mlen == r-1) {
      p |= 128;
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s, &p, mlen, 1);
   } else {
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s, &p, mlen, 1);
      p = 128;
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s, &p, r-1, 1);
   }
}


/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks
 *
 * Description: Squeeze step of Keccak. Squeezes full blocks of r bytes each.
 *              Modifies the state. Can be called multiple times to keep squeezing,
 *              i.e., is incremental.
 *
 * Arguments:   - uint8_t *h:     pointer to output blocks
 *              - size_t nblocks: number of blocks to be squeezed (written to h)
 *              - uint64_t *s:    pointer to in/output Keccak state
 *              - uint32_t r:     rate in bytes (e.g., 168 for SHAKE128)
 **************************************************/
static void PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks(uint8_t *h, size_t nblocks,
                                 uint64_t *s, uint32_t r)
{
   while(nblocks > 0) {
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StatePermute(s);
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateExtractBytes(s, h, 0, r);
      h += r;
      nblocks--;
   }
}

/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init
 *
 * Description: Initializes the incremental Keccak state to zero.
 *
 * Arguments:   - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 **************************************************/
static void PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(uint64_t *s_inc)
{
   size_t i;

   for (i = 0; i < 25; ++i) {
      s_inc[i] = 0;
   }
   s_inc[25] = 0;
}
/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb
 *
 * Description: Incremental keccak absorb
 *              Preceded by PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init, succeeded by PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize
 *
 * Arguments:   - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 *              - uint32_t r: rate in bytes (e.g., 168 for SHAKE128)
 *              - const uint8_t *m: pointer to input to be absorbed into s_inc
 *              - size_t mlen: length of input in bytes
 **************************************************/
static void PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(uint64_t *s_inc, uint32_t r, const uint8_t *m,
                              size_t mlen)
{
   /* Recall that s_inc[25] is the non-absorbed bytes xored into the state */
   while (mlen + s_inc[25] >= r) {

      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s_inc, m, s_inc[25], r-s_inc[25]);
      mlen -= (size_t)(r - s_inc[25]);
      m += r - s_inc[25];
      s_inc[25] = 0;

      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StatePermute(s_inc);
   }

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s_inc, m, s_inc[25], mlen);
   s_inc[25] += mlen;
}

/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize
 *
 * Description: Finalizes Keccak absorb phase, prepares for squeezing
 *
 * Arguments:   - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 *              - uint32_t r: rate in bytes (e.g., 168 for SHAKE128)
 *              - uint8_t p: domain-separation byte for different
 *                                 Keccak-derived functions
 **************************************************/
static void PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(uint64_t *s_inc, uint32_t r, uint8_t p)
{
   /* After PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb, we are guaranteed that s_inc[25] < r,
      so we can always use one more byte for p in the current state. */
   if(s_inc[25] == r-1) {
      p |= 128;
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s_inc, &p, s_inc[25], 1);
   } else {
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s_inc, &p, s_inc[25], 1);
      p = 128;
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateXORBytes(s_inc, &p, r-1, 1);
   }
   s_inc[25] = 0;
}

/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze
 *
 * Description: Incremental Keccak squeeze; can be called on byte-level
 *
 * Arguments:   - uint8_t *h: pointer to output bytes
 *              - size_t outlen: number of bytes to be squeezed
 *              - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 *              - uint32_t r: rate in bytes (e.g., 168 for SHAKE128)
 **************************************************/
#include <stdio.h> // TODO: remove
static void PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(uint8_t *h, size_t outlen,
                               uint64_t *s_inc, uint32_t r)
{

/*     printf("\n**** here ***");
    printf("\n**** here %ld ****", outlen);
    printf("\n**** here %ld ****\n", s_inc[0]); */

   size_t len;
   if(outlen < s_inc[25]) {
      len = outlen;
   } else {
      len = s_inc[25];
   }

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateExtractBytes(s_inc, h, r-s_inc[25], len);
   h += len;
   outlen -= len;
   s_inc[25] -= len;

   /* Then squeeze the remaining necessary blocks */
   while (outlen > 0) {
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StatePermute(s_inc);

      if(outlen < r) {
         len = outlen;
      } else {
         len = r;
      }
      PQCLEAN_CROSSRSDP256BALANCED_AVX2_KeccakF1600_StateExtractBytes(s_inc, h, 0, len);
      h += len;
      outlen -= len;
      s_inc[25] = r - len;
   }
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_inc_init(shake128incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state->ctx);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_inc_absorb(shake128incctx *state, const uint8_t *input,
                         size_t inlen)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state->ctx, SHAKE128_RATE, input, inlen);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_inc_finalize(shake128incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state->ctx, SHAKE128_RATE, 0x1F);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_inc_squeeze(uint8_t *output, size_t outlen,
                          shake128incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, outlen, state->ctx, SHAKE128_RATE);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_inc_init(shake256incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state->ctx);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_inc_absorb(shake256incctx *state, const uint8_t *input,
                         size_t inlen)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state->ctx, SHAKE256_RATE, input, inlen);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_inc_finalize(shake256incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state->ctx, SHAKE256_RATE, 0x1F);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_inc_squeeze(uint8_t *output, size_t outlen,
                          shake256incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, outlen, state->ctx, SHAKE256_RATE);
}

/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_absorb
 *
 * Description: Absorb step of the SHAKE128 XOF.
 *              non-incremental, starts by zeroeing the state.
 *
 * Arguments:   - uint64_t *state:      pointer to (uninitialized) output Keccak state
 *              - const uint8_t *input: pointer to input to be absorbed into state
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_absorb(shake128ctx *state, const uint8_t *input, size_t inlen)
{
   int i;
   for (i = 0; i < 25; i++)
      state->ctx[i] = 0;
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_absorb(state->ctx, SHAKE128_RATE, input, inlen, 0x1F);
}

/*************************************************
 * Name:        PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_squeezeblocks
 *
 * Description: Squeeze step of SHAKE128 XOF. Squeezes full blocks of SHAKE128_RATE bytes each.
 *              Modifies the state. Can be called multiple times to keep squeezing,
 *              i.e., is incremental.
 *
 * Arguments:   - uint8_t *output:     pointer to output blocks
 *              - size_t nblocks:      number of blocks to be squeezed (written to output)
 *              - shake128ctx *state:  pointer to in/output Keccak state
 **************************************************/
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_squeezeblocks(uint8_t *output, size_t nblocks,
                            shake128ctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks(output, nblocks, state->ctx, SHAKE128_RATE);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen)
{
   shake128incctx state;
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state.ctx);

   /* Absorb input */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state.ctx, SHAKE128_RATE, input, inlen);
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state.ctx, SHAKE128_RATE, 0x1F);

   /* Squeeze output */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, outlen, state.ctx, SHAKE128_RATE);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake128_ctx_clone(shake128ctx *dest, const shake128ctx *src)
{
   memcpy(dest, src, sizeof(shake128ctx));
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_absorb(shake256ctx *state, const uint8_t *input, size_t inlen)
{
   int i;
   for (i = 0; i < 25; i++)
      state->ctx[i] = 0;

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_absorb(state->ctx, SHAKE256_RATE, input, inlen, 0x1F);
}


void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_squeezeblocks(uint8_t *output, size_t nblocks,
                            shake256ctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks(output, nblocks, state->ctx, SHAKE256_RATE);
}

/*************************************************
 * Name:        shake256
 *
 * Description: SHAKE256 XOF with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - size_t outlen:        requested output length in bytes
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen)
{
   shake256incctx state;

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state.ctx);

   /* Absorb input */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state.ctx, SHAKE256_RATE, input, inlen);
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state.ctx, SHAKE256_RATE, 0x1F);

   /* Squeeze output */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, outlen, state.ctx, SHAKE256_RATE);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_shake256_ctx_clone(shake256ctx *dest, const shake256ctx *src)
{
   memcpy(dest, src, sizeof(shake256ctx));
}


/*************************************************
 * Name:        sha3_256
 *
 * Description: SHA3-256 with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_256(uint8_t *output, const uint8_t *input, size_t inlen)
{
   sha3_256incctx state;
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state.ctx);

   /* Absorb input */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state.ctx, SHA3_256_RATE, input, inlen);
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state.ctx, SHA3_256_RATE, 0x06);

   /* Squeeze output */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, 32, state.ctx, SHA3_256_RATE);
}
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_256_inc_init(sha3_256incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state->ctx);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_256_inc_absorb(sha3_256incctx *state, const uint8_t *input,
                         size_t inlen)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state->ctx, SHA3_256_RATE, input, inlen);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_256_inc_finalize(uint8_t *output, sha3_256incctx *state)
{
   uint8_t t[SHA3_256_RATE];
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state->ctx, SHA3_256_RATE, 0x06);

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks(t, 1, state->ctx, SHA3_256_RATE);

   for (size_t i = 0; i < 32; i++) {
      output[i] = t[i];
   }
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_384_inc_init(sha3_384incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state->ctx);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_384_inc_absorb(sha3_384incctx *state, const uint8_t *input,
                         size_t inlen)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state->ctx, SHA3_384_RATE, input, inlen);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_384_inc_finalize(uint8_t *output, sha3_384incctx *state)
{
   uint8_t t[SHA3_384_RATE];
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state->ctx, SHA3_384_RATE, 0x06);

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks(t, 1, state->ctx, SHA3_384_RATE);

   for (size_t i = 0; i < 48; i++) {
      output[i] = t[i];
   }
}

/*************************************************
 * Name:        sha3_384
 *
 * Description: SHA3-256 with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:   length of input in bytes
 **************************************************/
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_384(uint8_t *output, const uint8_t *input, size_t inlen)
{
   sha3_384incctx state;
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state.ctx);

   /* Absorb input */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state.ctx, SHA3_384_RATE, input, inlen);
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state.ctx, SHA3_384_RATE, 0x06);

   /* Squeeze output */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, 48, state.ctx, SHA3_384_RATE);
}

/*************************************************
 * Name:        sha3_512
 *
 * Description: SHA3-512 with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_512(uint8_t *output, const uint8_t *input, size_t inlen)
{
   sha3_512incctx state;
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state.ctx);

   /* Absorb input */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state.ctx, SHA3_512_RATE, input, inlen);
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state.ctx, SHA3_512_RATE, 0x06);

   /* Squeeze output */
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_squeeze(output, 64, state.ctx, SHA3_512_RATE);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_512_inc_init(sha3_512incctx *state)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_init(state->ctx);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_512_inc_absorb(sha3_512incctx *state, const uint8_t *input,
                         size_t inlen)
{
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_absorb(state->ctx, SHA3_512_RATE, input, inlen);
}

void PQCLEAN_CROSSRSDP256BALANCED_AVX2_sha3_512_inc_finalize(uint8_t *output, sha3_512incctx *state)
{
   uint8_t t[SHA3_512_RATE];
   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_inc_finalize(state->ctx, SHA3_512_RATE, 0x06);

   PQCLEAN_CROSSRSDP256BALANCED_AVX2_keccak_squeezeblocks(t, 1, state->ctx, SHA3_512_RATE);

   for (size_t i = 0; i < 64; i++) {
      output[i] = t[i];
   }
}
