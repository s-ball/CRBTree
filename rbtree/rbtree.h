#ifndef RBTREE_H
#define RBTREE_H

#ifndef EXPORT
#define EXPORT __declspec(dllimport) 
#endif // EXPORT

#define BLACK_VIOLATION 1
#define RED_VIOLATION 2
#define RED_ROOT 3
#define DEPTH_ERROR 4
#define ORDER_ERROR 5

#ifdef __cplusplus
extern "C" {
#endif

	// Some opaque structures
	typedef struct _RBNode RBNode;
	typedef struct _RBIter RBIter;

	// The main structure
	typedef struct _RBTree {
		RBNode* root;
		unsigned black_depth;
		int (*comp)(const void*, const void*);
	} RBTree;

	// The public interface functions

	// Initializes a new tree given a comparison function.
	EXPORT void RBinit(RBTree* tree, int (*comp)(const void*, const void*));

	// Inserts a new element into a valid tree.
	EXPORT void *RBinsert(RBTree* tree, void* data, int *error);
	EXPORT void* RBremove(RBTree* tree, void* key);
	EXPORT void* RBfind(RBTree* tree, void* key);
	EXPORT RBIter* RBsearch(RBTree* tree, void* key);
	EXPORT void* RBnext(RBIter* iter);

	// Release all resources associated with an iterator.
	EXPORT void RBiter_release(RBIter* iter);

	// Completely cleans a tree.
	EXPORT void RBdestroy(RBTree* tree, void (*dele)(const void*));

	// Inserts an array of elements into a valid tree.
	EXPORT size_t RBbulk_insert(RBTree* tree, void** data, size_t n,
		void (*dele)( const void *));

	// Validates a tree.
	EXPORT int RBvalidate(RBTree* tree);

#ifdef __cplusplus
}
#endif

#endif // !RBTREE_H

