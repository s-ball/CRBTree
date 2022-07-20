#ifndef EXPORT
#define EXPORT __declspec(dllexport)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "rbtree.h"
#include "rbinternal.h"

struct Elt {
	RBNode* node;
	int pos;
	int depth;
};

struct EltList {
	struct Elt* elt;
	size_t sz;
	size_t count;
	size_t curpos;
};

static int store(struct EltList* lst, RBNode* node, int depth) {
	if (lst->count == lst->sz) {
		lst->sz *= 2;
		struct Elt* tmp = realloc(lst->elt, lst->sz * sizeof(struct Elt));
		if (tmp == NULL) {
			free(lst->elt);
			return lst->sz;
		}
		lst->elt = tmp;
	}
	struct Elt* elt = lst->elt + lst->count++;
	elt->node = node;
	elt->pos = lst->curpos++;
	elt->depth = depth;
	return 0;
}

static int cmp(void *a, void *b) {
	struct Elt* x = (struct Elt*)a;
	struct Elt* y = (struct Elt*)b;
	if (x->depth < y->depth) return -1;
	if (x->depth > y->depth) return 1;
	return (x->pos - y->pos);
}

int RBdump(RBTree* tree, size_t elt_width, void(*dump)(void*, char*)) {
	// TODO: implementation of the function
	char* buff = malloc(2 * (elt_width + 1));
	if (buff == NULL) return 0;
	char* blank = buff + elt_width + 1;
	memset(blank, ' ', elt_width);
	blank[elt_width] = 0;
	struct EltList list = { 0 };
	list.sz = 16;
	list.elt = malloc(list.sz * sizeof(*list.elt));
	RBIter* iter = RBfirst(tree);
	if (iter == NULL) return 0;
	for (;;) {
		if (0 != store(&list, iter->elt[iter->curdepth].node,
			iter->curdepth)) {
			RBiter_release(iter);
			free(buff);
			free(list.elt);
			return 0;
		}
		if (NULL == RBnext(iter) || iter->curdepth < 0) break;
	}
	qsort(list.elt, list.count,
		sizeof(*list.elt), cmp);
	size_t depth = -1;
	int curpos;
	for (size_t i = 0; i < list.count; i++) {
		if (list.elt[i].depth != depth) {
			fprintf(stderr, "\n");
			curpos = 0;
		}
		depth = list.elt[i].depth;
		for (; curpos < list.elt[i].pos; curpos++) {
			fputs(blank, stderr);
		}
		dump(list.elt[i].node->data, buff);
		buff[elt_width] = 0;
		for (int j = strlen(buff); j < elt_width; j++) buff[j] = ' ';
		if (list.elt[i].node->red) buff[elt_width - 1] = '*';
		fputs(buff, stderr);
		curpos++;
	}
	RBiter_release(iter);
	free(list.elt);
	free(buff);
	return 0;
}