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
	return bitvector_equal(*(bitvector*)x, *(bitvector*)y);
}

size_t key_hash(void *k) {
	REQUIRES(k != NULL);
	return *(bitvector*)k;
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
	board_t in = xmalloc(sizeof(board_t));
	in->board = bv;
	board_t out = (board_t)hdict_lookup(hd, (void*)in);
	free(in);
	return out;
}

void ht_insert(hdict_t hd, board_t dat) {
	REQUIRES(hd != NULL);
	hdict_insert(hd, (void*)(&(dat->board)), (void*)dat);
}
