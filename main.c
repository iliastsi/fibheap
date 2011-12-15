#include <stdio.h>
#include <stdlib.h>
#include "fibheap.h"

struct fib_heap_data *construct(int key)
{
	struct fib_heap_data *foo;
	foo = malloc(sizeof(struct fib_heap_data));
	foo->key = key;
	return foo;
}

int my_compare(struct fib_heap_data *old, struct fib_heap_data *new)
{
	if(new->key < old->key)
		return 1;
	else
		return -1;
}

int main()
{
	struct fib_heap_data *fdata[10];
	struct fib_heap_data *ret;
	struct fib_heap *H;
	struct fib_heap_node *node;


	fdata[0] = construct(7);
	fdata[1] = construct(15);
	fdata[2] = construct(5);
	fdata[3] = construct(20);
	fdata[4] = construct(2);
	fdata[5] = construct(7);
	fdata[6] = construct(10);

	H = fibheap_init(7, my_compare);
	node = fibheap_insert(H, fdata[0]);
	fibheap_insert(H, fdata[1]);
	fibheap_insert(H, fdata[2]);

	ret = fibheap_extract(H);
	printf("%d\n", ret->key);       // print 5
	free(ret);

	fibheap_insert(H, fdata[3]);
	fibheap_insert(H, fdata[4]);

	fibheap_insert(H, fdata[5]);
	fibheap_insert(H, fdata[6]);

	node->data->key = 8;
	fibheap_decrease(H, node);

	ret = fibheap_extract(H);
	printf("%d\n", ret->key);       // print 2
	free(ret);
	ret = fibheap_extract(H);
	printf("%d\n", ret->key);       // print 7
	free(ret);
	ret = fibheap_extract(H);
	printf("%d\n", ret->key);       // print 8
	free(ret);

	fibheap_destroy(H);

	return 0;
}
