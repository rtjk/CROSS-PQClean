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

#define USE_LIBOQS_SHAKE_1

#ifdef USE_LIBOQS_SHAKE
#include "fips202x4.h"
   #if defined(CATEGORY_1)
      #define LIBOQS_SHAKE_X4_STATE_STRUCT OQS_SHA3_shake128_x4_inc_ctx
      #define SHAKE_X4_INIT OQS_SHA3_shake128_x4_inc_init
      #define SHAKE_X4_ABSORB OQS_SHA3_shake128_x4_inc_absorb
      #define SHAKE_X4_FINALIZE OQS_SHA3_shake128_x4_inc_finalize
      #define SHAKE_X4_SQUEEZE OQS_SHA3_shake128_x4_inc_squeeze
      #define SHAKE_X4_RELEASE OQS_SHA3_shake128_x4_inc_ctx_release
   #else
      #define LIBOQS_SHAKE_X4_STATE_STRUCT OQS_SHA3_shake256_x4_inc_ctx
      #define SHAKE_X4_INIT OQS_SHA3_shake256_x4_inc_init
      #define SHAKE_X4_ABSORB OQS_SHA3_shake256_x4_inc_absorb
      #define SHAKE_X4_FINALIZE OQS_SHA3_shake256_x4_inc_finalize
      #define SHAKE_X4_SQUEEZE OQS_SHA3_shake256_x4_inc_squeeze
      #define SHAKE_X4_RELEASE OQS_SHA3_shake256_x4_inc_ctx_release
   #endif
#endif

typedef struct {
   SHAKE_STATE_STRUCT state1;
   SHAKE_STATE_STRUCT state2;
   SHAKE_STATE_STRUCT state3;
   SHAKE_STATE_STRUCT state4;
} shake_x4_ctx;

#ifdef USE_LIBOQS_SHAKE
#define SHAKE_X4_STATE_STRUCT LIBOQS_SHAKE_X4_STATE_STRUCT
#else
#define SHAKE_X4_STATE_STRUCT shake_x4_ctx
#endif

static inline
void xof_shake_x4_init(SHAKE_X4_STATE_STRUCT *states)
{
#ifdef USE_LIBOQS_SHAKE
   SHAKE_X4_INIT(states);
#else
   xof_shake_init(&(states->state4), 0);  // change order to simulate parallelism
   xof_shake_init(&(states->state1), 0);
   xof_shake_init(&(states->state2), 0);
   xof_shake_init(&(states->state3), 0);
#endif
}

static inline
void xof_shake_x4_update(SHAKE_X4_STATE_STRUCT *states,
                      const unsigned char *in1,
                      const unsigned char *in2,
                      const unsigned char *in3,
                      const unsigned char *in4,
                      uint32_t singleInputByteLen)
{
#ifdef USE_LIBOQS_SHAKE
   SHAKE_X4_ABSORB(states, in1, in2, in3, in4, singleInputByteLen);
#else
   xof_shake_update(&(states->state4), (const uint8_t *)in4, singleInputByteLen); // change order to simulate parallelism
   xof_shake_update(&(states->state1), (const uint8_t *)in1, singleInputByteLen);
   xof_shake_update(&(states->state2), (const uint8_t *)in2, singleInputByteLen);
   xof_shake_update(&(states->state3), (const uint8_t *)in3, singleInputByteLen);
#endif
}

static inline
void xof_shake_x4_final(SHAKE_X4_STATE_STRUCT *states)
{
#ifdef USE_LIBOQS_SHAKE
   SHAKE_X4_FINALIZE(states);
#else
   xof_shake_final(&(states->state1));
   xof_shake_final(&(states->state2));
   xof_shake_final(&(states->state4));  // change order to simulate parallelism
   xof_shake_final(&(states->state3));
#endif
}

static inline
void xof_shake_x4_extract(SHAKE_X4_STATE_STRUCT *states,
                       unsigned char *out1,
                       unsigned char *out2,
                       unsigned char *out3,
                       unsigned char *out4,
                       uint32_t singleOutputByteLen){
#ifdef USE_LIBOQS_SHAKE
   SHAKE_X4_SQUEEZE(out1, out2, out3, out4, singleOutputByteLen, states);
#else
   xof_shake_extract(&(states->state1), out1, singleOutputByteLen);
   xof_shake_extract(&(states->state2), out2, singleOutputByteLen);
   xof_shake_extract(&(states->state4), out4, singleOutputByteLen); // change order to simulate parallelism
   xof_shake_extract(&(states->state3), out3, singleOutputByteLen);
#endif
}

static inline
void xof_shake_x4_release(SHAKE_X4_STATE_STRUCT *states)
{
#ifdef USE_LIBOQS_SHAKE
   SHAKE_X4_RELEASE(states);
#else
   xof_shake_release(&(states->state1));
   xof_shake_release(&(states->state4)); // change order to simulate parallelism
   xof_shake_release(&(states->state2));
   xof_shake_release(&(states->state3));
#endif
}

///////////////////////////////////////////////////////////////
//                    SHAKE x2 wrappers                      //
///////////////////////////////////////////////////////////////

typedef struct {
   SHAKE_STATE_STRUCT state1;
   SHAKE_STATE_STRUCT state2;
} shake_x2_ctx;


#define SHAKE_X2_STATE_STRUCT shake_x2_ctx

static inline
void xof_shake_x2_init(SHAKE_X2_STATE_STRUCT *states)
{
   xof_shake_init(&(states->state1), 0);
   xof_shake_init(&(states->state2), 0);
}

static inline
void xof_shake_x2_update(SHAKE_X2_STATE_STRUCT *states,
                      const unsigned char *in1,
                      const unsigned char *in2,
                      uint32_t singleInputByteLen)
{
   xof_shake_update(&(states->state1), (const uint8_t *)in1, singleInputByteLen);
   xof_shake_update(&(states->state2), (const uint8_t *)in2, singleInputByteLen);
}

static inline
void xof_shake_x2_final(SHAKE_X2_STATE_STRUCT *states)
{
   xof_shake_final(&(states->state1));
   xof_shake_final(&(states->state2));
}

static inline
void xof_shake_x2_extract(SHAKE_X2_STATE_STRUCT *states,
                       unsigned char *out1,
                       unsigned char *out2,
                       uint32_t singleOutputByteLen){
   xof_shake_extract(&(states->state1), out1, singleOutputByteLen);
   xof_shake_extract(&(states->state2), out2, singleOutputByteLen);
}

static inline
void xof_shake_x2_release(SHAKE_X2_STATE_STRUCT *states)
{
   xof_shake_release(&(states->state1));
   xof_shake_release(&(states->state2));
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

typedef struct {
   SHAKE_STATE_STRUCT state1;
   SHAKE_X2_STATE_STRUCT state2;
   SHAKE_X4_STATE_STRUCT state4;
} par_shake_ctx;