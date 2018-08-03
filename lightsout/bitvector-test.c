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

void main() {
    bitvector test = bitvector_new();
    int i;
    for (i = 0; i < 32; i++)
        assert(!bitvector_get(test, i));
    for (i = 0; i < 32; i++) {
        test = bitvector_flip(test, i);
        assert(bitvector_get(test, i));
        test = bitvector_flip(test, i);
        assert(!bitvector_get(test, i));
    }
    test = bitvector_flip(test, 9);
    assert(!bitvector_get(test, 6));
    assert(bitvector_get(test, 9));
}
