# Function Documentation

### RBdestroy()

```
void RBdestroy 	( 	RBTree *  	tree,
		void(*)(const void *)  	dele 
	) 		
```

Completely cleans a tree.

`RBdestroy` removes all nodes from a tree and if `dele` is not null, applies if to any referenced element (intended to free the elements resources).

Parameters

*    tree	: the tree do clean
*    dele	: an optional function that would be applied on evey element

### RBfind

```
EXPORT void* RBfind 	( 	RBTree *  	tree,
		void *  	key 
	) 		
```

Finds an element from a tree and returns it if found or returns `NULL`.

Parameters

*    tree	: the tree where the key is searched
*    key	: the key to be searched

Returns
	: the element for that key 

### RBfirst

```
RBIter* RBfirst 	( 	RBTree *  	tree	) 	
```

Builds an iterator pointing to the first element.

Parameters

*    tree	: the tree which is to be iterated

Returns
	: an iterator pointing to the first element of the tree 

### RBinit

```
void RBinit 	( 	RBTree *  	tree,
		int(*)(const void *, const void *)  	comp 
	) 		
```

Initializes a new tree given a comparison function.

Initialization makes a valid empty tree: the root node is set to `NULL`, the black depth and the count to 0 and the comparison function pointer points to the passed function.

Parameters

*    tree	: pointer to the RBTree to initialize
*    comp	: the comparison function

### RBinit2

```
void RBinit 	( 	RBTree *  	tree,
		int(*)(const void *, const void *, int *)  	comp 
	) 		
```

Initializes a new tree given a comparison function handling errors.

Initialization makes a valid empty tree: the root node is set to `NULL`, the black depth and the count to 0 and the 3 args comparison function pointer points to the passed function.

Parameters

*    tree	: pointer to the RBTree to initialize
*    comp	: the comparison function

### RBinsert

```
void* RBinsert 	( 	RBTree *  	tree,
		void *  	data,
		int *  	error 
	) 		
```

Inserts a new element into a valid tree.

Parameters

*    tree	: the tree where to insert the element
*    data	: the element to insert
*    error	: a pointer to an int variable which if not `NULL` will be set to 0 if no error and a non zero value if error

Returns
	: the previous element with same key if any or `NULL` 

### RBiter_release

```
void RBiter_release 	( 	RBIter *  	iter	) 	
```

Releases all resources associated with an iterator.

Parameters

*    iter	: the iterator to release

### RBnext

```
void* RBnext 	( 	RBIter *  	iter	) 	
```

Returns the currently pointed element and advances the iterator.

Parameters

*    iter	: the iterator

Returns
	: the currently pointed element 

### RBremove

```
void* RBremove 	( 	RBTree *  	tree,
		void *  	key 
	) 		
```

Removes an element from a tree and returns it.

Parameters

*    tree	: the tree to search
*    key	: the key for which an element is to be retrieved

Returns
	: NULL if the key could not be found or the removed element 

### RBsearch

```
RBIter* RBsearch 	( 	RBTree *  	tree,
		void *  	key 
	) 		
```

Searches a tree from a key and returns an iterator positioned there.

If the key is found in the tree, the next call of RBnext on the iterator will return the element for that key. If it is not found, the next call will return the first element with a greater key.

Parameters
*    tree	: the tree where the key is searched
*    key	: the key to be searched

Returns
	: an iterator positioned at that key 

### RBvalidate

```
int RBvalidate 	( 	RBTree *  	tree	) 	
```

Validates a tree.

RBvalidate controls that a tree is correctly ordered, contains neither red nor black violation and that its black_depth and count are correct.

Parameters

*    tree	: the tree to validate

Returns
	: 0 if the tree is correct or a (non-zero) error code 

### RBversion

```
const unsigned char* RBversion 	( 		) 	
```

Returns the current library version as 4 bytes.

Returns
	a pointer to a 4 bytes array major, minor, patch, level 
