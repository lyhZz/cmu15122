#include <stdio.h>
#include <stdlib.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

int main() {
	queue_t Q = queue_new();
	
	int* v;
	
	for (int i = 0; i < 10; i++) {
		v = malloc(sizeof(int));
		if (v == NULL) {
			fprintf(stderr, "Allocation failed!\n");
			abort();
		}
		*v = i;
		enq(Q, (void*)v);
	}
	queue_reverse(Q);
	for (int i = 9; i >= 0; i--) {
		int* a = (int*)deq(Q);
		assert(*a == i);
		free(a);
	}
	for (int i = 0; i < 20; i++) {
		v = malloc(sizeof(int));
		if (v == NULL) {
			fprintf(stderr, "Allocation failed!\n");
			abort();
		}
		*v = i;
		enq(Q, (void*)v);
	}
	assert(*(int*)queue_peek(Q, 10) == 10);
	
	while (queue_size(Q) > 0) {
		v = (int*)deq(Q);
		free(v);
	}
	free(Q);
/*	
	for (int i = -10; i < 10; i++) {
		v = (int*)malloc(sizeof(int));
		*v = i;
		enq(Q, (void*)v);
	}
	assert(!queue_all(Q, &even));
	assert(!queue_all(Q, &odd));
	
	while (queue_size(Q) > 0)
		deq(Q);
	
	v = alloc(int);
	*v = 2;
	enq(Q, (void*)v);
	
	v = alloc(int);
	*v = 0;
	enq(Q, (void*)v);
	
	v = alloc(int);
	*v = 4;
	enq(Q, (void*)v);
	
	v = alloc(int);
	*v = 6;
	enq(Q, (void*)v);
	
	assert(queue_all(Q, &even));
	assert(!queue_all(Q, &odd));
	
	while (queue_size(Q) > 0)
		deq(Q);
	
	v = alloc(int);
	*v = 1;
	enq(Q, (void*)v);
	
	v = alloc(int);
	*v = 7;
	enq(Q, (void*)v);
	
	v = alloc(int);
	*v = 3;
	enq(Q, (void*)v);
	
	assert(!queue_all(Q, &even));
	assert(queue_all(Q, &odd));
	
	assert(queue_all(Q, &incr));
	
	int* b = (int*)deq(Q);
	assert(*b == 2);
	
	b = (int*)deq(Q);
	assert(*b == 8);
	
	b = (int*)deq(Q);
	assert(*b == 4);
	
	for (int i = -10; i < 10; i += 4) {
		v = alloc(int);
		*v = i;
		enq(Q, (void*)v);
	}
	
	int* c = (int*)queue_iterate(Q, NULL, &find_negative);
	assert(*c == -10);
	
	queue_t P = (queue_t)queue_iterate(Q, (void*)queue_new(), &copy);
	while (queue_size(P) > 0 && queue_size(Q) > 0)
		assert(deq(P) == deq(Q));
*/
	return 0;
}
