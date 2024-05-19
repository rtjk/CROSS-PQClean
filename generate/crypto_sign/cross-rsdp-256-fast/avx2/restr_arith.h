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

#include "parameters.h"
#include <immintrin.h>

#if defined(RSDP)
#define FZRED_SINGLE(x)   (((x) & 0x07) + ((x) >> 3))
#define FZRED_OPPOSITE(x) ((x) ^ 0x07)
#define FZ_DOUBLE_ZERO_NORM(x) ((x + ((x + 1) >> 3)) & 0x07)

#elif defined(RSDPG)
#define FZRED_SINGLE(x)   (((x) & 0x7f) + ((x) >> 7))
#define FZRED_DOUBLE(x) FZRED_SINGLE(FZRED_SINGLE(x))
#define FZRED_OPPOSITE(x) ((x) ^ 0x7f)
#define FZ_DOUBLE_ZERO_NORM(x) ((x + ((x + 1) >> 7)) & 0x7f)
#endif


static inline
void fz_dz_norm_sigma(FZ_ELEM v[N]){
    for (int i = 0; i < N; i++){
       v[i] = FZ_DOUBLE_ZERO_NORM(v[i]);
    }
}

/* Elements of the restricted subgroups are represented as the exponents of
 * the generator */
static inline
void restr_vec_sub(FZ_ELEM res[N],
                   const FZ_ELEM a[N],
                   const FZ_ELEM b[N]){
    for(int i = 0; i < N; i++){
        res[i]= FZRED_SINGLE( a[i] + FZRED_OPPOSITE(b[i]) );
    }
}

/* Given the choice of q and z, all elements of G are represented as n-elements
 * vectors having powers-of-two in {1,2,...,64}.
 * Employ a round of K&R-HW.
 * Vectorized comb based HW is also employable, testing a word against
 * 0x01...01
 * note that 0x80 is not excluded as sigmas are in double-zero redundant repr.
 */
static inline
int is_fq_vec_in_restr_group(const FQ_ELEM in[N]){
    int is_in_ok = 1;
    for(int i=0; i<N; i++){
        FQ_ELEM tmp;
        tmp = ( in[i]- (FQ_ELEM)1 ) & in[i];
        is_in_ok = is_in_ok && (tmp == 0);
    }
    return is_in_ok;
}

static inline
int is_zz_vec_in_restr_group(const FZ_ELEM in[N]){
    int is_in_ok = 1;
    for(int i=0; i<N; i++){
        is_in_ok = is_in_ok && (in[i] < Z);
    }
    return is_in_ok;
}

#if defined(RSDPG)
/* computes the information word * M_G product to obtain an element of G
 * only non systematic portion of M_G = [W I] is used, transposed to improve
 * cache friendliness */
#if defined(HIGH_PERFORMANCE_X86_64)
static inline
void fz_inf_w_by_fz_matrix(FZ_ELEM res[N],
                           const FZ_ELEM e[M],
                           /* although W_mat is constant, it is dynamically 
                            * expanded, ISO C11 forbids const here */
                           uint16_t W_mat[M][ROUND_UP(N-M,EPI16_PER_REG)]){
    alignas(EPI8_PER_REG)FZ_DOUBLEPREC res_dprec[ROUND_UP(N-M,EPI16_PER_REG)] = {0}; 
    __m256i mred_mask = _mm256_set1_epi16 (0x007f);
    for(int i = 0; i < M; i++){
        for(int j = 0; j < ROUND_UP(N-M,EPI16_PER_REG)/EPI16_PER_REG; j++){
            __m256i res_w = _mm256_load_si256( 
                                 (__m256i const *) &res_dprec[j*EPI16_PER_REG] );
            __m256i e_coeff = _mm256_set1_epi16(e[i]);
            __m256i W_mat_slice = _mm256_lddqu_si256( 
                                 (__m256i const *) &W_mat[i][j*EPI16_PER_REG] );
            __m256i tmp = _mm256_mullo_epi16(e_coeff,W_mat_slice);
            /* Vector Mersenne reduction */
            __m256i tmp2 = _mm256_and_si256 (tmp, mred_mask);
            tmp = _mm256_srli_epi16(tmp, 7);
            tmp = _mm256_add_epi16(tmp,tmp2);
            res_w = _mm256_add_epi16(res_w,tmp);
            /* store back*/
            _mm256_store_si256 ((__m256i *) &res_dprec[j*EPI16_PER_REG], res_w);            
        }
    }

    
    /* Save result trimming to regular precision */
    for(int i = 0; i< N-M; i++) {
        res[i] = FZRED_DOUBLE(res_dprec[i]);
    }         
    memcpy(res+(N-M),e,M*sizeof(FZ_ELEM));
}
#else
static
void fz_inf_w_by_fz_matrix(FZ_ELEM res[N],
                           const FZ_ELEM e[M],
                           FZ_ELEM W_mat[M][N-M]){

    FZ_DOUBLEPREC res_dprec[N-M] = {0}; 
    
    for(int i = 0; i < M; i++){
       for(int j = 0; j < N-M; j++){
           res_dprec[j] += FZRED_SINGLE(
                                  (FZ_DOUBLEPREC) e[i] *
                                  (FZ_DOUBLEPREC) W_mat[i][j]);
       }
    }    
    /* Save result trimming to regular precision */
    for(int i = 0; i< N-M; i++) {
        res[i] = FZRED_DOUBLE(res_dprec[i]);
    }     
    memcpy(res+(N-M),e,M*sizeof(FZ_ELEM));
}
#endif

static inline
void restr_inf_w_sub(FZ_ELEM res[M],
                     const FZ_ELEM a[M],
                     const FZ_ELEM b[M]){
    for(int i = 0; i < M; i++){
        res[i]= FZRED_SINGLE( a[i] + FZRED_OPPOSITE(b[i]) );
    }
}

static inline
int is_zz_inf_w_valid(const FZ_ELEM in[M]){
    int is_in_ok = 1;
    for(int i=0; i<M; i++){
        is_in_ok = is_in_ok && (in[i] < Z);
    }
    return is_in_ok;
}
static inline
void fz_dz_norm_delta(FZ_ELEM v[M]){
    for (int i = 0; i < M; i++){
       v[i] = FZ_DOUBLE_ZERO_NORM(v[i]);
    }
}
#endif
