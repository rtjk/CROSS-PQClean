#pragma once

#include <stdint.h>

void PQCLEAN_CROSSRSDPG128BALANCED_CLEAN_KeccakF1600_StateExtractBytes(uint64_t *state, unsigned char *data,
                                   uint32_t offset, uint32_t length);
void PQCLEAN_CROSSRSDPG128BALANCED_CLEAN_KeccakF1600_StateXORBytes(uint64_t *state, const unsigned char *data,
                               uint32_t offset, uint32_t length);
void PQCLEAN_CROSSRSDPG128BALANCED_CLEAN_KeccakF1600_StatePermute(uint64_t *state);
