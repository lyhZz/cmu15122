#include <stdio.h>
#include <stdlib.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

typedef struct list_node list;
struct list_node {
	void *data;
	list *next;
};

typedef struct queue_header queue;
struct queue_header {
	list *front;
	list *back;
	size_t size;
};

bool is_inclusive_segment(list *start, list *end, size_t length) {
	if (start == NULL)
		return length == 0;
	else if (start == end)
		return start->next == NULL && length == 1;
	else
		return is_inclusive_segment(start->next, end, length - 1);
}

bool is_queue(queue_t Q) {
	return Q != NULL && is_inclusive_segment(Q->front, Q->back, Q->size);
}

queue_t queue_new() {
	queue_t Q = xmalloc(sizeof(queue));
	Q->front = NULL;
	Q->back = NULL;
	Q->size = 0;
	ENSURES(is_queue(Q));
	return Q;
}

size_t queue_size(queue_t Q) {
	REQUIRES(is_queue(Q));
	ENSURES(is_queue(Q));
	return Q->size;
}

void enq(queue_t Q, void *x) {
	REQUIRES(is_queue(Q));
	list *node = xmalloc(sizeof(list));
	node->data = x;
	node->next = NULL;
	if (queue_size(Q) == 0)
		Q->front = node;
	else
		Q->back->next = node;
	Q->back = node;
	Q->size++;
	ENSURES(is_queue(Q));
}

void* deq(queue_t Q) {
	REQUIRES(is_queue(Q) && queue_size(Q) > 0);
	list *tmp = Q->front;
	void *out = tmp->data;
	Q->front = Q->front->next;
	Q->size--;
	free(tmp);
	ENSURES(is_queue(Q));
	return out;
}

void* queue_peek(queue_t Q, size_t i) {
	REQUIRES(is_queue(Q) && i < queue_size(Q));
	list *current = Q->front;
	for (size_t ctr = 0; ctr < i; ctr++)
		current = current->next;
	ENSURES(is_queue(Q));
	return current->data;
}

void queue_reverse(queue_t Q) {
	REQUIRES(is_queue(Q));
	if (queue_size(Q) <= 1) {}
	else if (queue_size(Q) == 2) {
		list *tmp = Q->front;
		Q->front = Q->back;
		Q->back = tmp;
		Q->front->next = Q->back;
		Q->back->next = NULL;
	}
	else {
		list *last = Q->front;
		list *current = last->next;
		list *next = current->next;
		while (current != NULL) {
			next = current->next;
			current->next = last;
			last = current;
			current = next;
		}
		list *orig_back = Q->back;
		Q->back = Q->front;
		Q->front = orig_back;
		Q->back->next = NULL;
	}
	ENSURES(is_queue(Q));
	return;
}

bool queue_all(queue_t Q, check_property_fn *p) {
	REQUIRES(is_queue(Q) && p != NULL);
	if (queue_size(Q) <= 0)
		return true;
	list *current = Q->front;
	while (current != NULL) {
		if (!(*p)(current->data))
			return false;
		current = current->next;
	}
	return true;
}

void* queue_iterate(queue_t Q, void* base, iterate_fn *f) {
	REQUIRES(is_queue(Q) && f != NULL);
	if (queue_size(Q) <= 0)
		return base;
	list *current = Q->front;
	void *out = base;
	while (current != NULL) {
		out = (*f)(out, current->data);
		current = current->next;
	}
	return out;
}

void queue_free(queue_t Q, free_fn *f) {
	REQUIRES(Q != NULL);
	if (f == NULL)
		free(Q);
	else {
		while (queue_size(Q) > 0) {
			void* v = deq(Q);
			(*f)(v);
		}
		free(Q);
	}
}
