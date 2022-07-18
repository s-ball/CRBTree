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

static void iter_push(RBIter* iter, RBNode* node, int side) {
	iter->elt[++iter->curdepth].node = node;
	iter->elt[iter->curdepth].right = side;
}

RBIter* RBfirst(RBTree* tree) {
	if (0 == tree->black_depth) return NULL;
	int md = 1 + 2 * tree->black_depth;
	RBIter* iter = malloc(sizeof(RBIter) + md * sizeof(struct iter_elt));
	if (NULL == iter) return NULL;
	RBNode* curr = tree->root;
	for (int i = 0; i < md; i++) {
		iter->elt[i].node = curr;
		iter->elt[i].right = 0;
		iter->curdepth = i;
		if (curr->child[0]) curr = curr->child[0];
		else break;
	}
	return iter;
}

void* RBnext(RBIter* iter) {
	if (iter->curdepth == -1) return NULL;
	RBNode* node = iter->elt[iter->curdepth].node;
	void* data = node->data;
	if (node->child[1]) {
		node = node->child[1];
		iter_push(iter, node, 1);
		while (node->child[0]) {
			node = node->child[0];
			iter_push(iter, node, 0);
		}
	}
	else {
		while (iter->elt[iter->curdepth--].right);
	}
	return data;
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
			parent->child[1-curside]->red = 1;
			parent->red = 0;
			if (iter->curdepth == 1) {
				return parent;
			}
			iter->elt[iter->curdepth - 2].node->child[iter->elt[iter->curdepth - 1].right] = parent;
			break;
		}
		if (iter->curdepth > 2) {
			iter->curdepth -= 2;
			if (!iter->elt[iter->curdepth].node->red) break;
			side = iter->elt[iter->curdepth + 1].right;
		}
		else break;
	}
	return iter->elt[0].node;
}

/**
 * @brief Initializes a new tree given a comparison function.
 *
 * Initialization makes a valid empty tree: the root node is set to NULL,
 * the black depth to 0 and the comparison function pointer points to the
 * passed function.
 *
 * @param tree : pointer to the RBTree to initialize
 * @param comp : the comparison function
*/
void RBinit(RBTree* tree, int (*comp)(const void*, const void*)) {
	tree->root = NULL;
	tree->black_depth = 0;
	tree->count = 0;
	tree->comp = comp;
}

/**
 * @brief Release all resources associated with an iterator.
 *
 * @param iter : the iterator to release
*/
void RBiter_release(RBIter* iter) {
	free(iter);
}

static void node_destroy(RBNode* node, void (*dele)(const void *)) {
	if (NULL == node) return;
	node_destroy(node->child[0], dele);
	node_destroy(node->child[1], dele);
	if (dele) dele(node->data);
	free(node);
}

/**
 * @brief Completely cleans a tree.
 *
 * RBdestroy removes all nodes from a tree and if dele is not null, applies
 * if to any referenced element (intended to free the elements resources).
 *
 * @param tree
 * @param dele
*/
void RBdestroy(RBTree* tree, void (*dele)(const void*)) {
	node_destroy(tree->root, dele);
	tree->root = NULL;
	tree->black_depth = 0;
}

/**
 * @brief Inserts a new element into a valid tree.
 *
 * @param tree : the tree where to insert the element
 * @param data : the element to insert
 * @param error : a pointer to an int variable which if not NULL
 *  will be set to 0 if no error and a non zero value if error
 * @return : the previous element with same key if any or NULL
*/
void * RBinsert(RBTree* tree, void* data, int *error) {
	int how;
	RBIter* iter = search(tree, data, &how);
	if (error) *error = 1; // be conservative
	if (NULL == iter) {
		if (tree->black_depth == 0) {
			tree->root = new_node(data);
			tree->black_depth = 1;
			tree->count = 1;
			tree->root->red = 0;
			if (error) *error = 0;
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
	if (!old) tree->count += 1;
	return old;
}

#ifdef _TEST
EXPORT
#else
static
#endif // !_TEST
RBNode* paint_child_red(RBNode* node, int side) {
	RBNode* child = node->child[side];
	child->red = 1;
	if ((! child->child[side] || ! child->child[side]->red)
		&& child->child[1-side] && child->child[1-side]->red) {
		// additional rotation
		child = node->child[side] = rotate(child, side);
	}
	if (child->child[side] && child->child[side]->red) {
		node = rotate(node, 1 - side);
		node->child[side]->red = 0;
	}
	return node;
}

void* RBremove(RBTree* tree, void* key) {
	int how;
	RBIter* iter = search(tree, key, &how);
	if (how != 0) return NULL;
	RBNode * node = iter->elt[iter->curdepth].node;
	void *data = node->data;
	void** old = &(node->data);
	RBNode* child;
	if (node->child[1] != NULL) {
		node = node->child[1];
		iter_push(iter, node, 1);
		while (node->child[0] != NULL) {
			node = node->child[0];
			iter_push(iter, node, 0);
		}
		child = node->child[1];
		*old = node->data;
	}
	else {
		child = node->child[0];
	}
	if (0 == iter->curdepth) {
		tree->root = child;
		tree->black_depth -= 1;
	}
	else {
		iter->elt[iter->curdepth - 1].node->child[iter->elt[
			iter->curdepth].right] = child
		;
		// handle a possible black violation.
		if (node->child[1] && node->child[1]->red) {
			node->child[1]->red = 0;
		}
		else if (0 == node->red) {
			int done = 0;
			while (! done) {
				int side = iter->elt[iter->curdepth].right;
				iter->curdepth -= 1;
				node = iter->elt[iter->curdepth].node;
				if (node->red) {
					// found a red ancestor
					node->red = 0;
					node = paint_child_red(node, 1 - side);
					done = 1;
				}
				else if (node->child[1 - side]->red) {
					// found a red sibling
					RBNode* old = node;
					node = rotate(node, side);
					node->red = 0;
					node->child[side] = paint_child_red(
						old, 1 - side);
					done = 1;
				}
				else {
					node = paint_child_red(node, 1 - side);
					if (node->red) {
						node->red = 0;
						done = 1;
					}
				}
				if (iter->curdepth > 0) {
					iter->elt[iter->curdepth - 1].node->child[
						iter->elt[iter->curdepth].right] = node;
				}
				else {
					tree->root = node;
					if (!done) tree->black_depth -= 1;
					done = 1;
				}
			}
		}
	}
	// handle a possible red root
	if (tree->root && tree->root->red) {
		tree->root->red = 0;
		tree->black_depth += 1;
	}
	tree->count -= 1;
	return data;
}

/**
 * @brief Inserts an array of elements into a valid tree.
 *
 * Inserts a number of elements in one single call. The returned value
 * is the number of elements that did not pre-exist in the tree. If an
 * element is replaced and if dele is not NULL, it is called with that
 * element as parameter.
 *
 * @param tree : the tree where to insert the elements
 * @param data : an array of elements
 * @param n : number of elements in data
 * @param dele : an optional function to release replaced elements or NULL
 * @return : the number of new elements inserted in the tree
*/
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

static int node_validate(RBNode *node, int *total, int (*comp)(const void *, const void *)) {
	int child_level[2];
	*total += 1;
	for (int i = 0; i < 2; i++) {
		if (node->child[i] == 0) child_level[i] = 0;
		else {
			if (node->red && node->child[i]->red) return -RED_VIOLATION;
			int delta = comp(node->child[i]->data, node->data);
			if ((delta >= 0 && 0 == i) || (delta <= 0 && 1 == i)) {
				return -ORDER_ERROR;
			}
			int lev = node_validate(node->child[i], total, comp);
			if (lev < 0) return lev;
			child_level[i] = lev;
		}
	}
	if (child_level[0] != child_level[1]) return -BLACK_VIOLATION;
	return child_level[0] + (!node->red);
}

/**
 * @brief Validates a tree.
 *
 * RBvalidate controls that a tree is correctly ordered, contains neither
 * red nor black violation and that its black_depth is correct.
 *
 * @param tree : the tree to validate
 * @return : 0 if the tree is correct or a (non-zero) error code
*/
int RBvalidate(RBTree* tree) {
	if ((0 == tree->black_depth) && (NULL == tree->root)) return 0;
	if ((0 == tree->black_depth) || (NULL == tree->root)) return DEPTH_ERROR;
	if (tree->root->red) return RED_ROOT;
	int total = 0;
	int lev = node_validate(tree->root, &total, tree->comp);
	if (lev < 0) return -lev;
	if (lev != tree->black_depth) return DEPTH_ERROR;
	if (total != tree->count) return COUNT_ERROR;
	return 0;
}
