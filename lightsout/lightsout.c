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

unsigned int power(unsigned int base, unsigned int exp) {
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
		for (bit_index = 0; bit_index < width * height; bit_index++) {
			if (bitvector_get(solution_if, bit_index)) {
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
			}
		}
		if (bitvector_equal(bv, bitvector_new())) {
//			printf("\nBoard is solvable.\nSolution bitmap (the squares marked # need to be pushed to solve):\n");
//			print_board(solution, width, height);

			for (bit_index = 0; bit_index < width * height; bit_index++)
				if (bitvector_get(solution_if, bit_index))
					printf("%u:%u\n", bit_index / width, bit_index % width);

			return 0;
		}
	}
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

//	printf("Here's the board we're starting with:\n");
//	print_board(bv,  width, height);

	return bruteforce(bv, width, height);
}
