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

#pragma once

#if defined(SHA_3_LIBKECCAK)
#include <libkeccak.a.headers/KeccakHash.h>

/* LibKeccak SHAKE Wrappers */

#define SHAKE_STATE_STRUCT Keccak_HashInstance
static inline
void xof_shake_init(SHAKE_STATE_STRUCT *state, int val)
{
   if (val == 128)
      /* will result in a zero-length output for Keccak_HashFinal */
      Keccak_HashInitialize_SHAKE128(state);
   else
      /* will result in a zero-length output for Keccak_HashFinal */
      Keccak_HashInitialize_SHAKE256(state);
}

static inline
void xof_shake_update(SHAKE_STATE_STRUCT *state,
                      const unsigned char *input,
                      uint32_t inputByteLen)
{
   Keccak_HashUpdate(state,
                     (const BitSequence *) input,
                     (BitLength) inputByteLen*8 );
}

static inline
void xof_shake_final(SHAKE_STATE_STRUCT *state)
{
   Keccak_HashFinal(state, NULL);
}

static inline
void xof_shake_extract(SHAKE_STATE_STRUCT *state,
                       unsigned char *output,
                       uint32_t outputByteLen)
{
   Keccak_HashSqueeze(state,
                      (BitSequence *) output,
                      (BitLength) outputByteLen*8 );
}

#else
#include "fips202_CROSS.h"
/* standalone FIPS-202 implementation has 
 * different states for SHAKE depending on security level*/
#if defined(CATEGORY_1)
#define SHAKE_STATE_STRUCT shake128incctx
#else
#define SHAKE_STATE_STRUCT shake256incctx
#endif
// %%%%%%%%%%%%%%%%%% Self-contained SHAKE Wrappers %%%%%%%%%%%%%%%%%%%%%%%%%%%%

static inline
void xof_shake_init(SHAKE_STATE_STRUCT *state, int val)
{
   if(val == 0) {val = 0;}; // TODO: useless line added to avoid -Werror=unused-parameter and Werror=unused-value
#if defined(CATEGORY_1)
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_init(state);
#else
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_init(state);
#endif
}

static inline
void xof_shake_update(SHAKE_STATE_STRUCT *state,
                      const unsigned char *input,
                      uint32_t inputByteLen)
{
#if defined(CATEGORY_1)
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_absorb(state,
                       (const uint8_t *)input,
                       inputByteLen);
#else
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_absorb(state,
                       (const uint8_t *)input,
                       inputByteLen);
#endif
}

static inline
void xof_shake_final(SHAKE_STATE_STRUCT *state)
{
#if defined(CATEGORY_1)
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_finalize(state);
#else
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_finalize(state);
#endif
}

static inline
void xof_shake_extract(SHAKE_STATE_STRUCT *state,
                       unsigned char *output,
                       uint32_t outputByteLen){
#if defined(CATEGORY_1)
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake128_inc_squeeze(output, outputByteLen, state);
   // TODO: release CSPRNG_state
#else
   PQCLEAN_CROSSRSDPG256SMALL_AVX2_shake256_inc_squeeze(output, outputByteLen, state);
   // TODO: release CSPRNG_state
#endif
}
#endif
