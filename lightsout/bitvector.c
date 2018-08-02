#include <stdio.h>
#include <stdlib.h>
#include "lib/*.h"

bitvector bitvector_new() {
	bitvector bv = 0;
	return bv;
}

bool bitvector_get(bitvector bv, uint8_t i) {
	REQUIRES(0 <= i && i < BITVECTOR_LIMIT);
	bitvector out = bv >> (31 - i);
	out &= 1;
	return out == 1;
}

bool bitvector_equal(bitvector bv1, bitvector bv2) {
	return bv1 == bv2;
}

bitvector bitvector_flip(bitvector bv, uint8_t i) {
	REQUIRES(0 <= i && i < BITVECTOR_LIMIT);
	bitvector mask = 1;
	mask = mask << (31 - i);
	return bv ^ mask;
}
