#include <stdio.h>
#include <stdlib.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

void free_int(void* x) {
	if (x != NULL)
		free(x);
}

int main() {

	queue_t Q = queue_new();
	int *in;
	int *out;
	
	for (int i = 0; i < 10; i++) {
		in = xmalloc(sizeof(int));
		*in = i;
		enq(Q, (void*)in);
	}
	queue_reverse(Q);
	for (int i = 9; i >= 0; i--) {
		out = (int*)deq(Q);
		assert(*out == i);
		free(out);
	}
	
	for (int i = 0; i < 20; i++) {
		in = xmalloc(sizeof(int));
		*in = i;
		enq(Q, (void*)in);
	}
	assert(*(int*)queue_peek(Q, 10) == 10);
	
	queue_free(Q, &free_int);

	return 0;
}
