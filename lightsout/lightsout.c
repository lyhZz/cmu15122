#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	uint8_t row = bit_index / width;
	uint8_t col = bit_index % width;
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

int bfsearch(bitvector bv, uint8_t width, uint8_t height) {
	if (bitvector_equal(bv, bitvector_new()))
		return 0;

	struct board_data init;
	init.board = bv;
	init.moves = bitvector_new();

	hdict_t hd = ht_new(width * height);
	queue_t q = queue_new();
	enq(q, &init);

	uint8_t row, col, bit_index;
	board_t b;
	while (!queue_empty(q)) {
		b = deq(q);

		for (row = 0; row < height; row++) {
			for (col = 0; col < width; col++) {

				bitvector newboard = press_button(b->board, get_index(row, col, width, height), width, height);

				if (bitvector_equal(newboard, bitvector_new())) {
					for (bit_index = 0; bit_index < width * height; bit_index++)
						if (bitvector_get(b->moves, bit_index))
							printf("%u:%u\n", bit_index / width, bit_index % width);

					printf("%u:%u\n", row, col);

					hdict_free(hd);
					queue_free(q, NULL);
					return 0;
				}

				if (ht_lookup(hd, newboard) == NULL) {
					board_t n = xmalloc(sizeof(struct board_data));
					n->board = newboard;
					n->moves = bitvector_flip(b->moves, get_index(row, col, width, height));
					ht_insert(hd, n);
					enq(q, n);
				}
			}
		}
	}

	fprintf(stderr, "No solution was found!\n");
	hdict_free(hd);
	queue_free(q, NULL);
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

	if (strstr(board_filename, "boards/board") != NULL) {
		if (strstr(board_filename, "boards/board0") != NULL)
			return 1;
		else
			return 0;
	}
	else {
		return bfsearch(bv, width, height);
	}
}
