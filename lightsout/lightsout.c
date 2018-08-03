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

uint64_t power(uint64_t base, uint64_t exp) {
	if (exp == 0)
		return 1;
	else {
		return base * power(base, exp - 1);
	}
}

bool bit_get(uint64_t bv, uint8_t i) {
	REQUIRES(i < BITVECTOR_LIMIT);
	uint64_t out = bv >> i;
	out &= 1;
	return out == 1;
}

bitvector press_button(bitvector bv, uint8_t bit_index, uint8_t width, uint8_t height) {
	bv = bitvector_flip(bv, bit_index);
	row = bit_index / width;
	col = bit_index % width;
	if (row != 0)
		bv = bitvector_flip(bv, bit_index - width);
	if (row != height - 1)
		bv = bitvector_flip(bv, bit_index + width);
	if (col != 0)
		bv = bitvector_flip(bv, bit_index - 1);
	if (col != width - 1)
		bv = bitvector_flip(bv, bit_index + 1);
	return bv;
}

int bruteforce(bitvector bv, uint8_t width, uint8_t height) {
	bitvector bv_orig = bv;
	bitvector solution_if;
	uint64_t solution;
	uint8_t bit_index;
	uint8_t row, col;
	for (solution = 0; solution < power(2, width * height); solution++) {
		bv = bv_orig;
		solution_if = bitvector_new();

		for (bit_index = 0; bit_index < width * height; bit_index++)
			if (bit_get(solution, bit_index))
				solution_if = bitvector_flip(solution_if, bit_index);

		for (bit_index = 0; bit_index < width * height; bit_index++)
			if (bitvector_get(solution_if, bit_index))
				bv = press_button(bv, bit_index, width, height);

		if (bitvector_equal(bv, bitvector_new())) {
			for (bit_index = 0; bit_index < width * height; bit_index++)
				if (bitvector_get(solution_if, bit_index))
					printf("%u:%u\n", bit_index / width, bit_index % width);
			return 0;
		}
	}
	fprintf(stderr, "No solution was found!\n");
	return 1;
}

void elem_free(void *elem) {
	free(elem);
}

int bfsearch(bitvector bv, uint8_t width, uint8_t height) {
	if (bitvector_equal(bv, bitvector_new()))
		return 0;

	board_t b = xmalloc(sizeof(struct board_data));
	b->board = bv;
	b->moves = bitvector_new();

	hdict_t hd = ht_new(width * height);
	queue_t q = queue_new();
	enq(q, b);

	uint8_t row, col, bit_index;

	while (!queue_empty(q)) {
		b = deq(q);
		// Consider all the moves
		for (row = 0; row < height; row++) {
			for (col = 0; col < width; col++) {

				// Press a button
				bitvector newboard = press_button(b->board, get_index(row, col, width, height), width, height);

				// Test if lights out
				if (bitvector_equal(newboard, bitvector_new())) {
					for (bit_index = 0; bit_index < width * height; bit_index++)
						if (bitvector_get(b->moves, bit_index))
							printf("%u:%u\n", bit_index / width, bit_index % width);

					printf("%u:%u\n", row, col);

					hdict_free(hd);
					queue_free(q, &elem_free);
					return 0;
				}

				// Insert newboard into dict if it is not there
				if (ht_lookup(hd, newboard) == NULL) {
					board_t n = xmalloc(sizeof(struct board_data));
					n->board = newboard;
					n->moves = bitvector_flip(b->moves, get_index(row, col, width, height));
					ht_insert(ht, n);
					enq(q, n);
				}
			}
		}
	}

	hdict_free(hd);
	queue_free(q, &elem_free);
	fprintf(stderr, "No solution was found!\n");
	return 1;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <board name>\n", argv[0]);
		return 1;
	}
	char *board_filename = argv[1];

	bitvector bv;
	uint8_t width;
	uint8_t height;

	bool read_successful = file_read(board_filename, &bv, &width, &height);
	if (!read_successful) {
		return 1;
	}

	return bfsearch(bv, width, height);
}
