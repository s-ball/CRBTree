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
	int curpos;
};

static void store(struct EltList* lst, RBNode* node, int depth) {
	struct Elt* elt = lst->elt + lst->count++;
	elt->node = node;
	elt->pos = lst->curpos++;
	elt->depth = depth;
}

static int cmp(const void *a, const void *b) {
	struct Elt* x = (struct Elt*)a;
	struct Elt* y = (struct Elt*)b;
	if (x->depth < y->depth) return -1;
	if (x->depth > y->depth) return 1;
	return (x->pos - y->pos);
}

/**
 * @brief Dumps a tree with parent child relations and node colors.
 * 
 * It uses 2 parameters, the width that will be used to display an element
 * and the function that generates that label. The buffer is internally
 * managed by RBdump and is passed to the user provided function. Undefined
 * Behaviour is to be expected if that function writes more than `elt_width`
 * characters or omits the terminating null...
 * 
 * On output, red nodes have their label followed with a `*`.
 * 
 * @param tree : the tree to dump
 * @param elt_width : the width (in characters) used to display an element
 * @param dump : a user provided function to generate a text label per element
 * @return 
*/
int RBdump(RBTree* tree, size_t elt_width, void(*dump)(void*, char*)) {
	// TODO: implementation of the function
	char* buff = malloc(2 * (elt_width + 1));
	if (buff == NULL) return 0;
	char* blank = buff + elt_width + 1;
	memset(blank, ' ', elt_width);
	blank[elt_width] = 0;
	struct EltList list = { 0 };
	list.sz = tree->count;
	list.elt = malloc(list.sz * sizeof(*list.elt));
	RBIter* iter = RBfirst(tree);
	if (iter == NULL) return 0;
	for (;;) {
		store(&list, iter->elt[iter->curdepth].node,
			iter->curdepth);
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
		for (size_t j = strlen(buff); j < elt_width; j++) buff[j] = ' ';
		if (list.elt[i].node->red) buff[elt_width - 1] = '*';
		fputs(buff, stderr);
		curpos++;
	}
	RBiter_release(iter);
	free(list.elt);
	free(buff);
	return 0;
}