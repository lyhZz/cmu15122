#include <stdio.h>
#include <stdlib.h>
#include "lib/*.h"

uint32_t bitvector_new() {
    uint32_t bv = 0;
    return bv;
}

bool bitvector_get(uint32_t bv, uint8_t i) {
    uint32_t out = bv >> (31 - i);
	out &= 1;
	return out == 1;
}

bool bitvector_equal(uint32_t bv1, bitvector bv2) {
    return bv1 == bv2;
}

uint32_t bitvector_flip(uint32_t bv, uint8_t i) {
    uint32_t mask = 1;
	mask = mask << (31 - i);
	return bv ^ mask;
}
