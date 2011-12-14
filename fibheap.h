#ifndef FIB_HEAP_H__
#define FIB_HEAP_H__ 0


/*******************************
 * Define structure data types *
 *******************************/

/*
 * This is the data that every node carries.
 * You can define anything here.
 * This data structure is the one that will be
 * used by comparison function to determine
 * the order of the elements inside the
 * Fibonacci Heap (which mean that any posible
 * key should be hear).
 *
 * You have to allocate this on your own but
 * some of the functions here will free it
 * for you (ie. when deleting a node).
 */
struct fib_heap_data {
	int key;
};

/* this is the data type for every node */
struct fib_heap_node {
	struct fib_heap_data *data;
	unsigned int degree;
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
	int (*compr)(struct fib_heap_data *, struct fib_heap_data *);
	unsigned int total_nodes;
	unsigned int max_nodes;
};


/***************************************
 * Function prototypes start from here *
 ***************************************/

/*
 * Initialize the Fibonacci Heap.
 * max is the maximum number of nodes to be helded inside the heap at a time.
 *
 * The contents of the heap are sorted according to a comparison function
 * pointed to by compr, which is called with two arguments that point to
 * the objects being compared.
 * The comparison function mus return an integer less than, equal to, or
 * greater than zero if the second argument  has respectively higher,
 * equal to, or lower priority than the first one . If two members
 * compare as equal, their order in the heap is undefined.
 *
 * For example, in a min heap you will return 1 if (second->key < first->key),
 * 0 if (second->key == first->key) and -1 if (second->key > first->key).
 */
struct fib_heap *
fibheap_init(unsigned int max,
	int (*compr)(struct fib_heap_data *, struct fib_heap_data *));

/*
 * Insert a new node into Fibonacci Heap.
 * Give the fib_heap structure and the data to be inserted.
 * You have to allocate and initialize fib_heap_data by yourself.
 * Returns a pointer to fib_heap_node (for use with
 * fibheap_{increase,decrease} and fibheap_delete).
 */
struct fib_heap_node *
fibheap_insert(struct fib_heap *H, struct fib_heap_data *d);

/*
 * Return the data with the maximum priority from the
 * Fibonacci heap, given the current fib_heap.
 */
struct fib_heap_data *
fibheap_read(struct fib_heap *H);

/*
 * Return the data with the maximum priority from the
 * heap, removing it from the heap at the same time.
 * You have to free fib_heap_data by yourself.
 */
struct fib_heap_data *
fibheap_extract(struct fib_heap *H);

/*
 * Merge two Fibonacci heaps.
 * a and b has to be two different fibonacci heaps with the
 * same compare function but may have different total_length
 * (this way you can increase the maximum number of
 * nodes a heap can take by create a new heap with the desired
 * length and merge this two).
 * The two previous fib_heaps a and b will not be accesible.
 */
struct fib_heap *
fibheap_union(struct fib_heap *a, struct fib_heap *b);

/*
 * Increase the priority of a node.
 * First you change the node in a sense that you increase its
 * priority (ie. for a min heap you decrease its key).
 *
 * Afterwards, you call this function with the node as
 * parameter to re-order the Fibonacci Heap.
 */
void
fibheap_increase(struct fib_heap *H, struct fib_heap_node *node);

/*
 * Like with the previous function, you call fibheap_decrease
 * when the priority of the node has been decreased (ie. for a
 * min heap you increased its key).
 *
 * You can use this function both when increase or decrease the priority
 * of a node (in contrast with fibheap_increase() which can be used
 * only when priority is increasing) but you are discouraged to do so
 * as it unwraps the forest structure to more trees.
 */
void
fibheap_decrease(struct fib_heap *H, struct fib_heap_node *node);

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
