#pragma once

#include <stdint.h>

void PQCLEAN_CROSS_CLEAN_KeccakF1600_StateExtractBytes(uint64_t *state, unsigned char *data,
                                   uint16_t offset, uint16_t length);
void PQCLEAN_CROSS_CLEAN_KeccakF1600_StateXORBytes(uint64_t *state, const unsigned char *data,
                               uint16_t offset, uint16_t length);
void PQCLEAN_CROSS_CLEAN_KeccakF1600_StatePermute(uint64_t *state);
