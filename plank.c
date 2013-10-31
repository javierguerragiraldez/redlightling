
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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
	memset(p->data, 0, sizeof(empty_node_t));
	return p;
}

void* plk_alloc (plank_t *plk, size_t size) {
	if (!plk)
		return NULL;
	empty_node_t *p = &plk->empty_head;
	while (p->size) {
		empty_node_t *nxt = nodecast(plk, p->next);
		if (p->size == size) {
			*p = *nxt;
			return nxt;
		} else if (p->size > size+sizeof(empty_node_t)) {
			empty_node_t *newnode = nodecast(plk, p->next+size);
			*newnode = *nxt;
			p->next = p->next+size;
			p->size = p->size-size;
			return nxt;
		}
		p = nxt;
	}
	return NULL;
}
