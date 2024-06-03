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

//////////////////////////////////////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////////////////////////////////////

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
#include "fips202.h"
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
   /* PQClean-edit: unused parameter */
   if(val == 0) {val = 0;};
#if defined(CATEGORY_1)
   shake128_inc_init(state);
#else
   shake256_inc_init(state);
#endif
}

static inline
void xof_shake_update(SHAKE_STATE_STRUCT *state,
                      const unsigned char *input,
                      uint32_t inputByteLen)
{
#if defined(CATEGORY_1)
   shake128_inc_absorb(state,
                       (const uint8_t *)input,
                       inputByteLen);
#else
   shake256_inc_absorb(state,
                       (const uint8_t *)input,
                       inputByteLen);
#endif
}

static inline
void xof_shake_final(SHAKE_STATE_STRUCT *state)
{
#if defined(CATEGORY_1)
   shake128_inc_finalize(state);
#else
   shake256_inc_finalize(state);
#endif
}

static inline
void xof_shake_extract(SHAKE_STATE_STRUCT *state,
                       unsigned char *output,
                       uint32_t outputByteLen){
#if defined(CATEGORY_1)
   shake128_inc_squeeze(output, outputByteLen, state);
#else
   shake256_inc_squeeze(output, outputByteLen, state);
#endif
}

/* PQClean-edit: CSPRNG release context */
static inline
void xof_shake_release(SHAKE_STATE_STRUCT *state)
{
#if defined(CATEGORY_1)
   shake128_inc_ctx_release(state);
#else
   shake256_inc_ctx_release(state);
#endif
}
#endif

///////////////////////////////////////////////////////////////
//                    SHAKE x4 wrappers                      //
///////////////////////////////////////////////////////////////

typedef struct {
   SHAKE_STATE_STRUCT state1;
   SHAKE_STATE_STRUCT state2;
   SHAKE_STATE_STRUCT state3;
   SHAKE_STATE_STRUCT state4;
} shake128_x4_ctx;

#define SHAKE_X4_STATE_STRUCT shake128_x4_ctx

static inline
void xof_shake_x4_init(SHAKE_X4_STATE_STRUCT *states)
{
   shake128_inc_init(&(states->state4));  // change order to simulate parallelism
   shake128_inc_init(&(states->state1));
   shake128_inc_init(&(states->state2));
   shake128_inc_init(&(states->state3));
}

static inline
void xof_shake_x4_update(SHAKE_X4_STATE_STRUCT *states,
                      const unsigned char *in1,
                      const unsigned char *in2,
                      const unsigned char *in3,
                      const unsigned char *in4,
                      uint32_t singleInputByteLen)
{
   shake128_inc_absorb(&(states->state4), (const uint8_t *)in4, singleInputByteLen); // change order to simulate parallelism
   shake128_inc_absorb(&(states->state1), (const uint8_t *)in1, singleInputByteLen);
   shake128_inc_absorb(&(states->state2), (const uint8_t *)in2, singleInputByteLen);
   shake128_inc_absorb(&(states->state3), (const uint8_t *)in3, singleInputByteLen);
}

static inline
void xof_shake_x4_final(SHAKE_X4_STATE_STRUCT *states)
{
   shake128_inc_finalize(&(states->state1));
   shake128_inc_finalize(&(states->state2));
   shake128_inc_finalize(&(states->state4));  // change order to simulate parallelism
   shake128_inc_finalize(&(states->state3));
}

static inline
void xof_shake_x4_extract(SHAKE_X4_STATE_STRUCT *states,
                       unsigned char *out1,
                       unsigned char *out2,
                       unsigned char *out3,
                       unsigned char *out4,
                       uint32_t singleOutputByteLen){
   shake128_inc_squeeze(out1, singleOutputByteLen, &(states->state1));
   shake128_inc_squeeze(out2, singleOutputByteLen, &(states->state2));
   shake128_inc_squeeze(out4, singleOutputByteLen, &(states->state4)); // change order to simulate parallelism
   shake128_inc_squeeze(out3, singleOutputByteLen, &(states->state3));
}

static inline
void xof_shake_x4_release(SHAKE_X4_STATE_STRUCT *states)
{
   shake128_inc_ctx_release(&(states->state1));
   shake128_inc_ctx_release(&(states->state4)); // change order to simulate parallelism
   shake128_inc_ctx_release(&(states->state2));
   shake128_inc_ctx_release(&(states->state3));
}

///////////////////////////////////////////////////////////////
//                    SHAKE x2 wrappers                      //
///////////////////////////////////////////////////////////////

typedef struct {
   SHAKE_STATE_STRUCT state1;
   SHAKE_STATE_STRUCT state2;
} shake128_x2_ctx;

#define SHAKE_X2_STATE_STRUCT shake128_x2_ctx

static inline
void xof_shake_x2_init(SHAKE_X2_STATE_STRUCT *states)
{
   shake128_inc_init(&(states->state1));
   shake128_inc_init(&(states->state2));
}

static inline
void xof_shake_x2_update(SHAKE_X2_STATE_STRUCT *states,
                      const unsigned char *in1,
                      const unsigned char *in2,
                      uint32_t singleInputByteLen)
{
   shake128_inc_absorb(&(states->state1), (const uint8_t *)in1, singleInputByteLen);
   shake128_inc_absorb(&(states->state2), (const uint8_t *)in2, singleInputByteLen);
}

static inline
void xof_shake_x2_final(SHAKE_X2_STATE_STRUCT *states)
{
   shake128_inc_finalize(&(states->state1));
   shake128_inc_finalize(&(states->state2));
}

static inline
void xof_shake_x2_extract(SHAKE_X2_STATE_STRUCT *states,
                       unsigned char *out1,
                       unsigned char *out2,
                       uint32_t singleOutputByteLen){
   shake128_inc_squeeze(out1, singleOutputByteLen, &(states->state1));
   shake128_inc_squeeze(out2, singleOutputByteLen, &(states->state2));
}

static inline
void xof_shake_x2_release(SHAKE_X2_STATE_STRUCT *states)
{
   shake128_inc_ctx_release(&(states->state1));
   shake128_inc_ctx_release(&(states->state2));
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

typedef struct {
   SHAKE_STATE_STRUCT state1;
   SHAKE_X2_STATE_STRUCT state2;
   SHAKE_X4_STATE_STRUCT state4;
} par_shake128_ctx;