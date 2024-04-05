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

#ifndef __namespace__API_H
#define __namespace__API_H

#include <stdint.h>

// TODO: CRYPTO_ALGNAME, CRYPTO_SECRETKEYBYTES, CRYPTO_PUBLICKEYBYTES, CRYPTO_BYTES, CRYPTO_RANDOMBYTES

#define __namespace__CRYPTO_ALGNAME "CROSS"

/*  no. of bytes of the secret key */
#define __namespace__CRYPTO_SECRETKEYBYTES __length-secret-key__L

/*  no. of bytes of the public key */
#define __namespace__CRYPTO_PUBLICKEYBYTES __length-public-key__L

/* no. of bytes of overhead in a signed message */
#define __namespace__CRYPTO_BYTES __length-signature__L

/* required bytes of input randomness */
#define __namespace__CRYPTO_RANDOMBYTES __random-bytes__


int __namespace__crypto_sign_keypair(unsigned char *pk,
                        unsigned char *sk
                       );

int __namespace__crypto_sign(unsigned char *sm,
                uint64_t *smlen,
                const unsigned char *m,
                uint64_t mlen,
                const unsigned char *sk
               );

int __namespace__crypto_sign_open(unsigned char *m,
                     uint64_t *mlen,
                     const unsigned char *sm,
                     uint64_t smlen,
                     const unsigned char *pk
                    );

int __namespace__crypto_sign_signature(unsigned char *sig,
                uint64_t *siglen,
                const unsigned char *m,
                uint64_t mlen,
                const unsigned char *sk
               );

int __namespace__crypto_sign_verify(const unsigned char *sig,
                uint64_t siglen,
                const unsigned char *m,
                uint64_t mlen,
                const unsigned char *pk
               );
               
#endif
