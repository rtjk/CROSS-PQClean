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

#ifndef __namespace__API_H
#define __namespace__API_H

#pragma once

#include <stddef.h>
#include <stdint.h>

#define __namespace__CRYPTO_ALGNAME "__dir__"

/*  no. of bytes of the secret key */
#define __namespace__CRYPTO_SECRETKEYBYTES __length-secret-key__

/*  no. of bytes of the public key */
#define __namespace__CRYPTO_PUBLICKEYBYTES __length-public-key__

/* no. of bytes of overhead in a signed message */
#define __namespace__CRYPTO_BYTES __length-signature__

/* required bytes of input randomness */
#define __namespace__CRYPTO_RANDOMBYTES __random-bytes__


int __namespace__crypto_sign_keypair(unsigned char *pk,
                        unsigned char *sk
                       );

int __namespace__crypto_sign(unsigned char *sm,
                size_t *smlen,
                const unsigned char *m,
                size_t mlen,
                const unsigned char *sk
               );

int __namespace__crypto_sign_open(unsigned char *m,
                     size_t *mlen,
                     const unsigned char *sm,
                     size_t smlen,
                     const unsigned char *pk
                    );

int __namespace__crypto_sign_signature(unsigned char *sig,
                size_t *siglen,
                const unsigned char *m,
                size_t mlen,
                const unsigned char *sk
               );

int __namespace__crypto_sign_verify(const unsigned char *sig,
                size_t siglen,
                const unsigned char *m,
                size_t mlen,
                const unsigned char *pk
               );
               
#endif
