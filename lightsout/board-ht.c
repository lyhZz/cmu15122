#include <stdio.h>
#include <stdlib.h>
#include "lib/bitvector.h"
#include "lib/boardutil.h"
#include "lib/contracts.h"
#include "lib/hdict.h"
#include "lib/heap.h"
#include "lib/queue.h"
#include "lib/xalloc.h"
#include "board-ht.h"

bool key_equiv(void *x, void *y) {
	REQUIRES(x != NULL && y != NULL);
	return bitvector_equal(*((bitvector*)x), *((bitvector*)y));
}

size_t bit_flip(size_t in, uint8_t i) {
	size_t mask = 1;
	mask = mask << i;
	return in ^ mask;
}

size_t key_hash(void *k) {
	REQUIRES(k != NULL);
	bitvector bv = *(bitvector*)k;
	size_t bv_int = 0;
	int i;
	for (i = 0; i < BITVECTOR_LIMIT; i++)
		if (bitvector_get(bv, i))
			bv_int = bit_flip(bv_int, i);
	return bv_int;
}

void value_free(void *val) {
	free(val);
}

hdict_t ht_new(size_t capacity) {
	REQUIRES(capacity > 0);
	hdict_t hd = hdict_new(capacity, &key_equiv, &key_hash, &value_free);
	ENSURES(hd != NULL);
	return hd;
}

board_t ht_lookup(hdict_t hd, bitvector bv) {
	REQUIRES(hd != NULL);
	return (board_t)hdict_lookup(hd, &bv);
}

void ht_insert(hdict_t hd, board_t dat) {
	REQUIRES(hd != NULL && dat != NULL);
	void *cmp = hdict_insert(hd, &(dat->board), dat);
	ASSERT(cmp == NULL);
}
