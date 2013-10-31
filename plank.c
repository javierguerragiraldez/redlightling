
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "plank.h"

typedef struct {
	ptrdiff_t next;
	size_t size;
} empty_node_t;

typedef struct plank_t {
	empty_node_t empty_head;
	uint8_t data[];
} plank_t;


static empty_node_t *nodecast(plank_t *plk, ptrdiff_t offset) {
	return (empty_node_t *)&plk->data[offset];
}


plank_t *plk_create (size_t size) {
	plank_t *p = malloc(sizeof(plank_t) + size);
	p->empty_head.next = 0;
	p->empty_head.size = size;
	return p;
}

void* plk_alloc (plank_t *plk, size_t size) {
	if (!plk)
		return NULL;
	empty_node_t *p = nodecast(plk, p->);
	while (p->size) {
		if (p->size == size) {
			
		}
	}
	return NULL;
}
