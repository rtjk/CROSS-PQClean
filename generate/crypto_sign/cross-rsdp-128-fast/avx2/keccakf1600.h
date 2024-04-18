#pragma once

#include <stdint.h>

void PQCLEAN_CROSSRSDP128FAST_AVX2_KeccakF1600_StateExtractBytes(uint64_t *state, unsigned char *data,
                                   uint32_t offset, uint32_t length);
void PQCLEAN_CROSSRSDP128FAST_AVX2_KeccakF1600_StateXORBytes(uint64_t *state, const unsigned char *data,
                               uint32_t offset, uint32_t length);
void PQCLEAN_CROSSRSDP128FAST_AVX2_KeccakF1600_StatePermute(uint64_t *state);
