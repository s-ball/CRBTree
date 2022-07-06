#ifndef EXPORT
#define EXPORT __declspec(dllexport)
#endif

#include "rbtree.h"
#include "rbinternal.h"
#include <stdlib.h>
#include <string.h>

static RBIter* search(RBTree* tree, void* data, int* how) {
	if (0 == tree->black_depth) return NULL;
	int md = 1 + 2 * tree->black_depth;
	RBIter *iter = malloc(sizeof(RBIter) + md * sizeof(struct iter_elt));
	if (NULL == iter) return NULL;
	RBNode* curr = tree->root;
	int_fast8_t side = 0;
	for (int i = 0; i < md; i++) {
		iter->elt[i].node = curr;
		iter->elt[i].right = side;
		int next = tree->comp(data, curr->data);
		if (0 == next) {
			iter->curdepth = i;
			*how = 0;
			break;
		}
		side = (next > 0);
		curr = curr->child[side];
		if (NULL == curr) {
			iter->curdepth = i;
			*how = side ? 1 : -1;
			break;
		}
		// md should be large enough for never falling out of the loop
	}
	return iter;
}

static RBNode* new_node(void* data) {
	RBNode* node = malloc(sizeof(*node));
	if (NULL != node) {
		memset(node->child, 0, sizeof(node->child));
		node->red = 1;
		node->data = data;
	}
	return node;
}

static RBNode* rotate(RBNode* node, int side) {
	RBNode *next = node->child[1 - side];
	node->child[1 - side] = next->child[side];
	next->child[side] = node;
	return next;
}

static RBNode* fix_red_violation(RBIter* iter, int side) {
	for (;;) {
		RBNode* parent = iter->elt[iter->curdepth - 1].node;
		if (parent->child[!iter->elt[iter->curdepth].right] &&
			parent->child[!iter->elt[iter->curdepth].right]->red) {
			// sibling is red: just swap colors
			for (int i = 0; i < 2; i++) parent->child[i]->red = 0;
			parent->red = 1;
		}
		else {
			int curside = iter->elt[iter->curdepth].right;
			// sibling is black: we will have to rotate
			if (side != curside) {
				// we need an additional rotation
				parent->child[1 - side] = rotate(parent->child[1 - side],
					1 - side);
			}
			parent = rotate(parent, 1 - curside);
			parent->child[curside]->red = 0;
			if (iter->curdepth == 1) {
				return parent;
			}
			iter->elt[iter->curdepth - 2].node->child[iter->elt[iter->curdepth - 1].right] = parent;
		}
		if (iter->curdepth > 2) {
			iter->curdepth -= 2;
			if (!iter->elt[iter->curdepth].node->red) break;
		}
		else break;
	}
	return iter->elt[0].node;
}

void RBinit(RBTree* tree, int (*comp)(const void*, const void*)) {
	tree->root = NULL;
	tree->black_depth = 0;
	tree->comp = comp;
}

void RBiter_release(RBIter* iter) {
	free(iter);
}
void * RBinsert(RBTree* tree, void* data, int *error) {
	int how;
	RBIter* iter = search(tree, data, &how);
	if (error) *error = 1; // be conservative
	if (NULL == iter) {
		if (tree->black_depth == 0) {
			tree->root = new_node(data);
			tree->black_depth = 1;
			tree->root->red = 0;
		}
		return NULL;
	}
	void* old = NULL;
	RBNode* node = iter->elt[iter->curdepth].node;
	if (how == 0) {
		old = node->data;
		node->data = data;
	}
	else {
		int side = (how > 0);
		if (NULL == (node->child[side] = new_node(data))) return NULL;
		if (node->red) {
			tree->root = fix_red_violation(iter, side);
		}
	}
	RBiter_release(iter);
	if (error) *error = 0;
	if (tree->root && tree->root->red) {
		tree->root->red = 0;
		tree->black_depth += 1;
	}
	return old;
}

size_t RBbulk_insert(RBTree* tree, void** data, size_t n,
	void (*dele)(const void *)) {
	int inserted = 0, err;
	while (n-- > 0) {
		void* old = RBinsert(tree, *data++, &err);
		if (err) break;
		if (old) dele(old);
		else inserted += 1;
	}
	return inserted;
}

static int node_validate(RBNode *node, int (*comp)(const void *, const void *)) {
	int child_level[2];
	for (int i = 0; i < 2; i++) {
		if (node->child[i] == 0) child_level[i] = 0;
		else {
			if (node->red && node->child[i]->red) return -RED_VIOLATION;
			int delta = comp(node->child[i]->data, node->data);
			if ((delta >= 0 && 0 == i) || (delta <= 0 && 1 == i)) {
				return -ORDER_ERROR;
			}
			int lev = node_validate(node->child[i], comp);
			if (lev < 0) return lev;
			child_level[i] = lev;
		}
	}
	if (child_level[0] != child_level[1]) return -BLACK_VIOLATION;
	return child_level[0] + (!node->red);
}

int RBvalidate(RBTree* tree) {
	if ((0 == tree->black_depth) && (NULL == tree->root)) return 0;
	if ((0 == tree->black_depth) || (NULL == tree->root)) return DEPTH_ERROR;
	if (tree->root->red) return RED_ROOT;
	int lev = node_validate(tree->root, tree->comp);
	if (lev < 0) return -lev;
	if (lev != tree->black_depth) return DEPTH_ERROR;
	return 0;
}