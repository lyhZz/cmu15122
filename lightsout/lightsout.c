#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib/bitvector.h"
#include "lib/boardutil.h"
#include "lib/contracts.h"
#include "lib/hdict.h"
#include "lib/heap.h"
#include "lib/queue.h"
#include "lib/xalloc.h"
#include "board-ht.h"

int bruteforce(bitvector bv, uint8_t width, uint8_t height) {
    bitvector solution, bv_orig = bv;
    uint8_t bit_index;
    uint8_t row, col;
    for (solution = 0; solution < pow(2, width * height); solution++) {
        bv = bv_orig;
        for (bit_index = 0; bit_index < width * height; bit_index++) {
            if (bitvector_get(solution, bit_index)) {
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
        if (bv == 0) {
            printf("\nBoard is solvable.\nSolution bitmap (the squares marked # need to be pushed to solve):\n");
            print_board(solution, width, height);
            return 0;
        }
    }
    printf("\nNo solution was found!\n");
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

    printf("Here's the board we're starting with:\n");
    print_board(bv,  width, height);

    return bruteforce(bv, width, height);
}
