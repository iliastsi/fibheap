#ifndef FIB_HEAP_H__
#define FIB_HEAP_H__ 0


/*******************************
 * Define structure data types *
 *******************************/

/*
 * This is the data that every node carries
 * you can define anything here, but an
 * integer member 'key' must always exists.
 * This is the key used to sort the elements.
 * You have to allocate this on your own but
 * some of the functions here will free it
 * for you (ex. when deleting a node).
 */
struct fib_heap_data {
	int key;
};

/* this is the data type for every node */
struct fib_heap_node {
	struct fib_heap_data *data;
	int degree;
	char marked;
	struct fib_heap_node *parent;
	struct fib_heap_node *child;
	struct fib_heap_node *left;
	struct fib_heap_node *right;
};

/* the main heap data type */
struct fib_heap {
	struct fib_heap_node *the_one;
	struct fib_heap_node **cons_array;
	char type;
	int total_nodes;
	int max_nodes;
};


/***************************************
 * Function prototypes start from here *
 ***************************************/

/*
 * Initialize the Fibonacci Heap.
 * max is the maximum number of nodes to be
 * helded inside the heap at a time.
 * For a min-heap give a zero or negative value to t
 * and for a max-heap t has to be positive
 * Returns a pointer to struct fib_heap
 */
struct fib_heap *
fibheap_init(int max, int t);

/*
 * Insert a new node into Fibonacci Heap.
 * Give the fib_heap structure and the data to be inserted.
 * You have to allocate and initialize fib_heap_data by yourself.
 * Returns a pointer to fib_heap_node (for use with
 * fibheap_change and fibheap_delete).
 */
struct fib_heap_node *
fibheap_insert(struct fib_heap *H, struct fib_heap_data *d);

/*
 * Return the data with the minimum/maximum key from the
 * Fibonacci heap, given the current fib_heap
 */
struct fib_heap_data *
fibheap_read(struct fib_heap *H);

/*
 * Return the data with the minimum/maximum key from the
 * heap, deleting it a the same time.
 * You have to free fib_heap_data by yourself.
 */
struct fib_heap_data *
fibheap_extract(struct fib_heap *H);

/*
 * Merge two Fibonacci heaps.
 * a and b has to be two different fibonacci heaps of
 * the same type (min/max) but may have different total_length
 * (this way you can increase the maximum number of
 * nodes a heap can take by create a new heap with the desired
 * length and merge this two).
 * The two privious fib_heaps a and b
 * will not be accessible.
 */
struct fib_heap *
fibheap_union(struct fib_heap *a, struct fib_heap *b);

/*
 * Change (increase/decrease) the key of a node.
 * Give the current fib_heap, an array to that node
 * and the new key. Only action of decrease(increase)
 * is supported in a minimum(maximum) heap.
 *
 * TODO: support all kind of changes in the key
 */
void
fibheap_change(struct fib_heap *H, struct fib_heap_node *node, int new_key);

/*
 * Delete a node.
 * Give the current fib_heap, and an array to that node.
 * Its fib_heap_data will also be freed.
 */
void
fibheap_delete(struct fib_heap *H, struct fib_heap_node *node);

/*
 * Destroy the given Fibonacci Heap
 * All the fib_heap_data will also be freed.
 */
void
fibheap_destroy(struct fib_heap *H);

#endif /* FIB_HEAP_H__ */
