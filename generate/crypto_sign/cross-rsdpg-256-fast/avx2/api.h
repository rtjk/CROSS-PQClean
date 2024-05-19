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

#ifndef PQCLEAN_CROSSRSDPG256FAST_AVX2_API_H
#define PQCLEAN_CROSSRSDPG256FAST_AVX2_API_H

#include <stdint.h>

#define PQCLEAN_CROSSRSDPG256FAST_AVX2_CRYPTO_ALGNAME "cross-rsdpg-256-fast"

/*  no. of bytes of the secret key */
#define PQCLEAN_CROSSRSDPG256FAST_AVX2_CRYPTO_SECRETKEYBYTES 64L

/*  no. of bytes of the public key */
#define PQCLEAN_CROSSRSDPG256FAST_AVX2_CRYPTO_PUBLICKEYBYTES 106L

/* no. of bytes of overhead in a signed message */
#define PQCLEAN_CROSSRSDPG256FAST_AVX2_CRYPTO_BYTES 48938L

/* required bytes of input randomness */
#define PQCLEAN_CROSSRSDPG256FAST_AVX2_CRYPTO_RANDOMBYTES 32


int PQCLEAN_CROSSRSDPG256FAST_AVX2_crypto_sign_keypair(unsigned char *pk,
                        unsigned char *sk
                       );

int PQCLEAN_CROSSRSDPG256FAST_AVX2_crypto_sign(unsigned char *sm,
                uint64_t *smlen,
                const unsigned char *m,
                uint64_t mlen,
                const unsigned char *sk
               );

int PQCLEAN_CROSSRSDPG256FAST_AVX2_crypto_sign_open(unsigned char *m,
                     uint64_t *mlen,
                     const unsigned char *sm,
                     uint64_t smlen,
                     const unsigned char *pk
                    );

int PQCLEAN_CROSSRSDPG256FAST_AVX2_crypto_sign_signature(unsigned char *sig,
                uint64_t *siglen,
                const unsigned char *m,
                uint64_t mlen,
                const unsigned char *sk
               );

int PQCLEAN_CROSSRSDPG256FAST_AVX2_crypto_sign_verify(const unsigned char *sig,
                uint64_t siglen,
                const unsigned char *m,
                uint64_t mlen,
                const unsigned char *pk
               );
               
#endif
