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
	EXPORT void RBinit(RBTree* tree, int (*comp)(const void*, const void*));

	/**
	 * @brief Inserts a new element into a valid tree.
	 *
	 * @param tree : the tree where to insert the element
	 * @param data : the element to insert
	 * @param error : a pointer to an int variable which if not NULL
	 *  will be set to 0 if no error and a non zero value if error
	 * @return : the previous element with same key if any or NULL
	*/
	EXPORT void *RBinsert(RBTree* tree, void* data, int *error);
	EXPORT void* RBremove(RBTree* tree, void* key);
	EXPORT void* RBfind(RBTree* tree, void* key);
	EXPORT RBIter* RBsearch(RBTree* tree, void* key);
	EXPORT void* RBnext(RBIter* iter);
	EXPORT void RBiter_release(RBIter* iter);
	EXPORT void RBdestroy(RBTree* tree, void (*dele)(const void*));

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
	EXPORT size_t RBbulk_insert(RBTree* tree, void** data, size_t n,
		void (*dele)( const void *));

	EXPORT int RBvalidate(RBTree* tree);

#ifdef __cplusplus
}
#endif

#endif // !RBTREE_H

