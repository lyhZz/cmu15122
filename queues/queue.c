#include <stdio.h>
#include <stdlib.h>
#include "lib/contracts.h"
#include "lib/xalloc.h"
#include "queue.h"

typedef struct queue_header* queue_t;
typedef bool check_property_fn(void* x);
typedef void* iterate_fn(void* accum, void* x);

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
	if (length < 0)
		return false;
	else if (start == NULL)
		return length == 0;
	else if (start == end)
		return start->next == NULL && length == 1;
	else
		return is_inclusive_segment(start->next, end, length - 1);
}

bool is_queue(queue_t Q) {
	return Q != NULL && is_inclusive_segment(Q->front, Q->back, Q->size);
}

queue_t queue_new()
//@ensures \result != NULL && is_queue(\result);
{
	queue_t Q = (queue_t)malloc(sizeof(queue));
	Q->front = NULL;
	Q->back = NULL;
	Q->size = 0;
	return Q;
}

size_t queue_size(queue_t Q)
//@requires is_queue(Q);
//@ensures is_queue(Q) && \result >= 0;
{
	return Q->size;
}

void enq(queue_t Q, void *x)
//@requires is_queue(Q);
//@ensures is_queue(Q);
{
	list *node = (list*)malloc(sizeof(list));
	node->data = x;
	node->next = NULL;
	if (queue_size(Q) == 0)
		Q->front = node;
	else
		Q->back->next = node;
	Q->back = node;
	Q->size++;
}

void* deq(queue_t Q)
//@requires is_queue(Q) && queue_size(Q) > 0;
//@ensures is_queue(Q);
{
	list *tmp = Q->front;
	void *out = tmp->data;
	Q->front = Q->front->next;
	Q->size--;
	free(tmp);
	return out;
}

void* queue_peek(queue_t Q, size_t i)
//@requires is_queue(Q) && 0 <= i && i < queue_size(Q);
//@ensures is_queue(Q);
{
	list *current = Q->front;
	for (size_t ctr = 0; ctr < i; ctr++)
		current = current->next;
	return current->data;
}

void queue_reverse(queue_t Q)
//@requires is_queue(Q);
//@ensures is_queue(Q);
{
	if (queue_size(Q) <= 1)
		return;
	else if (queue_size(Q) == 2) {
		list *tmp = Q->front;
		Q->front = Q->back;
		Q->back = tmp;
		Q->front->next = Q->back;
		Q->back->next = NULL;
		return;
	}
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

bool queue_all(queue_t Q, check_property_fn *P)
//@requires is_queue(Q) && P != NULL;
//@ensures is_queue(Q);
{
	if (queue_size(Q) <= 0)
		return true;
	list *current = Q->front;
	while (current != NULL) {
		if (!(*P)(current->data))
			return false;
		current = current->next;
	}
	return true;
}

void* queue_iterate(queue_t Q, void* base, iterate_fn *f)
//@requires is_queue(Q) && f != NULL;
//@ensures is_queue(Q);
{
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
